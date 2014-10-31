
#include <avr/io.h>
#include <PinChangeInt.h>

#define FORWARD 1
#define BACKWARD 0

#define DRIVEMOTORLIMIT 10
#define DRIVEMOTORFORWARDCONTROLPIN 4
#define DRIVEMOTORREVERSECONTROLPIN 5

#define ROTATEMOTORLIMIT 12
#define ROTATEMOTORFORWARDCONTROLPIN 6
#define ROTATEMOTORREVERSECONTROLPIN 7

#define SYRINGEMOTORFORWARDCONTROLPIN 8
#define SYRINGEMOTORREVERSECONTROLPIN 9
#define INJECTIONDELAY 500
#define SYRINGEENCODE1 2
#define SYRINGEENCODE2 3

#define DRIVEMOTORLED A0
#define ROTATEMOTORLED A1
#define SYRINGEMOTORLED A2

int dropCount = 0;
boolean abortDrop = false;

boolean first = true;
int state = 0;

volatile int syringeAbsolute = 0;
volatile int counterSyringe = 0;
volatile int prevStateSyringe = 0;
volatile int currentStateSyringe = 0;
volatile int faultSyringe = 0;

void setup() {
  //setup xbee
  Serial.begin(57600);
  
  //setup motor controller
  pinMode(DRIVEMOTORLIMIT, INPUT);
  pinMode(ROTATEMOTORLIMIT, INPUT);
  pinMode(DRIVEMOTORFORWARDCONTROLPIN, OUTPUT);
  pinMode(DRIVEMOTORREVERSECONTROLPIN, OUTPUT);
  pinMode(ROTATEMOTORFORWARDCONTROLPIN, OUTPUT);
  pinMode(ROTATEMOTORREVERSECONTROLPIN, OUTPUT);
  pinMode(SYRINGEMOTORFORWARDCONTROLPIN, OUTPUT);
  pinMode(SYRINGEMOTORREVERSECONTROLPIN, OUTPUT);
  pinMode(11, OUTPUT);
  
  digitalWrite(DRIVEMOTORFORWARDCONTROLPIN, LOW);
  digitalWrite(DRIVEMOTORREVERSECONTROLPIN, LOW);
  digitalWrite(ROTATEMOTORFORWARDCONTROLPIN, LOW);
  digitalWrite(ROTATEMOTORREVERSECONTROLPIN, LOW);
  digitalWrite(SYRINGEMOTORFORWARDCONTROLPIN, LOW);
  digitalWrite(SYRINGEMOTORREVERSECONTROLPIN, LOW);
  digitalWrite(11, HIGH);
  
  //setup leds 
  pinMode(DRIVEMOTORLED, OUTPUT);
  pinMode(ROTATEMOTORLED, OUTPUT);
  pinMode(SYRINGEMOTORLED, OUTPUT);
  
  digitalWrite(DRIVEMOTORLED, HIGH);
  delay(333);
  digitalWrite(DRIVEMOTORLED, LOW);
  
  digitalWrite(ROTATEMOTORLED, HIGH);
  delay(333);
  digitalWrite(ROTATEMOTORLED, LOW);
  
  digitalWrite(SYRINGEMOTORLED, HIGH);
  delay(333);
  digitalWrite(SYRINGEMOTORLED, LOW);
  
  //setup limit switches
  pinMode(DRIVEMOTORLIMIT, INPUT);
  pinMode(ROTATEMOTORLIMIT, INPUT);  
  
  //setup interrupts for syringe
  pinMode(SYRINGEENCODE1, INPUT);
  pinMode(SYRINGEENCODE2, INPUT);
  
  attachInterrupt(0, encodeSyringe, CHANGE);
  attachInterrupt(1, encodeSyringe, CHANGE);
  
  initialize();
}

void loop() {
  //on signal
  
}

void initialize() {
  
}

void execute(int count) {
  int i;
  for(i=0; i<c; i++) 
  {
    Serial.print("Preparing to drop ball: "); Serial.print(++dropCount); Serial.println("");
    digitalWrite(DRIVEMOTORLED, HIGH);
    driveBall(1);
    digitalWrite(DRIVEMOTORLED, LOW);
    Serial.print("Injecting ball: "); Serial.print(dropCount);
    digitalWrite(SYRINGEMOTORLED, HIGH);
    if(!abortFlag) {
      inject(0);
    }
    digitalWrite(SYRINGEMOTORLED, LOW);
    driveBall(0);
    Serial.print("Retracting ball: "); Serial.print(dropCount);
    abortFlag = false;
    delay(500);
    Serial.print("Dropping ball: "); Serial.print(dropCount); Serial.println("\n\n");
    rotateWheel();
  }

}

