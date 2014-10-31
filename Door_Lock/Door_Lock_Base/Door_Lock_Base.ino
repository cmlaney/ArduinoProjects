#include <avr/io.h>
#include <SoftwareServo.h>

#define SERVO 0
#define RED 1
#define GREEN 2
#define BLUE 3
#define BUTTON 4
#define EXT_INPUT1 5
#define EXT_INPUT2 6
#define lock 110
#define unlock 10

int buttonStateGo = HIGH;
int lastButtonStateGo = HIGH;
long lastDebounceTimeGo = 0;
long debounceDelayGo = 50;

boolean locked = true;
boolean changed = true;
boolean prevLocked = true;

int angle = 90;
int position = 90;
int feedbackPin = 1;
int val = 0;
int calVal[] = {222, 1023};  // initial cal values
int calStartPos = 0;
int final = 90;

#define NUMREADINGS 10      // Number of readings to take for smoothing

int readings[NUMREADINGS];                // the readings from the analog input
int index = 0;                            // the index of the current reading
int total = 0;                            // the running total
int average = 0;                          // the average

SoftwareServo servo;

void setup() {
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  
  led_flash(RED);
  led_flash(GREEN);
  led_flash(BLUE);
  
  pinMode(BUTTON, INPUT);
  servo.attach(SERVO);
  servo.setMaximumPulse(2400);
  servo.write(lock);
  SoftwareServo::refresh();
  analogReference(EXTERNAL);
  calibrate();
}

void loop() {
 if(buttonPressed()) {
      digitalWrite(GREEN,LOW);
      digitalWrite(RED,LOW);
      digitalWrite(BLUE, HIGH);
      delay(100);
      digitalWrite(BLUE, LOW);
      if (prevLocked) {
        unlockDoor();
       prevLocked = false; 
      } else {
        lockDoor();
        prevLocked = true;
      }
      changed = true;
  }
  
  if(locked) {
    digitalWrite(GREEN,LOW);
    digitalWrite(RED,HIGH);
  } else {
    digitalWrite(RED,LOW);
    digitalWrite(GREEN,HIGH);
  }
  
  checkWiper();
}

void lockDoor() {
  servo.write(lock);
 int i = 0;
     for(i=0; i<10; i++) {
       SoftwareServo::refresh();
       delay(50);
     } 
}

void unlockDoor() {
  servo.write(unlock);
  int i = 0;
     for(i=0; i<10; i++) {
       SoftwareServo::refresh();
       delay(50);
     }
}

void checkWiper() {
  static int v = 0;
  position = analogRead(feedbackPin);  
  position = smooth(position);
  final = position;
  angle = map(final, calVal[0], calVal[1], 0, 180);
  
  if(angle < 90) {
    locked = false;
  } else {
    locked = true;
  }
  
}

int smooth(int data) {
    total -= readings[index];               // subtract the last reading
    readings[index] = analogRead(feedbackPin); // read from the sensor
    total += readings[index];               // add the reading to the total
    index = (index + 1);                    // advance to the next index

    if (index >= NUMREADINGS)               // if we're at the end of the array...
    index = 0;                            // ...wrap around to the beginning

    val = total / NUMREADINGS;          // calculate the average
    return val;
}

void calibrate() {
        servo.write(1);
        int c1 = 0;
        int c2 = 0;
        for(c1=0; c1<15; c1++) {
         SoftwareServo::refresh();
        delay(50); 
        }
        calVal[0] = analogRead(feedbackPin);  
        servo.write(180);
        for(c2=0; c2<15; c2++) {
         SoftwareServo::refresh();
        delay(50); 
        }
        calVal[1] = analogRead(feedbackPin);
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

void led_flash(int pin) {
  digitalWrite(pin, HIGH);
  delay(500);
  digitalWrite(pin, LOW);
  delay(500);
}

