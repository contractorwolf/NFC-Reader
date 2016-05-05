#include <SPI.h>
#include "BM019.h"

/*
NFC Communication with the Solutions Cubed, LLC BM019 
and an Arduino Uno.  The BM019 is a module that
carries ST Micro's CR95HF, a serial to NFC converter.

Wiring:
 Arduino          BM019
 IRQ: Pin 9       DIN: pin 2
 SS: pin 10       SS: pin 3
 MOSI: pin 11     MOSI: pin 5 
 MISO: pin 12     MISO: pin4
 SCK: pin 13      SCK: pin 6
 
 */

// the sensor communicates using SPI, so include the library:


BM019 reader(9, 10,5,6,4, false);


void setup() {
  Serial.begin(9600);
  Serial.println("RFID Reader started");


  
  reader.Begin();
}



void loop() {
  
  if(reader.NFCReady == 0)
  {
    reader.Initialize();  // reads the CR95HF ID
    delay(1000);
    reader.SetProtocol(); // ISO 15693 settings
    delay(1000);
  }
  else
  {
    reader.CheckForTag();
    delay(100);    
  }  

}

