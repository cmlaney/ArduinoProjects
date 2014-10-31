#include <Wire.h>
#include <Servo.h> 

#define VCNL4000_ADDRESS 0x13  // 0x26 write, 0x27 read

// VCNL4000 Register Map
#define COMMAND_0 0x80  // starts measurments, relays data ready info
#define PRODUCT_ID 0x81  // product ID/revision ID, should read 0x11
#define IR_CURRENT 0x83  // sets IR current in steps of 10mA 0-200mA
#define AMBIENT_PARAMETER 0x84  // Configures ambient light measures
#define AMBIENT_RESULT_MSB 0x85  // high byte of ambient light measure
#define AMBIENT_RESULT_LSB 0x86  // low byte of ambient light measure
#define PROXIMITY_RESULT_MSB 0x87  // High byte of proximity measure
#define PROXIMITY_RESULT_LSB 0x88  // low byte of proximity measure
#define PROXIMITY_FREQ 0x89  // Proximity IR test signal freq, 0-3
#define PROXIMITY_MOD 0x8A  // proximity modulator timing

#define BUMP_SENSOR 2
#define BUTTON 8
#define LED 13
#define LEFT 1
#define RIGHT 2
#define RED 5
#define GREEN 6
#define BLUE 7
#define SPEED 40
#define IRL A3
#define IRR A2
#define LSERVO 9
#define RSERVO 10

Servo lservo;
Servo rservo;

// IR sensor variables
int leftIR;
int rightIR;
int LEFT_THRESHOLD;
int RIGHT_THRESHOLD;

int buttonState = HIGH;
int lastButtonState = HIGH;
long lastDebounceTime = 0;
long debounceDelay = 50;

// state variables
boolean leftSide;
boolean rightSide;
boolean go = false;
uint8_t side;
boolean obstacle = false;
boolean calibrate = false;
boolean calibrated = false;

long printTimer = 0;

uint16_t PROXIMITY_THRESHOLD = 2700;
uint32_t ambientValue, proximityValue;

uint16_t minLeft = 1024;
uint16_t maxLeft = 0;
uint16_t minRight = 1024;
uint16_t maxRight = 0;

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
    rservo.write(90);
  }
  //
void BUTTONinit() {
//  attachInterrupt(1, movingSwitch, RISING);
//  attachInterrupt(0, lcdSwitch, RISING);
    pinMode(BUTTON, INPUT_PULLUP);
    pinMode(BUMP_SENSOR, INPUT_PULLUP);
}

void IRinit() {
    pinMode(IRL, INPUT);
    pinMode(IRR, INPUT);
}

void SENSORinit() {
    Wire.begin();  // initialize I2C stuff
  
    /* Test that the device is working correctly */
    byte temp = readByte(PRODUCT_ID);
    if (temp != 0x11) { // Product ID Should be 0x11 
      Serial.print("Something's wrong. Not reading correct ID: 0x");
      Serial.println(temp, HEX);
    } else
      Serial.println("VNCL4000 Online...");
  
    /* Now some VNCL400 initialization stuff. Feel free to play with any of these values, but check the datasheet first!*/
    writeByte(AMBIENT_PARAMETER, 0x0F);  // Single conversion mode, 128 averages
    writeByte(IR_CURRENT, 20);  // Set IR current to 200mA
    writeByte(PROXIMITY_FREQ, 2);  // 781.25 kHz
    writeByte(PROXIMITY_MOD, 0x81);  // 129, recommended by Vishay
}

//##############################################################################################################################################
//Main Code
//##############################################################################################################################################

void setup()
{
  Serial.begin(9600);  // Serial's used to debug and print data
  SENSORinit();
  BUTTONinit();
  SERVOinit();
  LEDInit();
}

void loop() {
  //Print sensor values
   leftIR = analogRead(IRL);
   rightIR = analogRead(IRR);
  ambientValue = readAmbient();
  proximityValue = readProximity();
  if(millis()-printTimer>250) {
    printTimer = millis();
    Serial.print(ambientValue, DEC);
    Serial.print("\t");
    Serial.print(proximityValue, DEC);
    Serial.print("\t");
    Serial.print(leftIR);
    Serial.print("\t");
    Serial.print(rightIR);
    Serial.print("\t");
    Serial.print(LEFT_THRESHOLD);
    Serial.print("\t");
    Serial.println(RIGHT_THRESHOLD);
    
  }
 
  //Detect an obstacle
  if(!digitalRead(BUMP_SENSOR)){
    Serial.println("BUMPED");
    obstacle = true;
  } else {
    obstacle = false;
  }
  
  //Listen for button press
  if(buttonPressed(BUTTON)){
    go = !go;
    Serial.println("PRESSED");
  }
  
  if(go) {
    if(obstacle) {
      avoidObstacle();
    } else {
      followLine(leftIR, rightIR);
    }
  } else {
    halt();
        calibrateIR();

  }
  
  if(calibrate) {
  }
}

