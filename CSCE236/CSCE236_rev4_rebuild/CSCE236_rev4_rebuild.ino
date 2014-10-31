
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
#define RED A4
#define GREEN A3
#define BLUE A2
#define LSERVO 6
#define RSERVO 5
#define BUTTON1 2
#define BUTTON2 3
#define IRL A1
#define IRR A5

LiquidCrystal lcd(13,12,11,10,9,8);
Servo lservo;
Servo rservo;

int irvalueR;
int irvalueL;
int buttonDelay = 75;
int moveDelay = 50;
int turnDelay = 50;
int lcdDelay = 500;
int lcdMode = 0;
int rMin = 1023;
int rMax = 0;
int lMin = 1023;
int lMax = 0;

long lastButtonPress = 0;
long turnTimer;
long irvaluetimer;
long lcdTimer = 0;
long calTime = 0;
long convTimer = 0;
long gradTimer = 0;

boolean beginMoving = false;
boolean lcdPrint = false;
boolean rightSide = false;
boolean leftSide = false;
boolean calibration = true;
boolean straightLine = false;

//##############################################################################################################################################
//Initializing Functions
//##############################################################################################################################################
void LEDInit(){
  pinMode(RED, OUTPUT);     
  pinMode(GREEN, OUTPUT);     
  pinMode(BLUE, OUTPUT);     
   //Turn all off
  digitalWrite(RED,HIGH);
  digitalWrite(GREEN,HIGH);
  digitalWrite(BLUE,HIGH);

  //Test LEDs
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Testing LEDs...");
  digitalWrite(RED,LOW);
  delay(200);
  digitalWrite(RED,HIGH);
  digitalWrite(GREEN,LOW);
  delay(200);
  digitalWrite(GREEN,HIGH);
  digitalWrite(BLUE,LOW);
  delay(200);
  digitalWrite(BLUE,HIGH);
  delay(200);
  lcd.print("Done.");
  }
  
void SERVOinit() {
    lservo.attach(LSERVO);
    rservo.attach(RSERVO);
    lservo.write(93);
    rservo.write(90);
  }
  //
void BUTTONinit() {
  attachInterrupt(1, movingSwitch, RISING);
  attachInterrupt(0, lcdSwitch, RISING);
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

//##############################################################################################################################################
//Interrupts
//##############################################################################################################################################

void movingSwitch() {
	if(calibration) {
		calibration = false;
		lcdPrint = true;
	} else if (lcdMode==3) {
           lcd.clear();
           lcd.print(analogRead(IRL));
        } else if (lcdMode==4) {
           straightLine = !straightLine;
        } else {
	  beginMoving = !beginMoving;
	  lcdPrint = !lcdPrint;
	  digitalWrite(RED, !beginMoving);
	}
}

void lcdSwitch() {
	if(!calibration) {
	  lcdMode = (++lcdMode)%5;
	  lcd.clear();
	}
}

//##############################################################################################################################################
//Setup
//##############################################################################################################################################

void printIR() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("IRL");
  lcd.setCursor(8,0);
  lcd.print("IRR");
  lcd.setCursor(0,1);
  lcd.print(analogRead(IRL));
  lcd.setCursor(8,1);
  lcd.print(analogRead(IRR));
}

void printCal() {
		lcd.clear();
		lcd.print("LMaxLMinRMaxRMin");
		lcd.setCursor(0,1);
		lcd.print(lMax);
		lcd.setCursor(4,1);
		lcd.print(lMin);
		lcd.setCursor(8,1);
		lcd.print(rMax);
		lcd.setCursor(12,1);
		lcd.print(rMin);
}

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
		if(time-calTime>150) {
			calTime = time;
			lcd.clear();
			lcd.print("LMaxLMinRMaxRMin");
			lcd.setCursor(0,1);
			lcd.print(lMax);
			lcd.setCursor(4,1);
			lcd.print(lMin);
			lcd.setCursor(8,1);
			lcd.print(rMax);
			lcd.setCursor(12,1);
			lcd.print(rMin);
		}
		irvalueR = (rMin+rMax)/2;
		irvalueL = (lMin+lMax)/2;
}

void setup() {
  Serial.begin(9600);
  lcd.begin(16,2); 
  lcd.print("Starting up...");
  
  LEDInit();
  SERVOinit();
  IRinit();
  BUTTONinit();
  
  irvaluetimer = millis();
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Finished");
  lcd.setCursor(0,1);
  lcd.print("setup.");
  
}

//##############################################################################################################################################
//Main Code
//##############################################################################################################################################
void loop() {
  
//  unsigned long long startt,stopt,dur=0;
  
//  startt = micros();
	
  if(calibration) {
    calibrate();
  }
  
  if(lcdPrint && lcdMode==0 && millis()-lcdTimer>lcdDelay) {
    lcdTimer = millis();
    lcd.clear();
    lcd.print("Ready");
  }
  
  if(lcdPrint && lcdMode==1 && millis()-lcdTimer>lcdDelay) {
    lcdTimer = millis();
    printIR();
  }

  if(lcdPrint && lcdMode==2 && millis()-lcdTimer>lcdDelay) {
    lcdTimer = millis();
    printCal();
  }
  
   if(lcdPrint && lcdMode==4 && millis()-lcdTimer>lcdDelay) {
    lcdTimer = millis();
    lcd.clear();
    lcd.print("Straight Line");
  }
  
  if(straightLine) {
    forward(1);
    Serial.println(analogRead(IRL));
  }
  
  if(!beginMoving&&!straightLine) {
    lservo.write(95);
    rservo.write(90);
  }

  if(beginMoving && analogRead(IRL)>irvalueL && analogRead(IRR)<irvalueR) {
    forward(1);
    rightSide = true;
    leftSide = false;
    Serial.print(analogRead(IRL));
    Serial.print("\t");
    Serial.print(analogRead(IRR));
    Serial.print("\t");
    Serial.print("0");
    Serial.print("\n");
    
  }

  if(beginMoving && analogRead(IRL)<irvalueL && analogRead(IRR)>irvalueR) {
    forward(1); 
    rightSide = false;
    leftSide = true;
    Serial.print(analogRead(IRL));
    Serial.print("\t");
    Serial.print(analogRead(IRR));
    Serial.print("\t");
    Serial.print("0");
    Serial.print("\n");
  }

  if(beginMoving && analogRead(IRL)>irvalueL && analogRead(IRR)>irvalueR) {
    if (rightSide) {
          rightTurn(1);
          Serial.print(analogRead(IRL));
    Serial.print("\t");
    Serial.print(analogRead(IRR));
    Serial.print("\t");
    Serial.print("1");
    Serial.print("\n");
      } 
    if (leftSide) {
          leftTurn(1);
          Serial.print(analogRead(IRL));
    Serial.print("\t");
    Serial.print(analogRead(IRR));
    Serial.print("\t");
    Serial.print("2");
    Serial.print("\n");
    }
  }

   if(beginMoving && analogRead(IRL)<irvalueL && analogRead(IRR)<irvalueR) {
     if (rightSide) {
          leftTurn(1);
          Serial.print(analogRead(IRL));
    Serial.print("\t");
    Serial.print(analogRead(IRR));
    Serial.print("\t");
    Serial.print("2");
    Serial.print("\n");
     }
     if (leftSide) {
          rightTurn(1);
          Serial.print(analogRead(IRL));
    Serial.print("\t");
    Serial.print(analogRead(IRR));
    Serial.print("\t");
    Serial.print("1");
    Serial.print("\n");
     }
  }
//    stopt = micros();
    
//      dur = (stopt-startt);

}

