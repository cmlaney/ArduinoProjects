/*
A sketch to power a laser ball (tennis ball containing laser modules).

The laser numbering is as follows; from the logo, left to right:

     9
 5  6  7  8
 1  2  3  4
10 11 12 13
     14

*/

#define lm1 3
#define lm2 4
#define lm3 5
#define lm4 6
#define lm5 7
#define lm6 8
#define lm7 9
#define lm8 11
#define lm9 12
#define lm10 13
#define lm11 A0
#define lm12 A1
#define lm13 A2
#define lm14 A3

void enableLasers(boolean l1, boolean l2, boolean l3, 
                  boolean l4, boolean l5, boolean l6, 
                  boolean l7, boolean l8, boolean l9, 
                  boolean l10, boolean l11, boolean l12, 
                  boolean l13, boolean l14) {
  if(l1) {digitalWrite(lm1, HIGH); delay(2); digitalWrite(lm1, LOW);}
  if(l2) {digitalWrite(lm2, HIGH); delay(2); digitalWrite(lm2, LOW);}
  if(l3) {digitalWrite(lm3, HIGH); delay(2); digitalWrite(lm3, LOW);}
  if(l4) {digitalWrite(lm4, HIGH); delay(2); digitalWrite(lm4, LOW);}
  if(l5) {digitalWrite(lm5, HIGH); delay(2); digitalWrite(lm5, LOW);}
  if(l6) {digitalWrite(lm6, HIGH); delay(2); digitalWrite(lm6, LOW);}
  if(l7) {digitalWrite(lm7, HIGH); delay(2); digitalWrite(lm7, LOW);}
  if(l8) {digitalWrite(lm8, HIGH); delay(2); digitalWrite(lm8, LOW);}
  if(l9) {digitalWrite(lm9, HIGH); delay(2); digitalWrite(lm9, LOW);}
  if(l10) {digitalWrite(lm10, HIGH); delay(2); digitalWrite(lm10, LOW);}
  if(l11) {digitalWrite(lm11, HIGH); delay(2); digitalWrite(lm11, LOW);}
  if(l12) {digitalWrite(lm12, HIGH); delay(2); digitalWrite(lm12, LOW);}
  if(l13) {digitalWrite(lm13, HIGH); delay(2); digitalWrite(lm13, LOW);}
  if(l14) {digitalWrite(lm14, HIGH); delay(2); digitalWrite(lm14, LOW);}
}

void laserOn(int laser) {
  digitalWrite(laser, HIGH);
}

void laserOff(int laser) {
  digitalWrite(laser, LOW);
}

void test() {
  digitalWrite(lm1, HIGH); delay(200); digitalWrite(lm1, LOW);
  digitalWrite(lm2, HIGH); delay(200); digitalWrite(lm2, LOW);
  digitalWrite(lm3, HIGH); delay(200); digitalWrite(lm3, LOW);
  digitalWrite(lm4, HIGH); delay(200); digitalWrite(lm4, LOW);
  digitalWrite(lm5, HIGH); delay(200); digitalWrite(lm5, LOW);
  digitalWrite(lm6, HIGH); delay(200); digitalWrite(lm6, LOW);
  digitalWrite(lm7, HIGH); delay(200); digitalWrite(lm7, LOW);
  digitalWrite(lm8, HIGH); delay(200); digitalWrite(lm8, LOW);
  digitalWrite(lm9, HIGH); delay(200); digitalWrite(lm9, LOW);
  digitalWrite(lm10, HIGH); delay(200); digitalWrite(lm10, LOW);
  digitalWrite(lm11, HIGH); delay(200); digitalWrite(lm11, LOW);
  digitalWrite(lm12, HIGH); delay(200); digitalWrite(lm12, LOW);
  digitalWrite(lm13, HIGH); delay(200); digitalWrite(lm13, LOW);
  digitalWrite(lm14, HIGH); delay(200); digitalWrite(lm14, LOW);
}

void setup() {
//  attachInterrupt(0, switchMode, RISING);
  pinMode(lm1, OUTPUT);
  pinMode(lm2, OUTPUT);
  pinMode(lm3, OUTPUT);
  pinMode(lm4, OUTPUT);
  pinMode(lm5, OUTPUT);
  pinMode(lm6, OUTPUT);
  pinMode(lm7, OUTPUT);
  pinMode(lm8, OUTPUT);
  pinMode(lm9, OUTPUT);
  pinMode(lm10, OUTPUT);
  pinMode(lm11, OUTPUT);
  pinMode(lm12, OUTPUT);
  pinMode(lm13, OUTPUT);
  pinMode(lm14, OUTPUT);
  test();
}

void loop() {
  laserOn(lm1);
  laserOn(lm2);
  laserOn(lm3);
  laserOn(lm4);
  laserOn(lm5);
  laserOn(lm6);
  laserOn(lm7);
  laserOn(lm8);
  laserOn(lm9);
  laserOn(lm10);
  laserOn(lm11);
  laserOn(lm12);
  laserOn(lm13);
  laserOn(lm14);
}