//##############################################################################################################################################
//Movement Functions
//##############################################################################################################################################

void followLine(int irleft, int irright) {
  
  
  if(irleft>LEFT_THRESHOLD && irright<RIGHT_THRESHOLD) {
    forward(1);
    rightSide = true;
    leftSide = false;
  }

  if(irleft<LEFT_THRESHOLD && irright>RIGHT_THRESHOLD) {
    forward(1); 
    rightSide = false;
    leftSide = true;
  }

  if(irleft>LEFT_THRESHOLD && irright>RIGHT_THRESHOLD) {
    if (rightSide) {
          rightTurn(0.5);
      } 
    if (leftSide) {
          leftTurn(0.5);
    }
  }

   if(irleft<LEFT_THRESHOLD && irright<RIGHT_THRESHOLD) {
     if (rightSide) {
          leftTurn(0.5);
     }
     if (leftSide) {
          rightTurn(0.5);
     }
  }
}

void avoidObstacle(){
  lservo.write(90);
  rservo.write(90);
  delay(1000);
  lservo.write(90 - SPEED);
  delay(800);
  lservo.write(90 + SPEED);
  rservo.write(90 - SPEED);
  delay(1200);
  lservo.write(90 + SPEED);
  rservo.write(90);
  delay(800);
  lservo.write(90 + SPEED);
  rservo.write(90 - SPEED);
  delay(1200);
  lservo.write(90 + SPEED);
  rservo.write(90);
  delay(600);
  while(analogRead(IRR) < RIGHT_THRESHOLD){
    if (readProximity() < PROXIMITY_THRESHOLD){
      lservo.write(150);
      rservo.write(90);
    } else {
      lservo.write(90);
      rservo.write(30);
    }
  }
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
    lservo.write(90);
//   lservo.write(servoTurnSpeed);
   rservo.write(servoTurnSpeed);
}

void rightTurn(double speed) {
	int servoTurnSpeed = 90+(90*speed);
   lservo.write(servoTurnSpeed);
   rservo.write(90);
//   rservo.write(servoTurnSpeed);
}

void halt() {
   lservo.write(90);
  rservo.write(90); 
}

//##############################################################################################################################################
//Sensor Functions
//##############################################################################################################################################

// readProximity() returns a 16-bit value from the VCNL4000's proximity data registers
unsigned int readProximity()
{
  unsigned int data;
  byte temp;
  
  temp = readByte(COMMAND_0);
  writeByte(COMMAND_0, temp | 0x08);  // command the sensor to perform a proximity measure
  
  while(!(readByte(COMMAND_0)&0x20)) 
    ;  // Wait for the proximity data ready bit to be set
  data = readByte(PROXIMITY_RESULT_MSB) << 8;
  data |= readByte(PROXIMITY_RESULT_LSB);
  
  return data;
}

// readAmbient() returns a 16-bit value from the VCNL4000's ambient light data registers
unsigned int readAmbient()
{
  unsigned int data;
  byte temp;
  
  temp = readByte(COMMAND_0);
  writeByte(COMMAND_0, temp | 0x10);  // command the sensor to perform ambient measure
  
  while(!(readByte(COMMAND_0)&0x40)) 
    ;  // wait for the proximity data ready bit to be set
  data = readByte(AMBIENT_RESULT_MSB) << 8;
  data |= readByte(AMBIENT_RESULT_LSB);
  
  return data;
}

// writeByte(address, data) writes a single byte of data to address
void writeByte(byte address, byte data)
{
  Wire.beginTransmission(VCNL4000_ADDRESS);
  Wire.write(address);
  Wire.write(data);
  Wire.endTransmission();
}

// readByte(address) reads a single byte of data from address
byte readByte(byte address)
{
  byte data;
  
  Wire.beginTransmission(VCNL4000_ADDRESS);
  Wire.write(address);
  Wire.endTransmission();
  Wire.requestFrom(VCNL4000_ADDRESS, 1);
  while(!Wire.available())
    ;
  data = Wire.read();

  return data;
}

void calibrateIR(){
  if(leftIR < minLeft){
      minLeft = leftIR;
    }
    if(leftIR > maxLeft){
      maxLeft = leftIR;
    }
    if(rightIR < minRight){
      minRight = rightIR;
    }
    if(rightIR > maxRight){
      maxRight = rightIR;
    }
    // set corresponding threshold value to the average
    LEFT_THRESHOLD = (maxLeft + minLeft) / 2;
    RIGHT_THRESHOLD = (maxRight + minRight) / 2;
}

//##############################################################################################################################################
//Sensor Functions
//##############################################################################################################################################

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
