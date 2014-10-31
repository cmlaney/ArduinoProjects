#include <MANCHESTER.h>

#define TxPin 0  //the digital pin to use to transmit data

unsigned int Tdata = 0;  //the 16 bits to send

void setup() 
{                
MANCHESTER.SetTxPin(TxPin);      // sets the digital pin as output default 4
pinMode(1,OUTPUT);
}//end of setup

void loop() 
{
 Tdata = 0;
 MANCHESTER.Transmit(Tdata);
 digitalWrite(1,Tdata);
 delay(1000);
 Tdata = 1;
 MANCHESTER.Transmit(Tdata);
 digitalWrite(1,Tdata);
 delay(1000);
}//end of loop


