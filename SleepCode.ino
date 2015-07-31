//Libraries//
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <VirtualWire.h>
//-----------------------------------End of Libraries---------------------------------------------//

// Global variables //
const byte LED             = 13;
       int ATimer;
       int BattValue       = 0;
       int BattCheckPin    = A3;
       int DataRadioSwitch = 9;
       int RadioTXPin      = 11;
/////////////////////////////////////  8 = 1 min
///////////////////////////////////// 45 = 6 min
///////////////////////////////////// 75 = 8 min 
int SleepTime = 450;///////////////// 450 = 1 Hour
///////////////////////////////////// 5400 = 12 Hours  
///////////////////////////////////// 10800 = 24 Hours                                      
                                     
//-----------------------------------End of Global Variables--------------------------------------//

// Data Structures//
typedef struct roverRemoteData 
{
  int    TX_ID;      // Just to make sure that the transmission is ID'd 
  int    Sensor1Data;// The variable were the data from each sensor
};
//-----------------------------------End of Data Structures---------------------------------------//

// watchdog interrupt//
ISR (WDT_vect) 
{
  wdt_disable();                   // disable watchdog//
}  

//------------------------------------End of Watchdog---------------------------------------------//
//Setup//
void setup () 
{
  vw_setup(2000);                  // Radio Baud Rate//
  vw_set_tx_pin(RadioTXPin );      // Set DATA radio TX Pin// NOTE: MUST BE PWM PIN
  pinMode(DataRadioSwitch,OUTPUT);
}
//------------------------------------End of Setup-------------------------------------------------//
 //Main Loop//
 void loop () 
 {   
  flash ();                        //HeartBeat Flash//
  AwakeTimer();                    //Awake Timer// 
    if ( ATimer == SleepTime)
    { 
      BattRead();
      DataTX();
    } // End of Main loop if timer was reached

//----------------------------------- End of Actual executing code---------------------------//
  //Preparing to go to sleep//
  
  digitalWrite(DataRadioSwitch,LOW);               //Switch Data Radio off

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

//----------------------------------------END OF MAIN PROGRAM---------------------------------------//

// VOIDS FUNCTIONS//

//Wake up timer//
void AwakeTimer()                  
{
  ATimer++;
}

// Heartbeat flash//
void flash () 
{
  pinMode (LED, OUTPUT);
  digitalWrite (LED, HIGH);
  delay (16);
  digitalWrite (LED, LOW);
  delay (16);
  ATimer = 0;                      // Reset Wake up timer//
}

// Read Own battery State//
void BattRead()
{
  BattValue = 0;
  BattValue= analogRead(BattCheckPin);
}

// Transmit data//
void DataTX()
{
  digitalWrite(DataRadioSwitch,HIGH);
  struct roverRemoteData payload;

  payload.TX_ID = 3;
  payload.Sensor1Data = BattValue;

  vw_send((uint8_t *)&payload, sizeof(payload));
  vw_wait_tx();
  digitalWrite(DataRadioSwitch,LOW);
}





