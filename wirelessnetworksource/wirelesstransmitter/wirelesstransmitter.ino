#include "VirtualWire.h"
#include "DHT.h"

#define MYID 1      //the ID number of this board.  Change this for each board you flash.
                    //The ID will be transmitted with the data so you can tell which device is transmitting
#define TRANSPIN 3  //what pin to transmit on
#define DHTPIN 4     // what pin the DHT is connected to
#define UNIT 1      // 0 for Fahrenheit and 1 for Celsius

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11 
//#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

DHT dht(DHTPIN, DHTTYPE);

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
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float tf = t * 1.8 +32;  //Convert from C to F 

 //build the message
  char temp[6]; //2 int, 2 dec, 1 point, and \0
  char hum[6];
  if (UNIT == 0 ){  //choose the right unit F or C
    ftoa(temp,tf);
  }
  else {
    ftoa(temp,t);
  }
  ftoa(hum,h);

  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (isnan(t) || isnan(h)) {
    sprintf(message, "ID:%d:TS:%lu:ER:ERROR\0", MYID, millis());  //millis provides a stamp for deduping if signal is repeated
    Serial.println("Failed to read from DHT");
    xmitMessage(message);
  } else {
    Serial.print("Humidity: "); 
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: "); 
    if (UNIT == 0 ) {
      Serial.print(tf);
      Serial.println(" *F");
    }else {
      Serial.print(t);
      Serial.println(" *C");
    }
    Serial.print("Sending Message: ");
    if (UNIT == 0 ){
      sprintf(message, "ID:%d:TS:%lu:TF:%s:RH:%s\0", MYID, millis(), temp, hum);  //millis provides a stamp for deduping if signal is repeated
    }
    else { //Celsius
      sprintf(message, "ID:%d:TS:%lu:TC:%s:RH:%s\0", MYID, millis(), temp, hum);  //millis provides a stamp for deduping if signal is repeated
    }
    Serial.println(message);
    xmitMessage(message);  //message will not be sent if there is an error
  }
  unsigned long randNumber = random(60,120); //1 to 2 minutes to delay
  unsigned long sleepTime=randNumber*1000;
  Serial.print("Sleeping ");
  Serial.print(sleepTime);
  Serial.println(" miliseconds");
  delay(sleepTime);  //Sleep randomly to avoid cross talk with another unit
}


