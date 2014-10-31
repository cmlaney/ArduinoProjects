/*

Maze solving robot

This code uses an Ultrasonic sensor (HC-SR04) to detect walls to the right 
of the robot and a forward bump sensor to detect when the robot has encountered 
a wall. It uses the right hand rule to navigate a maze from start to finish and 
it uses an IR sensor to detect the end point of the maze.

Class: CSCE 236
Author: Christian Laney
Date of last revision: 5/4/14

*/

#include <avr/io.h>
#include <Servo.h>

#define IR A0
#define LSERVO 10
#define RSERVO 11
#define BUTTON 3
#define BUMP 2
#define RED 5
#define GREEN 6
#define BLUE 7
#define ECHO 8 //Echo pin for ultrasonic sensor
#define TRIG 9 //Trigger pin for ultrasonic sensor

Servo lservo;
Servo rservo;

int buttonStateGo = HIGH;
int lastButtonStateGo = HIGH;
long lastDebounceTimeGo = 0;
long debounceDelayGo = 50;

double nearBound;
double farBound;

boolean wall = false;
boolean beginMoving = false;
boolean first = true;

//##############################################################################################################################################
//Triggers
//##############################################################################################################################################

void movingSwitch() {
  digitalWrite(GREEN,LOW);
    beginMoving = !beginMoving;
    digitalWrite(RED, !digitalRead(RED));
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

void bumped() {
  lservo.write(93);
  rservo.write(90);
  wall = true;
}

//##############################################################################################################################################
//Directional Functions
//##############################################################################################################################################

void forward(double speed) {
	int rspeed = 90 - (90*speed);
	int lspeed = 90 + (90*speed)+10;
    rservo.write(rspeed);
    lservo.write(lspeed);
}

void leftTurn(double speed) {
    int servoTurnSpeed = 90-(90*speed);
   lservo.write(110);
   rservo.write(servoTurnSpeed);
}

void rightTurn(double speed) {
	int servoTurnSpeed = 90+(90*speed);
   lservo.write(servoTurnSpeed);
   rservo.write(76);
}

void halt() {
   lservo.write(93);
   rservo.write(90); 
}

void followWall() {
  
  double val = readUltra();
  if(val>13) {
    doABarrelRoll();
  } else  
   if(val>nearBound && val<farBound) {
     Serial.println("straight");
     forward(0.3);
  } else

  if(val>farBound) {
    Serial.println("too far");
    rightTurn(0.3);
  } else

  if(val<nearBound) {
    Serial.println("too close");
    leftTurn(0.3);
  }
}

void turnLeft() {
//  lservo.write(93);
//  rservo.write(90);
//  delay(1000);
  lservo.write(0);
  rservo.write(180);
  delay(100);
  rservo.write(0);
  lservo.write(0);
  delay(300);
//  lservo.write(93);
//  rservo.write(90);
//  delay(1000);
}

void doABarrelRoll() {
//  lservo.write(93);
//  rservo.write(90);
//  delay(1000);
  lservo.write(180);
  rservo.write(0);
  delay(200);
  rservo.write(90);
  delay(750);
  rservo.write(0);
  delay(400);
//  lservo.write(93);
//  rservo.write(90);
//  delay(1000);
}

void victoryDance() {
  long timer = millis();
  
  while(millis()-timer<5000) {
    lservo.write(180);
    rservo.write(180);
    int pin = 5 + random(3);
    digitalWrite(pin, HIGH);
    delay(100);
    digitalWrite(pin,LOW);
  }
  digitalWrite(GREEN,HIGH);
  lservo.write(93);
  rservo.write(90);
}

//##############################################################################################################################################
//Ultrasonic sensor code
//##############################################################################################################################################

double readUltra() {
  double ret;
  double duration;
  
  
    digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  duration = (double) pulseIn(ECHO, HIGH);
  ret = duration/74.0/2.0;
  
  return ret;
}

void calibrate() {
  double current = readUltra();
  nearBound = current-0.5;
  farBound = current+1;
}

//##############################################################################################################################################
//Initialization Functions
//##############################################################################################################################################

void LEDinit(){
  int LEDdelay = 250;
  pinMode(RED, OUTPUT);     
  pinMode(GREEN, OUTPUT);     
  pinMode(BLUE, OUTPUT);   
   //Turn all off
  digitalWrite(RED,LOW);
  digitalWrite(GREEN,LOW);
  digitalWrite(BLUE,LOW);

  //Test LEDs
  digitalWrite(RED,HIGH);
  delay(LEDdelay);
  digitalWrite(RED,LOW);
  digitalWrite(GREEN,HIGH);
  delay(LEDdelay);
  digitalWrite(GREEN,LOW);
  digitalWrite(BLUE,HIGH);
  delay(LEDdelay);
  digitalWrite(BLUE,LOW);
  delay(LEDdelay);
  }
  
void SERVOinit() {
    lservo.attach(LSERVO);
    rservo.attach(RSERVO);
    lservo.write(93);
    rservo.write(90);
  }
  //
void BUTTONinit() {
  attachInterrupt(0,bumped,RISING);
  attachInterrupt(1,movingSwitch,RISING);
}

void IRinit() {
  pinMode(IR, INPUT);
}

void ULTRAinit() {
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
}

//##############################################################################################################################################
//Main Code
//##############################################################################################################################################

void setup() {
  LEDinit();
  SERVOinit();
  BUTTONinit();
  IRinit();
  ULTRAinit();
  calibrate();
  Serial.begin(9600);
  Serial.println("Setup complete.");
}

void loop() {
  
  if(beginMoving&&analogRead(IR)>900) {
    movingSwitch();
    victoryDance();
  }
  
  if(beginMoving) {
    if(first) {
     first = false;
     delay(10000); 
    }
    if(wall) {
      wall = false;
      turnLeft();
    } else {
      followWall();
    }
  } else {
    halt();
  }
   
}
