

/*
  Morse.cpp - Library for flashing Morse code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/


#include "BM019.h"


BM019::BM019(int IRQPin, int SSPin)
{
   _IRQPin = IRQPin;
   _SSPin = SSPin;
}


void BM019::Begin(){

    Serial.println("begin started"); 
        Serial.println(_IRQPin); 
        Serial.println(_SSPin); 
    NFCReady = 0;
    pinMode(_IRQPin, OUTPUT);
    digitalWrite(_IRQPin, HIGH); // Wake up pulse
    pinMode(_SSPin, OUTPUT);
    digitalWrite(_SSPin, HIGH);

    SPI.begin();
    SPI.setDataMode(SPI_MODE0);
    SPI.setBitOrder(MSBFIRST);
    SPI.setClockDivider(SPI_CLOCK_DIV32);
 
 // The CR95HF requires a wakeup pulse on its IRQ_IN pin
 // before it will select UART or SPI mode.  The IRQ_IN pin
 // is also the UART RX pin for DIN on the BM019 board.
 
    delay(10);                      // send a wake up
    digitalWrite(_IRQPin, LOW);      // pulse to put the 
    delayMicroseconds(100);         // BM019 into SPI
    digitalWrite(_IRQPin, HIGH);     // mode 
    delay(10);
    Serial.println("begin ended"); 
 
}


void BM019::Initialize()
{
  //Serial.println("initialize started"); 
  byte i = 0;

// step 1 send the command
  digitalWrite(_SSPin, LOW);
  SPI.transfer(0);  // SPI control byte to send command to CR95HF
  SPI.transfer(1);  // IDN command
  SPI.transfer(0);  // length of data that follows is 0
  digitalWrite(_SSPin, HIGH);
  delay(1);
 
// step 2, poll for data ready
// data is ready when a read byte
// has bit 3 set (ex:  B'0000 1000')

  digitalWrite(_SSPin, LOW);

  //Serial.println("initialize 1"); 
  while(RXBuffer[0] != 8)
    {
    RXBuffer[0] = SPI.transfer(0x03);  // Write 3 until
    RXBuffer[0] = RXBuffer[0] & 0x08;  // bit 3 is set
    }
  digitalWrite(_SSPin, HIGH);
  delay(1);

        //Serial.println("initialize 2"); 
// step 3, read the data
  digitalWrite(_SSPin, LOW);
  SPI.transfer(0x02);   // SPI control byte for read         
  RXBuffer[0] = SPI.transfer(0);  // response code
  RXBuffer[1] = SPI.transfer(0);  // length of data
  for (i=0;i<RXBuffer[1];i++)      
      RXBuffer[i+2]=SPI.transfer(0);  // data
  digitalWrite(_SSPin, HIGH);
  delay(1);

          //Serial.println("initialize 3"); 

  if ((RXBuffer[0] == 0) & (RXBuffer[1] == 15))
  {  
    Serial.println("IDN COMMAND-");  //
    Serial.print("RESPONSE CODE: ");
    Serial.print(RXBuffer[0]);
    Serial.print(" LENGTH: ");
    Serial.println(RXBuffer[1]);
    Serial.print("DEVICE ID: ");
    for(i=2;i<(RXBuffer[1]);i++)
    {
      Serial.print(RXBuffer[i],HEX);
      Serial.print(" ");
    }
    Serial.println(" ");
    Serial.print("ROM CRC: ");
    Serial.print(RXBuffer[RXBuffer[1]],HEX);
    Serial.print(RXBuffer[RXBuffer[1]+1],HEX);
    Serial.println(" ");
  }
  else
    Serial.println("BAD RESPONSE TO IDN COMMAND!");


        Serial.println("initialize 4"); 

  Serial.println(" ");
}

/* SetProtocol_Command programs the CR95HF for
ISO/IEC 15693 operation.

This requires three steps.
1. send command
2. poll to see if CR95HF has data
3. read the response

If the correct response is received the serial monitor is used
to display successful programming. 
*/
void BM019::SetProtocol()
 {
 byte i = 0;
 
// step 1 send the command
  digitalWrite(_SSPin, LOW);
  SPI.transfer(0x00);  // SPI control byte to send command to CR95HF
  SPI.transfer(0x02);  // Set protocol command
  SPI.transfer(0x02);  // length of data to follow
  SPI.transfer(0x01);  // code for ISO/IEC 15693
  SPI.transfer(0x0D);  // Wait for SOF, 10% modulation, append CRC
  digitalWrite(_SSPin, HIGH);
  delay(1);
 
// step 2, poll for data ready

  digitalWrite(_SSPin, LOW);
  while(RXBuffer[0] != 8)
    {
    RXBuffer[0] = SPI.transfer(0x03);  // Write 3 until
    RXBuffer[0] = RXBuffer[0] & 0x08;  // bit 3 is set
    }
  digitalWrite(_SSPin, HIGH);
  delay(1);

// step 3, read the data
  digitalWrite(_SSPin, LOW);
  SPI.transfer(0x02);   // SPI control byte for read         
  RXBuffer[0] = SPI.transfer(0);  // response code
  RXBuffer[1] = SPI.transfer(0);  // length of data
  digitalWrite(_SSPin, HIGH);

  if ((RXBuffer[0] == 0) & (RXBuffer[1] == 0))
  {
     Serial.println("PROTOCOL SET-");  //
     NFCReady = 1; // NFC is ready
  }
  else
  {
     Serial.println("BAD RESPONSE TO SET PROTOCOL");
     NFCReady = 0; // NFC not ready
  }
  Serial.println(" ");
}

