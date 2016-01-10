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

#define FLOAT_BUFFER_SIZE 6
char float_buffer[FLOAT_BUFFER_SIZE];

void setup()
{
    Serial.begin(115200);	//Transmit readings over this port
    // Initialise the IO and ISR
    vw_set_rx_pin(RXPIN);
    vw_set_ptt_inverted(true); // Required for DR3100
    vw_setup(2000);	 // Bits per sec
    vw_rx_start();       // Start the receiver PLL running
}

void ftos(char *buf, float value)
{
    int16_t value_int = value;
    sprintf(buf, "%d", value_int);
}

void get_test_message(uint8_t *buf, uint8_t *buf_len)
{
  delay(1000);
  *(buf++) = 23; // node ID
  *(buf++) = 1;  // temperature measurement code
  *(float*)buf = 42.0f;  // dummy value
  *buf_len = 1+1+4;
}

void loop()
{
    uint8_t buf[VW_MAX_MESSAGE_LEN];
    uint8_t buflen = VW_MAX_MESSAGE_LEN;

    digitalWrite(13, true); // Flash a light to show received good message

    get_test_message(buf, &buflen);
//    if (vw_get_message(buf, &buflen)) // Non-blocking
    {
        int buf_index = 0;
//        Serial.print("{\"node\":");
        uint8_t node_index = *(uint8_t*)(buf+buf_index);
        buf_index += 1;
        Serial.print(node_index);
//        Serial.print(",");
        Serial.print(":");
        while(buf_index < buflen)
        {
            uint8_t data_type = *(uint8_t*)(buf+buf_index);
            buf_index++;
            float data = *(float*)(buf+buf_index);
            buf_index += 4;
//            Serial.print("\"temperature\":");
            ftos(float_buffer, data);
            Serial.print(float_buffer);
//            Serial.print(",");
        }
//            Serial.println("}");
            Serial.println();
            digitalWrite(13, false);
    }
}
