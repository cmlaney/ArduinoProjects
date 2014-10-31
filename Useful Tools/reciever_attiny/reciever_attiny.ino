#include <MANCHESTER.h>

#define RxPin 0
#define ledPin 1

void setup() 
{ 
 pinMode(ledPin, OUTPUT);
 MANCHESTER.SetRxPin(RxPin); //user sets rx pin default 4
 MANCHESTER.SetTimeOut(1000); //user sets timeout default blocks
 Serial.begin(9600);
}

void loop() 
{
 unsigned int data = MANCHESTER.Receive();
 Serial.print("Data: ");
 Serial.println(data);
 if (data == 1)
 {
   digitalWrite(1, HIGH);
 }
 else
 {
  digitalWrite(1, LOW); 
 }
}

