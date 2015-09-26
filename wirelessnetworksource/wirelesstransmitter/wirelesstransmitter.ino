#include <avr/sleep.h>
#include <avr/wdt.h>
#include "VirtualWire.h"
#include "DHT.h"

#define MYID 1      //the ID number of this board.  Change this for each board you flash.
                    //The ID will be transmitted with the data so you can tell which device is transmitting
#define TRANSPIN 3  //what pin to transmit on
#define DHTPIN 4     // what pin the DHT is connected to

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11 
//#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

       int ATimer;
       
/////////////////////////////////////  8 = 1 min
///////////////////////////////////// 45 = 6 min
///////////////////////////////////// 75 = 8 min 
int SleepTime = 450;///////////////// 450 = 1 Hour
///////////////////////////////////// 5400 = 12 Hours  
///////////////////////////////////// 10800 = 24 Hours                                      

// Connect pin 1 (on the left) of the sensor to +5V
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

DHT dht(DHTPIN, DHTTYPE);

// watchdog interrupt//
ISR (WDT_vect) 
{
  wdt_disable();                   // disable watchdog//
}  

void setup() {
  Serial.begin(9600); 
  randomSeed(analogRead(A0));  //initialize the random number generator with
                               //a random read from an unused and floating analog port
  //initialize the virtual wire library
  vw_set_ptt_inverted(true); // Required for DR3100
  vw_set_tx_pin(TRANSPIN);
  vw_setup(2000);  //keep the data rate low for better reliability and range
  
  //Initialize the Sensor
  dht.begin();
  
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

int xmitMessage(char *msg){
    digitalWrite(13, true); // Flash an led to show transmitting
    vw_send((uint8_t *)msg, strlen(msg));
    vw_wait_tx(); // Wait until the whole message is gone
    digitalWrite(13, false);
}


void loop() {
  char message[50];

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float t = dht.readTemperature();

 //build the message
  char temp[6]; //2 int, 2 dec, 1 point, and \0
  ftoa(temp,t);

  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (isnan(t)) {
    sprintf(message, "ID:%d:TS:%lu:ER:ERROR\0", MYID, millis());  //millis provides a stamp for deduping if signal is repeated
    Serial.println("Failed to read from DHT");
    xmitMessage(message);
  } else {
    Serial.print("Temperature: "); 
    Serial.print(t);
    Serial.println(" *C");
    Serial.print("Sending Message: ");
    sprintf(message, "ID:%d:TS:%lu:TC:%s\0", MYID, millis(), temp);  //millis provides a stamp for deduping if signal is repeated
    Serial.println(message);
    xmitMessage(message);  //message will not be sent if there is an error
  }
  unsigned long randNumber = random(60,120); //1 to 2 minutes to delay
 
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

