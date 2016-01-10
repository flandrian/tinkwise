#include <s10e5.h>

void setup() {
  char message[100];

  // put your setup code here, to run once:
  Serial.begin(115200);

  s10e5 temp_h(25.1);
  float temp_f = (float)temp_h;
  Serial.print("25.1f becomes ");
  Serial.println(temp_f);

  s10e5 pi_h(3.1416f);
  float pi_f = (float)pi_h;
  Serial.print("3.1416f becomes ");
  Serial.println(pi_f);

  s10e5 third_h(1.0f/3.0f);
  float third_f = (float)third_h;
  Serial.print("1.0f/3.0f becomes ");
  Serial.println(third_f);
}

void loop() {
  // put your main code here, to run repeatedly:
}
