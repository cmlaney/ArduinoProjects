#include <avr/sleep.h>
#include <avr/wdt.h>

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= -_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

int led = 1;                                       //Buzzer connected to Digital Pin 0 
int Pir = 0;                                          //Passive Infra-Red Sensor Connected to Digital Pin 1 
int Photoval = 0;                                     //Interger to hold status of the Photo Cell light value
int Pirval = 0;                                       //Integer to hold status (value) of the PIR
void setup() {
   pinMode(led,OUTPUT);                            // Enable internal pullup resistors for buzzer output.
 }
 
void system_sleep()
{
  sbi(MCUCR,PUD);                                  //Disables All Internal Pullup Resistors
  sbi(GIMSK,PCIE);                                   //Enable Pin Change Interrupts Interrups
  sbi(PCMSK,PCINT0);                              //Changes Interrupt to PIN1 (PCINT1) 
  cbi(ADCSRA,ADEN);                              //switch Analog to Digital Converter OFF
  cbi(MCUCR,SM0);                                  //Power Down Mode
  sbi(MCUCR,SM1);                                  //Power Down Mode
  sbi(MCUCR,SE);                                    //sleep Mode Power down enable (Sleep_enable(); should set this-- not tested yet)
  sleep_enable();                                      //Sets the Sleep Enable bit in the MCUCR Register (SE BIT)
  sleep_mode();                                       //sleep begins here
  sleep_disable();                                     //Coming out of sleep
  sbi(ADCSRA,ADEN);                             //switch Analog to Digital Converter ON
  cbi(MCUCR,PUD);                                 //Enables Pullup Resistors Again (this should happen anyway, but i have it here for now just in case, haven't fully tested yet)
 }

void loop()
{
  digitalWrite(led,LOW);                    //Read the status of the photocell, if it is dark outside, Enable Buzzer output if Motion Deteced by PIR Sensor
Pirval = digitalRead(Pir);                                   //read the status of the PIR, sets the value to HIGH or LOW
if ((Pirval ==HIGH))                 //Check status of Photocell, If it is light, Turn outputs off)
{
  digitalWrite(led,HIGH);
  delay(1000); 
}
system_sleep();                                      //Runs the system_sleep() function
}
