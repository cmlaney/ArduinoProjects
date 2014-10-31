/*
CSCE 236 Lab 5: Button Interrupts
Christian Laney
*/

#define red 12
#define green 11
#define blue 10
#define myButton 3

boolean method1 = true  ;

void setup() {
 Serial.begin(9600);
 pinMode(red, OUTPUT);
 pinMode(green, OUTPUT);
 pinMode(blue, OUTPUT);
 pinMode(myButton, INPUT_PULLUP);
 digitalWrite(red, HIGH);
 delay(500);
 digitalWrite(red,LOW);
 digitalWrite(green, HIGH);
 delay(500);
 digitalWrite(green,LOW);
 digitalWrite(blue, HIGH);
 delay(500);
 digitalWrite(blue,LOW);
 EICRA = 0;
 if(method1) {
   EICRA |= (1<<ISC11);
 } else {
   EICRA |= (1<<ISC10);
 }
 EIMSK = 2; 
}

SIGNAL(INT1_vect) {
  digitalWrite(green,HIGH);
}

void loop() {
  if(digitalRead(myButton)==HIGH) {
      if(method1){cli();}
    digitalWrite(green,LOW);
        if(method1){sei();}

  }
}
