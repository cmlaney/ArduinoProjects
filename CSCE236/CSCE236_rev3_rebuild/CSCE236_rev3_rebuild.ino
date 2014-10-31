
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
#define IRLED 7

LiquidCrystal lcd(13,12,11,10,9,8);
Servo lservo;
Servo rservo;
int irvalue;
int buttonDelay = 75;
int moveDelay = 50;
int turnDelay = 50;
int lcdDelay = 250;
int lcdMode = 0;
long lastButtonPress = 0;
long turnTimer;
long irvaluetimer;
long lcdTimer = 0;
boolean beginMoving = false;
boolean lcdPrint = true;
boolean rightSide = false;
boolean leftSide = false;

boolean redled = false;
boolean ledTest = false;

//##############################################################################################################################################
//Initializing Functions
//##############################################################################################################################################
void LEDInit(){
  pinMode(RED, OUTPUT);     
  pinMode(GREEN, OUTPUT);     
  pinMode(BLUE, OUTPUT);     
  pinMode(IRLED, OUTPUT);
   //Turn all off
  digitalWrite(RED,HIGH);
  digitalWrite(GREEN,HIGH);
  digitalWrite(BLUE,HIGH);
  digitalWrite(IRLED, HIGH);

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
  digitalWrite(IRLED,HIGH);
  delay(200);
  digitalWrite(IRLED,LOW);
  Serial.println("done.");
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
void forward() {
    rservo.write(30);
    lservo.write(180);
    delay(moveDelay);
    lservo.write(93);
    rservo.write(90);
}

void backward() {
    rservo.write(180);
    lservo.write(55);
    delay(moveDelay);
    lservo.write(93);
    rservo.write(90);
}

void leftTurn() {
    rservo.write(0);
    lservo.write(0);
    delay(turnDelay);
    rservo.write(90);
    lservo.write(93); 
}

void rightTurn() {
   lservo.write(180);
   rservo.write(180);
   delay(turnDelay);
   lservo.write(93);
  rservo.write(90); 
}

//##############################################################################################################################################
//Interrupts
//##############################################################################################################################################

void movingSwitch() {
  beginMoving = !beginMoving;
  lcdPrint = !lcdPrint;
  digitalWrite(RED, !beginMoving);
}

void lcdSwitch() {
  lcdMode = (++lcdMode)%3;
  lcd.clear();
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

void setup() {
  
  lcd.begin(16,2); 
  lcd.print("Starting up...");
  
  LEDInit();
  SERVOinit();
  IRinit();
  BUTTONinit();
  
  irvalue = 500;
  irvaluetimer = millis();
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Finished");
  lcd.setCursor(0,1);
  lcd.print("setup.");
  
  digitalWrite(IRLED, HIGH);
}

//##############################################################################################################################################
//Main Code
//##############################################################################################################################################
void loop() {
  
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
    lcd.clear();
    if(rightSide) {
      lcd.print("Right");
    } else {
      lcd.print("Left");
    }
  }
  
  if(analogRead(IRL)>irvalue && analogRead(IRR)<irvalue && beginMoving) {
    forward();
    rightSide = true;
    leftSide = false;
  }
  
  if(analogRead(IRL)<irvalue && analogRead(IRR)>irvalue && beginMoving) {
    forward(); 
    rightSide = false;
    leftSide = true;
  }
  
  if(analogRead(IRL)>irvalue && analogRead(IRR)>irvalue && beginMoving) {
    if (rightSide) {
          rightTurn();
      } 
    if (leftSide) {
          leftTurn();
    }
  
  }
  
   if(analogRead(IRL)<irvalue && analogRead(IRR)<irvalue && beginMoving) {
     if (rightSide) {
          leftTurn();
     }
     if (leftSide) {
          rightTurn();
     }


}

}


