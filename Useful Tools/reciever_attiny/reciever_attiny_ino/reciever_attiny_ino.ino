#include <MANCHESTER.h>

#define RxPin 0

void setup() 
{ 
 MANCHESTER.SetRxPin(RxPin); //user sets rx pin default 4
 MANCHESTER.SetTimeOut(1000); //user sets timeout default blocks
 pinMode(1,OUTPUT);
}//end of setup

void loop() 
{
 unsigned int data = MANCHESTER.Receive();
 digitalWrite(1,data);
}//end of loop


