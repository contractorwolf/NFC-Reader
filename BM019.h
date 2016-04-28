/*
  Morse.h - Library for flashing Morse code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/

#ifndef Morse_h
#define Morse_h

#include "Arduino.h"
#include <SPI.h>

class BM019
{
  public:
    BM019(int IRQPin, int SSPin);
    void Begin();
    void Initialize();
    void SetProtocol();
    void CheckForTag();
    byte NFCReady;  // used to track NFC state
    
  private:
    int _SSPin;  // Slave Select pin
    int _IRQPin;  // Sends wake-up pulse
    byte TXBuffer[40];    // transmit buffer
    byte RXBuffer[40];    // receive buffer

};


#endif





