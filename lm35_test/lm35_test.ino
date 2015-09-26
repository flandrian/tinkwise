    //TMP36 Pin Variables
    int sensorPin = 0; //the analog pin the TMP36's Vout (sense) pin is connected to
    int sensorSupplyPin = 2; //the analog pin the TMP36's Vout (sense) pin is connected to
    //the resolution is 10 mV / degree centigrade with a
    //500 mV offset to allow for negative temperatures
    /*
    * setup() - this function runs once when you turn your Arduino on
    * We initialize the serial connection with the computer
    */
    void setup()
    {
    Serial.begin(9600); //Start the serial connection with the computer
    //to view the result open the serial monitor
      pinMode(sensorSupplyPin, OUTPUT);      // sets the digital pin as output
    }
    void loop() // run over and over again
    {
    digitalWrite(sensorSupplyPin, HIGH);    // enable supply for lm35
    delay(1); //let sensor input settle
    int reading = analogRead(sensorPin);    //getting the voltage reading from the temperature sensor
    digitalWrite(sensorSupplyPin, LOW);    // enable supply for lm35
    // converting that reading to voltage, for 3.3v arduino use 3.3
    float voltage = reading * 5.0;
    voltage /= 1024.0;
    // print out the voltage
    Serial.print(voltage); Serial.println(" volts");
    // now print out the temperature
    float temperatureC = voltage * 100 ; //converting from 10 mv per degree wit 500 mV offset
    //to degrees ((voltage - 500mV) times 100)
    Serial.print(temperatureC); Serial.println(" degrees C");
    delay(1000); //waiting a second
    }