/* Inventory_Command chekcs to see if an RF
tag is in range of the BM019.

This requires three steps.
1. send command
2. poll to see if CR95HF has data
3. read the response

If the correct response is received the serial monitor is used
to display the the RF tag's universal ID.  
*/
void BM019::CheckForTag()
 {
 byte i = 0;

// step 1 send the command
  digitalWrite(_SSPin, LOW);
  SPI.transfer(0x00);  // SPI control byte to send command to CR95HF
  SPI.transfer(0x04);  // Send Receive CR95HF command
  SPI.transfer(0x03);  // length of data that follows is 0
  SPI.transfer(0x26);  // request Flags byte
  SPI.transfer(0x01);  // Inventory Command for ISO/IEC 15693
  SPI.transfer(0x00);  // mask length for inventory command
  digitalWrite(_SSPin, HIGH);
  delay(1);
 
// step 2, poll for data ready
// data is ready when a read byte
// has bit 3 set (ex:  B'0000 1000')

  digitalWrite(_SSPin, LOW);
  while(RXBuffer[0] != 8)
  {
    RXBuffer[0] = SPI.transfer(0x03);  // Write 3 until
    RXBuffer[0] = RXBuffer[0] & 0x08;  // bit 3 is set
  }
  digitalWrite(_SSPin, HIGH);
  delay(1);


// step 3, read the data
  digitalWrite(_SSPin, LOW);
  SPI.transfer(0x02);   // SPI control byte for read         
  RXBuffer[0] = SPI.transfer(0);  // response code
  RXBuffer[1] = SPI.transfer(0);  // length of data
  for (i=0;i<RXBuffer[1];i++)      
      RXBuffer[i+2]=SPI.transfer(0);  // data
  digitalWrite(_SSPin, HIGH);
  delay(1);

  if (RXBuffer[0] == 128)
  {  
    Serial.println("TAG DETECTED");
    Serial.print("UID: ");
    for(i=11;i>=4;i--)
    {
      Serial.print(RXBuffer[i],HEX);
      Serial.print(" ");
    }
    Serial.println(" ");
  }
  else
    {
    Serial.print("NO TAG IN RANGE - ");
    Serial.print("RESPONSE CODE: ");
    Serial.println(RXBuffer[0],HEX);
    }
  Serial.println(" ");
}
