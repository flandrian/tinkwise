// receiver.pde
//
// Simple example of how to use VirtualWire to receive messages
// Implements a simplex (one-way) receiver with an Rx-B1 module
//
// See VirtualWire.h for detailed API docs
// Author: Mike McCauley (mikem@airspayce.com)
// Copyright (C) 2008 Mike McCauley
// $Id: receiver.pde,v 1.3 2009/03/30 00:07:24 mikem Exp $

#include "VirtualWire.h"
#define RXPIN 2

#define FLOAT_BUFFER_SIZE 10
char float_buffer[FLOAT_BUFFER_SIZE];

#define TEMPERATURE_CODE 1
#define SUPPLY_VOLTAGE_CODE 2

void setup()
{
  Serial.begin(115200);	//Transmit readings over this port
  // Initialise the IO and ISR
  vw_set_rx_pin(RXPIN);
  vw_set_ptt_inverted(true); // Required for DR3100
  vw_setup(2000);	 // Bits per sec
  vw_rx_start();       // Start the receiver PLL running
}

int ftoa(char *a, float f)  //translates floating point readings into strings to send over the air
{
  int left = int(f);
  float decimal = f - left;
  int right = decimal * 100; //2 decimal points
  if (right > 10) {  //if the decimal has two places already. Otherwise
    sprintf(a, "%d.%d", left, right);
  } else {
    sprintf(a, "%d.0%d", left, right); //pad with a leading 0
  }
}

void loop()
{
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;

  digitalWrite(13, true); // Flash a light to show received good message

  if (vw_get_message(buf, &buflen)) // Non-blocking
  {
    int buf_index = 0;
    Serial.print("{\"node\":");
    uint8_t node_index = *(uint8_t*)(buf + buf_index);
    buf_index += 1;
    Serial.print(node_index);
    Serial.print(",");
    int data_index = 0;
    while (buf_index < buflen)
    {
      uint8_t data_type = *(uint8_t*)(buf + buf_index);
      buf_index++;
      float data = *(float*)(buf + buf_index);
      buf_index += 4;
      if (data_index > 0) Serial.print(",");
      switch (data_type)
      {
        case TEMPERATURE_CODE:
          Serial.print("\"temperature\":");
          break;
        case SUPPLY_VOLTAGE_CODE:
          Serial.print("\"supply voltage\":");
          break;
      }
      ftoa(float_buffer, data);
      Serial.print(float_buffer);
      data_index++;
    }
    Serial.println("}");
    Serial.println();
    digitalWrite(13, false);
  }
}
