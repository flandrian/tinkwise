template <class T>
int append_data(void** buffer, uint8_t meaning_code, T data);

#include <avr/sleep.h>
#include <avr/wdt.h>
#include "VirtualWire.h"
#include "DHT.h"

#define MYID 0      //the ID number of this board.  Change this for each board you flash.
                    //The ID will be transmitted with the data so you can tell which device is transmitting
#define TRANSPIN 2  //what pin to transmit on

#define TEMPERATURE_CODE 1

//LM35 Pin Variables
int sensorPin = 6;        // the analog pin the LM35's Vout (sense) pin is connected to
int sensorSupplyPin = 10;  // the digital pin that the LM35's supply is connected to

int wakeupIndex = 0;
int wakeupCount = 70; // one deep sleep phase is about 8s                                    

// watchdog interrupt//
ISR (WDT_vect) 
{
  wdt_disable();                   // disable watchdog//
}  

void setup() {
  Serial.begin(115200); 
  randomSeed(analogRead(A0));  //initialize the random number generator with
                               //a random read from an unused and floating analog port
  //initialize the virtual wire library
  vw_set_ptt_inverted(true); // Required for DR3100
  vw_set_tx_pin(TRANSPIN);
  vw_setup(2000);  //keep the data rate low for better reliability and range
  
  pinMode(sensorSupplyPin, OUTPUT);      // sets the digital pin as output
}

int ftoa(char *a, float f)  //translates floating point readings into strings to send over the air
{
  int left=int(f);
  float decimal = f-left;
  int right = decimal *100; //2 decimal points
  if (right > 10) {  //if the decimal has two places already. Otherwise
    sprintf(a, "%d.%d",left,right);
  } else { 
    sprintf(a, "%d.0%d",left,right); //pad with a leading 0
  }
}

int xmitMessage(char *msg, int msg_len){
    digitalWrite(13, true); // Flash an led to show transmitting
    vw_send((uint8_t *)msg, msg_len);
    vw_wait_tx(); // Wait until the whole message is gone
    digitalWrite(13, false);
}

void measureAndSend()
{
    char message[50];

    digitalWrite(sensorSupplyPin, HIGH);    // enable supply for lm35
    delay(1); //let sensor input settle
    int reading = analogRead(sensorPin);    // getting the voltage reading from the temperature sensor
    digitalWrite(sensorSupplyPin, LOW);    // disable supply for lm35
    // converting that reading to voltage, for 3.3v arduino use 3.3
    float voltage = reading * 5.0;
    voltage /= 1024.0;
    // now print out the temperature
    float temperature = voltage * 100 ; //converting from 10 mv per degree

 //build the message
  char temp_string[6]; //2 int, 2 dec, 1 point, and \0
  ftoa(temp_string,temperature);

    sprintf(message, "%d:%s\0", MYID, temp_string);  //millis provides a stamp for deduping if signal is repeated
    Serial.println(message);
    xmitMessage(message);  //message will not be sent if there is an error

void get_test_message(uint8_t *buf, uint8_t *buf_len)
{
  *(buf++) = 23; // node ID
  *(buf++) = 1;  // temperature measurement code
  *(float*)buf = 42.0f;  // dummy value
  *buf_len = 1+1+4;
}

void send_encoded_message()
{
  uint8_t message[50];
  
  void *buffer_ptr = message;
  uint8_t message_size = 0;
  *(uint8_t*)buffer_ptr = MYID;
  buffer_ptr += 1;
  message_size++;
  message_size += append_data(&buffer_ptr, (uint8_t)TEMPERATURE_CODE, 42.0f);

  xmitMessage((char*)message, message_size);
}

template <class T>
int append_data(void** buffer, uint8_t meaning_code, T data)
{
  *((uint8_t*)(*buffer)) = meaning_code;
  (*buffer) += 1;
  *(T*)(*buffer) = data;
  (*buffer) += sizeof(T);
  return sizeof(T) + 1;
}

void send_test_message()
{
  uint8_t message[50];
  
  void *buffer_ptr = message;
  uint8_t message_size = 0;
  get_test_message((uint8_t*)buffer_ptr, &message_size);

  xmitMessage((char*)message, message_size);
}

void loop() {
  if (wakeupIndex == 0)
  {
    measureAndSend();
    wakeupIndex = wakeupCount;
//    unsigned long randNumber = random(60,120); //1 to 2 minutes to delay
  }
  wakeupIndex--;

 
  byte old_ADCSRA = ADCSRA;                        // disable ADC //
  ADCSRA = 0;                                      // disable ADC //

  byte old_PRR = PRR;                              // disable Internal modules//
  PRR = 0xFF;                                      // disable Internal modules//

  MCUSR = 0;                                       // clear various "reset" flags// 

  // Watchdog Timer Parameters//
  WDTCSR = bit (WDCE) | bit (WDE);                 // allow changes, disable reset
  WDTCSR = bit (WDIE) | bit (WDP3) | bit (WDP0);   // set WDIE, and 8 seconds delay
  wdt_reset();                                     // pat the dog once program has executed.

  // Sleep Activation //
  set_sleep_mode (SLEEP_MODE_PWR_DOWN);            //Sleep mode Selection//
  sleep_enable();                                  //Sleep Now//

  // turn off brown-out enable in software//
  MCUCR = bit (BODS) | bit (BODSE);                //Brown out settings
  MCUCR = bit (BODS);                              //Brown out set.
  sleep_cpu ();                                    //CPU is now sleeping

  //--------------------------------------End of sleep Preperation-------------------------------//
  // Once awake code executes from this point//
  // Once CPU wakes up do the follwoing to restore full operations//
  sleep_disable();
  PRR = old_PRR;
  ADCSRA = old_ADCSRA;
}

