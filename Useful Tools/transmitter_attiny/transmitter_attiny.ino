#include <MANCHESTER.h>

#define TxPin 0  //the digital pin to use to transmit data

unsigned int ON = 1010;  //the 16 bits to send
unsigned int OFF = 0000;  //the 16 bits to send

void setup() 
{  
  MANCHESTER.SetTxPin(TxPin);      // sets the digital pin as output default 4

}

void loop() 
{
       MANCHESTER.Transmit(ON);
      delay(1000);
   
      MANCHESTER.Transmit(OFF);
      delay(1000);

}

