/*
* setup() - this function runs once when you turn your Arduino on
* We initialize the serial connection with the computer
*/
void setup()
{
  Serial.begin(115200); //Start the serial connection with the computer
}

void loop() // run over and over again
{
  int node = random(0, 10);
  float temperature = 20.0 + node + (random(0, 10) * 0.1);
  Serial.print(node);
  Serial.print(":");
  Serial.print(temperature);
  Serial.println();
  delay(1000); //waiting a second
}

