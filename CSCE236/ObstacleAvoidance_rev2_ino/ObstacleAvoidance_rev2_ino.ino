
/*
Code to run an arduino based, line-following robot using two IR sensors.

Class: CSCE 236
Author: Christian Laney
Date of last revision: 3/10
*/


#include <avr/io.h>
#include <Servo.h>
#include <LiquidCrystal.h>

// Pin 13 has an LED connected on most Arduino boards:
#define RED 5
#define GREEN 6
#define BLUE 7
#define LSERVO 9
#define RSERVO 10
#define BUMP 2
#define BUTTON 3
#define IRL A3
#define IRR A2

Servo lservo;
Servo rservo;

int irvalueR;
int irvalueL;
int buttonDelay = 75;
int moveDelay = 50;
int turnDelay = 50;
int rMin = 1023;
int rMax = 0;
int lMin = 1023;
int lMax = 0;

int buttonState = HIGH;
int lastButtonState = HIGH;
long lastDebounceTime = 0;
long debounceDelay = 50;

long lastButtonPress = 0;
long turnTimer;
long irvaluetimer;
long calTime = 0;
long convTimer = 0;
long gradTimer = 0;

boolean obstacle = false;
boolean beginMoving = false;
boolean rightSide = false;
boolean leftSide = false;
boolean calibration = true;

//##############################################################################################################################################
//Initializing Functions
//##############################################################################################################################################
void LEDInit(){
  pinMode(RED, OUTPUT);     
  pinMode(GREEN, OUTPUT);     
  pinMode(BLUE, OUTPUT);     
   //Turn all off
  digitalWrite(RED,LOW);
  digitalWrite(GREEN,LOW);
  digitalWrite(BLUE,LOW);

  //Test LEDs
  digitalWrite(RED,HIGH);
  delay(200);
  digitalWrite(RED,LOW);
  digitalWrite(GREEN,HIGH);
  delay(200);
  digitalWrite(GREEN,LOW);
  digitalWrite(BLUE,HIGH);
  delay(200);
  digitalWrite(BLUE,LOW);
  delay(200);
  }
  
void SERVOinit() {
    lservo.attach(LSERVO);
    rservo.attach(RSERVO);
    lservo.write(90);
    rservo.write(93);
  }
  //
void BUTTONinit() {
  pinMode(BUTTON, INPUT);
  pinMode(BUMP,INPUT);
}

void IRinit() {
  pinMode(IRL, INPUT);
  pinMode(IRR, INPUT);
}
  
//##############################################################################################################################################
//Directional Functions
//##############################################################################################################################################
void forward(double speed) {
	int rspeed = 90 - (90*speed);
	int lspeed = 90 + (90*speed);
    rservo.write(rspeed);
    lservo.write(lspeed);
}

void leftTurn(double speed) {
    int servoTurnSpeed = 90-(90*speed);
   lservo.write(servoTurnSpeed);
   rservo.write(servoTurnSpeed);
}

void rightTurn(double speed) {
	int servoTurnSpeed = 90+(90*speed);
   lservo.write(servoTurnSpeed);
   rservo.write(servoTurnSpeed);
}

void halt() {
   lservo.write(90);
   rservo.write(93); 
}

void followLine() {
   if(beginMoving && analogRead(IRL)>irvalueL && analogRead(IRR)<irvalueR) {
    forward(1);
    rightSide = true;
    leftSide = false;
  }

  if(beginMoving && analogRead(IRL)<irvalueL && analogRead(IRR)>irvalueR) {
    forward(1); 
    rightSide = false;
    leftSide = true;
  }

  if(beginMoving && analogRead(IRL)>irvalueL && analogRead(IRR)>irvalueR) {
    if (rightSide) {
          rightTurn(1);
      } 
    if (leftSide) {
          leftTurn(1);
    }
  }

   if(beginMoving && analogRead(IRL)<irvalueL && analogRead(IRR)<irvalueR) {
     if (rightSide) {
          leftTurn(1);
     }
     if (leftSide) {
          rightTurn(1);
     }
  } 
}

void avoidObstacle() {
  lservo.write(0);
  rservo.write(180);
  delay(200);
  rservo.write(93);
  lservo.write(0);
  delay(500);
  lservo.write(180);
  rservo.write(0);
  delay(900);
  lservo.write(180);
  rservo.write(93);
  delay(500);
  lservo.write(180);
  rservo.write(0);
  delay(900);
  lservo.write(180);
  rservo.write(93);
  delay(400);
  lservo.write(180);
  rservo.write(0);
  while(analogRead(IRR) < irvalueR){
    delay(1);
  }
}

//##############################################################################################################################################
//Interrupts
//##############################################################################################################################################

void movingSwitch() {
  if(calibration) {
       calibration = false; 
       digitalWrite(BLUE,LOW);
    }
    beginMoving = !beginMoving;
    digitalWrite(RED, !digitalRead(RED));
}

boolean buttonPressed(int pin){
  int reading = digitalRead(pin);
  boolean pressed = false;
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  } 
  
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == HIGH) {
        pressed = true;
      }
    }
  }
  lastButtonState = reading;
  return pressed;
}


//##############################################################################################################################################
//Setup
//##############################################################################################################################################

void calibrate() {
	int l;
	int r;
		l = analogRead(IRL);
		r = analogRead(IRR);
		if(l<lMin) {lMin = l;}
		if(l>lMax) {lMax = l;}
		if(r<rMin) {rMin = r;}
		if(r>rMax) {rMax = r;}
		long time = millis();
		irvalueR = (rMin+rMax)/2;
		irvalueL = (lMin+lMax)/2;
}

void setup() {
  Serial.begin(9600);
  
  LEDInit();
  SERVOinit();
  IRinit();
  BUTTONinit();
  
  irvaluetimer = millis();
}

//##############################################################################################################################################
//Main Code
//##############################################################################################################################################
void loop() {
  
  Serial.print(digitalRead(BUTTON));
  Serial.print("\t");
  Serial.println(digitalRead(BUMP));
  
  if(buttonPressed(BUTTON)){
    movingSwitch();
    Serial.println("PRESS!");
  }
  
  if(!digitalRead(BUMP)) {
     obstacle = true; 
     Serial.println("BUMP!");
  }
  	
  if(calibration) {
    calibrate();
    digitalWrite(BLUE,HIGH);
  }
  
  if(!beginMoving) {
    halt();
  } else {
    if(obstacle) {
      obstacle = false;
      avoidObstacle();
    } else {
      followLine();
    } 
  }
}

