
/*

Obstacle avoidance code

This program is meant to be used with an arduino based robot using two IR sensors and two bump sensors.
It is capable of following a line and avoiding obstacle as they appear on the track. The maximum width for these
obstacles is approximately 4 inches.

Class: CSCE 236
Author: Christian Laney, David Shriver
Date of last revision: 4/30
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
#define BUMPL A5
#define BUMPR A4
#define GOBUTTON 3
#define MODEBUTTON 2
#define IRL A3
#define IRR A2
#define MODEBLUE 12
#define MODEGREEN 11

Servo lservo;
Servo rservo;

  int lspeed = 120;

//Sensor value variables
int irvalueR;
int irvalueL;
int rMin = 1023;
int rMax = 0;
int lMin = 1023;
int lMax = 0;

//Button debounce variables
int buttonStateGo = HIGH;
int lastButtonStateGo = HIGH;
long lastDebounceTimeGo = 0;
long debounceDelayGo = 50;

int buttonStateMode = HIGH;
int lastButtonStateMode = HIGH;
long lastDebounceTimeMode = 0;
long debounceDelayMode = 50;

//Obstacle variables
long obsTimer = 0;
int obsDelay = 50;
long lineTimer = 0;

//state variables
boolean obstacleR = false;
boolean obstacleL = false;
boolean beginMoving = false;
boolean rightSide = false;
boolean leftSide = false;
boolean calibration = true;
boolean leftObs = false;
boolean hasFoundLine = false;
//##############################################################################################################################################
//Initializing Functions
//##############################################################################################################################################
void LEDInit(){
  int LEDdelay = 100;
  pinMode(RED, OUTPUT);     
  pinMode(GREEN, OUTPUT);     
  pinMode(BLUE, OUTPUT);  
  pinMode(MODEGREEN, OUTPUT);     
  pinMode(MODEBLUE, OUTPUT);    
   //Turn all off
  digitalWrite(RED,LOW);
  digitalWrite(GREEN,LOW);
  digitalWrite(BLUE,LOW);
  digitalWrite(MODEGREEN,LOW);
  digitalWrite(MODEBLUE,LOW);

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
  digitalWrite(MODEGREEN,HIGH);
  delay(LEDdelay);
  digitalWrite(MODEGREEN,LOW);
  digitalWrite(MODEBLUE,HIGH);
  delay(LEDdelay);
  digitalWrite(MODEBLUE,LOW);
  delay(LEDdelay);
  }
  
void SERVOinit() {
    lservo.attach(LSERVO);
    rservo.attach(RSERVO);
    lservo.write(90);
    rservo.write(93);
  }
  //
void BUTTONinit() {
  pinMode(GOBUTTON, INPUT);
  pinMode(MODEBUTTON, INPUT);
  pinMode(BUMPR, INPUT_PULLUP);
  pinMode(BUMPL,INPUT_PULLUP);
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
	int lspeed = 90 + (70*speed);
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

//The robot uses state variables to know which side of the line it's on and take 
//action accordingly when faced with leaving the line or both sensors seeing black.
void followLine() {
   if(beginMoving && analogRead(IRL)>irvalueL && analogRead(IRR)<irvalueR) {
    forward(1);
    lineTimer = millis();
    hasFoundLine = false;
    rightSide = true;
    leftSide = false;
  }

  if(beginMoving && analogRead(IRL)<irvalueL && analogRead(IRR)>irvalueR) {
    forward(1); 
    lineTimer = millis();
    hasFoundLine = false;
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
     if(millis()-lineTimer>2000) {
      findLine(); //If the robot has been off of the line for more than two seconds, it will attempt to find the line again.
     } else {
       if (rightSide) {
          leftTurn(1);
       }
       if (leftSide) {
          rightTurn(1);
       }
     }
  } 
}

//This function will cause the robot to slowly spiral out from its current position until it sees the line again.
void findLine() {
   
      if(rightSide) {
        rservo.write(0);
        lservo.write(108);
      }else {
        lservo.write(180);
        rservo.write(80);
   }
}

//A hard coded set of instructions for dodging any obstacle, whether it's a block or another robot.
//If the robot attempts to dodge multiple obstacles, it will default to wall following.
void avoidObstacle(int val) {
  
  
  if((val==3&&leftObs)||val==0) {
  lservo.write(0);
  rservo.write(180);
  delay(200);
  rservo.write(93);
  lservo.write(0);
  delay(500);
  lservo.write(lspeed);
  rservo.write(0);
  delay(900);
  lservo.write(180);
  rservo.write(93);
  delay(500);
  lservo.write(lspeed);
  rservo.write(0);
  delay(900);
  lservo.write(180);
  rservo.write(93);
  delay(400);
  lservo.write(lspeed);
  rservo.write(0);
  while(analogRead(IRR) < irvalueR){
    if(!digitalRead(BUMPR)||!digitalRead(BUMPL)) {avoidObstacle(4); break;}
    delay(1);
  }
 } else if((val==3&&!leftObs)||val==1) {
   lservo.write(0);
  rservo.write(180);
  delay(200);
  rservo.write(180);
  lservo.write(90);
  delay(500);
  lservo.write(lspeed-5);
  rservo.write(0);
  delay(900);
  lservo.write(90);
  rservo.write(0);
  delay(500);
  lservo.write(lspeed-5);
  rservo.write(0);
  delay(900);
  lservo.write(90);
  rservo.write(0);
  delay(400);
  lservo.write(180);
  rservo.write(0);
  while(analogRead(IRR) < irvalueR){
    if(!digitalRead(BUMPR)||!digitalRead(BUMPL)) {avoidObstacle(5); break;}
    delay(1);
  }
 } else if(val == 4) {
   shortLeft();
  while(analogRead(IRR)<irvalueR) {
    if(!digitalRead(BUMPR)||!digitalRead(BUMPL)) {
      shortRight();
    }
     delay(1); 
  }
  } else if (val==5) {
    shortRight();
  while(analogRead(IRR) < irvalueR){
    if(!digitalRead(BUMPR)||!digitalRead(BUMPL)) {
      shortLeft();
    }
   delay(1); 
  }
  }
 }
 void shortRight() {
  lservo.write(0);
  rservo.write(180);
  delay(200);
  rservo.write(180);
  lservo.write(90);
  delay(500);
  lservo.write(lspeed-5);
  rservo.write(0);
  delay(900);
  lservo.write(90);
  rservo.write(0);
  delay(500);
  lservo.write(lspeed-5);
  rservo.write(0);
 }
 
 void shortLeft() {
   lservo.write(0);
  rservo.write(180);
  delay(200);
  rservo.write(93);
  lservo.write(0);
  delay(500);
  lservo.write(lspeed);
  rservo.write(0);
  delay(900);
  lservo.write(180);
  rservo.write(93);
  delay(500);
  lservo.write(lspeed);
  rservo.write(0);
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
    modeLED(false);
}

boolean goButtonPressed(){
  int reading = digitalRead(GOBUTTON);
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

//The robot can be set to go to the left or the right of the line when both bump sensors are pressed at the same time,
//indicating that the robot has detected either a block or another robot. This allows us to avoid the situation where
//both robots attempt to go the same way around an obstacle.
boolean modeButtonPressed(){
  int reading = digitalRead(MODEBUTTON);
  boolean pressed = false;
  if (reading != lastButtonStateMode) {
    lastDebounceTimeMode = millis();
  } 
  
  if ((millis() - lastDebounceTimeMode) > debounceDelayMode) {
    if (reading != buttonStateMode) {
      buttonStateMode = reading;
      if (buttonStateMode == HIGH) {
        pressed = true;
      }
    }
  }
  lastButtonStateMode = reading;
  return pressed;
}

void changeMode() {
  leftObs = !leftObs;
}

void modeLED(boolean val) {
  if(val) {
    if(leftObs) {
      digitalWrite(MODEGREEN,LOW);
    digitalWrite(MODEBLUE,HIGH);
    } else {
      digitalWrite(MODEGREEN,HIGH);
    digitalWrite(MODEBLUE,LOW);
    }
  } else {
    digitalWrite(MODEGREEN,LOW);
    digitalWrite(MODEBLUE,LOW);
  }
}


//##############################################################################################################################################
//Setup
//##############################################################################################################################################

//A calibration function for the line sensors that sets the partially dynamic threshold.
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

}

//##############################################################################################################################################
//Main Code
//##############################################################################################################################################
void loop() {
  
  //The following two functions acknowledge button presses.
  if(goButtonPressed()){
    lineTimer = millis();
    movingSwitch();
  }
  
  if(modeButtonPressed()) {
     changeMode(); 
  }
  
  //acknowledge that the right bump sensor has been triggered.
  if(!digitalRead(BUMPR)) {
    if(!obstacleR) {
      obsTimer = millis(); 
    }
     obstacleR = true; 
  }
  
  //acknowledge that the left bump sensor has been triggered.
  if(!digitalRead(BUMPL)) {
    if(!obstacleL) {
      obsTimer = millis(); 
    }
     obstacleL = true; 
  }
  
  //Calibrate the robot	
  if(calibration) {
    calibrate();
    digitalWrite(BLUE,HIGH);
    modeLED(true);
  }
  
  //Main movement code
  if(!beginMoving) {
    halt();
  } else {
    if(millis()-obsTimer>obsDelay && (obstacleR||obstacleL)) { //check to see if both sensors have been triggered, or just one.
      if(obstacleL && obstacleR) {
        obstacleR = false;
        obstacleL = false;
        avoidObstacle(3);
      } else if(obstacleL) {
        obstacleL = false;
        avoidObstacle(1);
      }else if(obstacleR) {
        obstacleR = false;
        avoidObstacle(0);
      }
    }else {
      followLine();
    } 
  }
}

