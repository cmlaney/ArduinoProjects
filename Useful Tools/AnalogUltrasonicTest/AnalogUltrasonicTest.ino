/*
Prints the value of the analog ultrasonic sensor
to the serial port
*/

#define ULTRA A0

void setup() {
  Serial.begin(9600);
  pinMode(A0,INPUT);
}

void loop() {
 Serial.print("Ping: ");
 Serial.println(ULTRA); 
}
