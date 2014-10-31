#include <avr/io.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <Servo.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <SPI.h>
#include <MirfHardwareSpiDriver.h>

#define RED 5
#define GREEN 6
#define BLUE 7
#define BUTTON 8
#define SERVO 9

#define lock 0
#define unlock 135

int buttonStateGo = HIGH;
int lastButtonStateGo = HIGH;
long lastDebounceTimeGo = 0;
long debounceDelayGo = 50;

boolean locked = true;
boolean changed = false;

Servo servo;

void setup() {     
  power_adc_disable();
  power_twi_disable();
  
  MCUCR = _BV (BODS) | _BV (BODSE);
  MCUCR = _BV (BODS);
  
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(SERVO, OUTPUT);
  servo.attach(SERVO);
  servo.write(lock);
  
  Mirf.cePin = 4;
  Mirf.csnPin = 3;
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setRADDR((byte *)"clie1");
  Mirf.payload = sizeof(unsigned long);
  Mirf.channel = 7;
  Mirf.config();
}

void loop() {
  Mirf.powerDown();
  if(!digitalRead(BUTTON)) {
      digitalWrite(RED, HIGH);
      delay(100);
      digitalWrite(RED, LOW);
      locked = !locked;
      changed = true;
  }
  
  if(changed) {
     if(locked) {servo.write(lock); delay(500); servo.write(lock+15);}
     else {servo.write(unlock);delay(500); servo.write(unlock-15);}
     changed = false;
  }
  enterSleep();
}

boolean buttonPressed(){
  int reading = digitalRead(BUTTON);
  boolean pressed = false;
  if (reading != lastButtonStateGo) {
    lastDebounceTimeGo = millis();
  } 
  
  if ((millis() - lastDebounceTimeGo) > debounceDelayGo) {
    if (reading != buttonStateGo) {
      buttonStateGo = reading;
      if (buttonStateGo == HIGH) {
        pressed = true;
      }
    }
  }
  lastButtonStateGo = reading;
  return pressed;
}

void enterSleep() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);             // select the watchdog timer mode
MCUSR &= ~(1 << WDRF);                           // reset status flag
WDTCSR |= (1 << WDCE) | (1 << WDE);              // enable configuration changes
WDTCSR =  (1 << WDP2)|(1 << WDP1); // set the prescalar = 7
WDTCSR |= (1 << WDIE);  
  sleep_mode();
  sleep_disable();
}

ISR(WDT_vect){}
