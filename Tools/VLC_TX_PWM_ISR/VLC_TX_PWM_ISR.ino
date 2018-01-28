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
 *
 * Place the Timer1 files in Arduino/Libraries/
 */

/** Libraries **/
#include "TimerOne.h"

/** Defines **/
#define PIN_TX 10
#define FASTCLEAR(x,y) (x&=(~(1<<y)))
#define FASTSET(x,y) (x|=(1<<y))

/** Variables **/
unsigned char incomingByte = 0;
unsigned char symbols[25] = {0};
volatile unsigned char txFlag = 0;

/** Initialisation **/
void setup() {
  pinMode(PIN_TX, OUTPUT);    // Make pin PIN_TX an output.
  digitalWrite(PIN_TX, 0);    // Disable it.

  Serial.begin(9600);     // opens serial port, sets data rate to 9600 bps.
  
  Timer1.initialize(1);            // initialize timer1, and set a 1/2 second period
  Timer1.attachInterrupt(callback);   // attaches callback() as a timer overflow interrupt
}

/** Main loop **/
void loop() {
  if (Serial.available() > 0) {
    incomingByte = Serial.read();                   // Read the incoming byte.
    prepareTransmission(symbols, incomingByte);   // Create a symbol array based on the received char.
   
    startTransmission();                            // Start the transmission.

    while(txFlag);   // Wait for the transmission to finish. 
  }
}

/** Interrupt service routines **/
void callback() {
  static unsigned char finger = 0;

  if(symbols[finger++]) {   // Send the symbols one by one.
    FASTSET(PORTB, 6);     // Led on.
  } else {
    FASTCLEAR(PORTB, 6);   // Led off.
  }
  
  //digitalWrite(PIN_TX, symbols[finger++]);  //Arduino digitalWrite has a lot of safety checks which reduce speed.

  if (finger > 24) {      // When we are at the end, stop the transmission.
    FASTCLEAR(PORTB, 6); // Led off.
    finger = 0;
    txFlag = 0;           // Flag the main program to continue.
    Timer1.stop();        // Disable the timer interrupt.
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
  }
  symbolbuffer[finger++] = 1;   // Last symbol is always a 1.

  while (finger < 25) {
    symbolbuffer[finger++] = 0; // Fill the rest of the buffer with 0's.
  }
}

void startTransmission() {
  txFlag = 1;         // Flag to indicate transmission.
  Timer1.resume();    // Start Timer1.
}






