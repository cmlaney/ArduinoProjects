#define R 10
#define L 11

void setup() {
  pinMode(R, OUTPUT);
  pinMode(L, OUTPUT);
  Serial.begin(57600);  
}

void loop() {
  digitalWrite(R, HIGH);
  Serial.println("Blink.");
  delay(100);
  digitalWrite(R, LOW);
  delay(400);
  
  digitalWrite(L, HIGH);
  Serial.println("Blink.");
  delay(100);
  digitalWrite(L, LOW);
  delay(400);
}
