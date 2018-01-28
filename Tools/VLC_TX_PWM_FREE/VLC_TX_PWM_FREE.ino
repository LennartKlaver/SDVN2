/**
 * Author: Lennart Klaver
 * Date:   28-01-2018
 * License: Creative Commons CC BY-NC-SA 4.0 see https://creativecommons.org/licenses/by-nc-sa/4.0/
 * Description: 
 * This program sends each byte it receives on its Serial port using visible light communication.
 * It uses the pulse position modulation scheme, which is not optimal in terms of speed but has timing encoded in it which makes it more robust and avoids flickering.
 *
 * The transmitter circuit looks like this:
 * +5V ---------------
 *       |           |
 *     R 33K        LED >>
 *       |           |
 * PIN6 ----------10N20C
 *       |           |
 *      R 68K      R 680
 *       |           |
 * GND ---------------
 *
 * We used a Sparkfun Arduino Pro Micro.
 */

/** Libraries **/
#include "TimerOne.h"	// Library created by Jesse Tane for http://labs.ideo.com August 2008.

/** Defines **/
#define PIN_TX 10
#define FASTCLEAR(x,y) (x&=(~(1<<y)))
#define FASTSET(x,y) (x|=(1<<y))

/** Variables **/
unsigned char incomingByte = 0;
unsigned char symbols[25] = {0};

/** Initialisation **/
void setup() {
  pinMode(PIN_TX, OUTPUT);    // Make pin PIN_TX an output.
  digitalWrite(PIN_TX, 0);    // Disable it.

  Serial.begin(9600);     // opens serial port, sets data rate to 9600 bps.
  
}

/** Main loop **/
void loop() {
  if (Serial.available() > 0) {
    incomingByte = Serial.read();                   // Read the incoming byte.
    prepareTransmission(symbols, incomingByte);   // Create a symbol array based on the received char.
    transmit(symbols);      // Send the message.
  }
}

void prepareTransmission(unsigned char * symbolbuffer, unsigned char msg) {
  unsigned char finger = 0;
  
  for(int i = 7; i>=0; i--) {       // Walk over all the bits in the message.
    if ((msg >> i) & 0x01) {        // Next symbol represents a 1.
      symbolbuffer[finger++] = 1;
      symbolbuffer[finger++] = 0;
    } else {                       // Next symbol represents a 0.
      symbolbuffer[finger++] = 1;
      symbolbuffer[finger++] = 0;
      symbolbuffer[finger++] = 0;
    }
    //delayMicroseconds(1);
  }
  symbolbuffer[finger++] = 1;   // Last symbol is always a 1.

  while (finger < 25) {
    symbolbuffer[finger++] = 0; // Fill the rest of the buffer with 0's.
  }
}

void transmit(unsigned char * symbolbuffer) {
  for(unsigned char finger = 0; finger < 25; finger++) {
    if(symbolbuffer[finger]) {
      FASTSET(PORTB, 6);
    } else {
      FASTCLEAR(PORTB, 6);
    }
    delayMicroseconds(10);  // Some delay to allow more light, this results in ~35-50kbps.
    //digitalWrite(PIN_TX, symbolbuffer[finger]);  // Send the symbols one by one.
  }
}



