#define FORWARD 1
#define BACKWARD 0

#define M1A 11
#define M1B 10
#define M1EN 37
#define M1TRI 35
#define M1ENCA 21
#define M1ENCB 20

#define M2A 12
#define M2B 13
#define M2EN 25
#define M2TRI 23
#define M2ENCA 19
#define M2ENCB 18

#define M3A 45
#define M3B 46
#define M3EN 39
#define M3TRI 28
#define M1ENCA 2
#define M1ENCB 3

#define M4A 9
#define M4B 44
#define M4EN 25
#define M4TRI 23

#define LED 38

volatile int m1Absolute = 0;
volatile int m1Counter = 0;
volatile int m1PrevState = 0;
volatile int m1CurrentState = 0;
volatile int m1Fault = 0;

volatile int m2Absolute = 0;
volatile int m2Counter = 0;
volatile int m2PrevState = 0;
volatile int m2CurrentState = 0;
volatile int m2Fault = 0;

volatile int m3Absolute = 0;
volatile int m3Counter = 0;
volatile int m3PrevState = 0;
volatile int m3CurrentState = 0;
volatile int m3Fault = 0;

void setup() {
  Serial.begin(115200);
  pinMode(M1A, OUTPUT);
  pinMode(M1B, OUTPUT);
  pinMode(M1EN, OUTPUT);
  pinMode(M1TRI, OUTPUT);
  digitalWrite(M1EN, HIGH);
  digitalWrite(M1TRI, HIGH);
  
  pinMode(M2A, OUTPUT);
  pinMode(M2B, OUTPUT);
  pinMode(M2EN, OUTPUT);
  pinMode(M2TRI, OUTPUT);
  digitalWrite(M2EN, HIGH);
  digitalWrite(M2TRI, HIGH);
  
  pinMode(M3A, OUTPUT);
  pinMode(M3B, OUTPUT);
  pinMode(M3EN, OUTPUT);
  pinMode(M3TRI, OUTPUT);
  digitalWrite(M3EN, HIGH);
  digitalWrite(M3TRI, HIGH);
  
  pinMode(M4A, OUTPUT);
  pinMode(M4B, OUTPUT);
  pinMode(M4EN, OUTPUT);
  pinMode(M4TRI, OUTPUT);
  digitalWrite(M4EN, HIGH);
  digitalWrite(M4TRI, HIGH);
  
  pinMode(LED, OUTPUT);
  
  pinMode(M1ENCA, INPUT);
  pinMode(M1ENCB, INPUT);
  
  pinMode(M2ENCA, INPUT);
  pinMode(M2ENCB, INPUT);
  
  pinMode(M3ENCA, INPUT);
  pinMode(M3ENCB, INPUT);
  
  attachInterrupt(0, m1Encode, CHANGE);
  attachInterrupt(1, m1Encode, CHANGE);

  attachInterrupt(2, m2Encode, CHANGE);
  attachInterrupt(3, m2Encode, CHANGE);
  
  attachInterrupt(4, m3Encode, CHANGE);
  attachInterrupt(5, m3Encode, CHANGE);
}

void loop() {
  Serial.println("Motor forward");
  Serial.print("Current pos: ");
  Serial.println(counterSyringe);
  motorF(M1A);
  Serial.println("Motor backward");
  Serial.print("Current pos: ");
  Serial.println(counterSyringe);
  motorB(M1B);
}

//######################################################################################################################

void motorF(int pin) {
  digitalWrite(LED, HIGH);
  delay(250);
  digitalWrite(LED, LOW);
  digitalWrite(pin, HIGH);
  long start = millis();
  while(millis() - start < 250) {
    asm("nop");
  }
  digitalWrite(pin, LOW);
}

void motorB(int pin) {
  digitalWrite(LED, HIGH);
  delay(250);
  digitalWrite(LED, LOW);
  delay(250);
  digitalWrite(LED, HIGH);
  delay(250);
  digitalWrite(LED, LOW);
  digitalWrite(pin, HIGH);
  long start = millis();
  while(millis() - start < 250) {
    asm("nop");
  }
  digitalWrite(pin, LOW);
}

//######################################################################################################################

void m1Encode() {
  m1CurrentState = getState(digitalRead(M1ENCA), digitalRead(M1ENCB));

  if(((m1PrevState +1)%4) == m1CurrentState) {
    m1Counter++;
    m1Absolute++;
  } 
  else if (((m1PrevState -1 +4)%4) == m1CurrentState) {
    m1Counter--;
    m1Absolute--;
  } 
  else {
    m1Fault++;
  }
  m1PrevState = m1CurrentState;
}

void m2Encode() {
  m2CurrentState = getState(digitalRead(M2ENCA), digitalRead(M2ENCB));

  if(((m2PrevState +1)%4) == m2CurrentState) {
    m2Counter++;
    m2Absolute++;
  } 
  else if (((m2PrevState -1 +4)%4) == m2CurrentState) {
    m2Counter--;
    m2Absolute--;
  } 
  else {
    m2Fault++;
  }
  m2PrevState = m2CurrentState;
}

void m3Encode() {
  m3CurrentState = getState(digitalRead(M3ENCA), digitalRead(M3ENCB));

  if(((m3PrevState +1)%4) == m3CurrentState) {
    m3Counter++;
    m3Absolute++;
  } 
  else if (((m3PrevState -1 +4)%4) == m3CurrentState) {
    m3Counter--;
    m3Absolute--;
  } 
  else {
    m3Fault++;
  }
  m3PrevState = m3CurrentState;
}

int getState(int A, int B) {
  if(A) {
    if(B) {
      return 1;
    } 
    else {
      return 2;
    }
  } 
  else {
    if(B) {
      return 0;
    } 
    else {
      return 3;
    }
  }
}


