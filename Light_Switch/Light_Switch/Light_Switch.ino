#include <Servo.h>
#include <Manchester.h>
#include <avr/io.h>
#define servoPin 4
#define BUTTON 5
#define RX_PIN 12

Servo servo;
boolean state = false;
boolean valid = false;
int on = 175;
int off = 115;

int buttonStateGo = HIGH;
int lastButtonStateGo = HIGH;
long lastDebounceTimeGo = 0;
long debounceDelayGo = 50;

uint8_t data = 0;
uint8_t id = 0;

void setup() {
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(servoPin, OUTPUT);
  man.setupReceive(RX_PIN, MAN_1200);
  man.beginReceive();

  servo.attach(servoPin);
  servo.write(off);
  delay(500);
  servo.write(off+10);
}

// the loop routine runs over and over again forever:
void loop() {
  if(buttonPressed()||valid) {
    if(state) {
      servo.write(off);
      delay(500);
      servo.write(off+10);
    } else {
      servo.write(on);
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