void getState() {
  Serial.print("State "); Serial.println(state); Serial.println("");
}

//function to rotate ball carrier
void rotateWheel() {
  state = 1;
  long rotateStart = millis();
  int rotateSafetyDelay = 1000;
  
  digitalWrite(ROTATEMOTORFORWARDCONTROLPIN, HIGH);
  delay(50);
  while(digitalRead(ROTATEMOTORLIMIT)) 
  {
    rotateCurrent = millis();
    if(millis()-rotateStart > rotateSafetyDelay) 
    {
      digitalWrite(ROTATEMOTORFORWARDCONTROLPIN, LOW);
      abortDrop = true;
      error("Rotate failed.\n");
      return;
    }
    delay(10);
  }
  
  digitalWrite(ROTATEMOTORFORWARDCONTROLPIN, LOW);
  
  digitalWrite(ROTATEMOTORREVERSECONTROLPIN, HIGH);
  while(digitalRead(ROTATEMOTORLIMIT)) 
  {
   delay(10); 
  }
  digitalWrite(ROTATEMOTORREVERSECONTROLPIN, LOW);
}

//function to inject fluid
void inject() {
  state = 3;
  long injectStart = millis();
  int injectSafetyDelay = 2000;
  
  counterSyringe = 0;
  digitalWrite(SYRINGEMOTORFORWARDCONTROLPIN, HIGH);
  while(counterSyringe < INJECTIONDELAY) {
    if(millis()-injectStart > injectSafetyDelay) {
      abortDrop = true;
      error("Inject failed.\n");
      return;
    }
    delay(10);
  }
  digitalWrite(SYRINGEMOTORFORWARDCONTROLPIN, LOW);
}

void syringeReposition(int c) {
  if(c == 0)
  {
    digitalWrite(SYRINGEMOTORREVERSECONTROLPIN, HIGH);
    while(syringeAbsolute > 0) {
      delay(1);
    }
    digitalWrite(SYRINGEMOTORREVERSECONTROLPIN, LOW);
  } 
  else if (c > 0) 
  {
    digitalWrite(SYRINGEMOTORFORWARDCONTROLPIN, HIGH);
    delay(c);
    digitalWrite(SYRINGEMOTORFORWARDCONTROLPIN, LOW);
  } 
  else 
  {
    digitalWrite(SYRINGEMOTORREVERSECONTROLPIN, HIGH);
    delay(c*(-1));
    digitalWrite(SYRINGEMOTORREVERSECONTROLPIN, LOW);
  }
}

//function to move ball forward/backward
void driveBall(int dir) {
  
  long driveStart = millis();
  int driveSafetyDelay = 3000;
  
   if(dir == FORWARD) 
   state = 2;
   {
     digitalWrite(DRIVEMOTORFORWARDCONTROLPIN, HIGH);
     delay(300);
     
     while(digitalRead(DRIVEMOTORLIMIT)) 
     {
       if(millis()-driveStart > driveSafetyDelay) 
       {
         abortFlag = true;
         error("Ball can't drive forward.\n");
         return;
       }
       delay(10);
     }
     delay(300);
     
     digitalWrite(DRIVEMOTORFORWARDCONTROLPIN, LOW);
     
   } 
   else if (dir == BACKWARD)
   state = 4; 
   {  
     digitalWrite(DRIVEMOTORREVERSECONTROLPIN, HIGH);
     delay(500);
     
     while(digitalRead(DRIVEMOTORLIMIT)) 
     {
       if(millis()-driveStart > driveSafetyDelay) 
       {
         abortFlag = true;
         error("Ball can't drive backward.\n");
         return;
       }
       delay(10);
     }
     digitalWrite(DRIVEMOTORREVERSECONTROLPIN, LOW);
   }
}

void error(String s) {
  Serial.print("ERROR: "); Serial.println(s);
}

void encodeSyringe() {
  currentStateSyringe = getState(digitalRead(SYRINGEENCODE1), digitalRead(SYRINGEENCODE2));
  
  if(((prevStateSyringe +1)%4) == currentStateSyringe) {
    counterSyringe++;
    syringeAbsolute++;
  } else if (((prevStateSyringe -1 +4)%4) == currentStateSyringe) {
    counterSyringe--;
    syringeAbsolute--;
  } else {
    faultSyringe++;
  }
  prevStateSyringe = currentStateSyringe;
}

int getState(int A, int B) {
  if(A) {
    if(B) {
      return 1;
    } else {
      return 2;
    }
  } else {
    if(B) {
      return 0;
    } else {
      return 3;
    }
  }
}

