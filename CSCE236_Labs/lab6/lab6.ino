// Wire Slave Receiver
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Receives data as an I2C/TWI slave device
// Refer to the "Wire Master Writer" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>

#define red 13
#define button1 2

long buttonTimer = 0;
boolean printStuff = false;

void setup()
{
  Wire.begin(4);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);           // start serial for output
  pinMode(red, OUTPUT);
  pinMode(button1, INPUT_PULLUP);
  attachInterrupt(0, sendStuff, RISING);
}

void loop()
{
  if(printStuff) {
   printStuff = !printStuff;
   Wire.beginTransmission(1);
    Wire.write('p');
    Wire.endTransmission();
  }
delay(100);
}

void sendStuff() {
  Serial.println("pressed");
  printStuff = true;
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  while(0 < Wire.available()) // loop through all but the last
  {
    char c = Wire.read(); // receive byte as a character
    Serial.println(c);         // print the character
    if(c==112) {
       digitalWrite(red, !digitalRead(red));
    }
  }
}
