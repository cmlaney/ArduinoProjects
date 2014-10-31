#include <Servo8Bit.h>
#include <Manchester.h>
#include <avr/io.h>
#define servoPin 0
#define BUTTON 2
#define RX_PIN 1

Servo8Bit servo;
boolean state = false;
boolean valid = false;
int on = 170;
int mid = 130;
int off = 115;
int servoDelay = 25;

int buttonStateGo = HIGH;
int lastButtonStateGo = HIGH;
long lastDebounceTimeGo = 0;
long debounceDelayGo = 50;

uint8_t data = 0;
uint8_t id = 0;

void setup() {
  pinMode(BUTTON, INPUT);
  digitalWrite(BUTTON, HIGH);
  pinMode(servoPin, OUTPUT);
  man.setupReceive(RX_PIN, MAN_1200);
  man.beginReceive();

  servo.attach(servoPin);
  servo.write(off);
  delay(servoDelay);
  servo.write(mid);
  delay(servoDelay);
}

// the loop routine runs over and over again forever:
void loop() {
  if(buttonPressed()||valid) {
    if(state) {
      servo.write(off);
       delay(servoDelay);
      delay(250);
      servo.write(mid);
       delay(servoDelay);
    } else {
      servo.write(on);
       delay(servoDelay);
    }
    state = !state;
    valid = false;
  }
  
  if (man.receiveComplete()) {
    uint16_t m = man.getMessage();
    man.beginReceive();
    if(m==2141) {
     valid = true; 
    }
  }
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
