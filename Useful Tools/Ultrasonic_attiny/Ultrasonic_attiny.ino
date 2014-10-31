#include <avr/io.h>
#include <ShiftLCD.h>

const int trigPin = 4;
const int echoPin = 3;
long timer = 0;
long duration;
int inches;
ShiftLCD lcd(0,2,1);

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  lcd.begin(16,2);
  lcd.print("Distance");
}
 
void loop()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  
  inches = microsecondsToInches(duration);
  
  if(millis()-timer>500) {
    timer = millis();
    lcd.setCursor(0,1);
    lcd.print("                ");
    if(inches>145) {
      lcd.setCursor(0,1);
      lcd.print("Too far!");
    } else if (inches==0) {
      lcd.setCursor(0,1);
      lcd.print("No reading");
    } else {
      inches += 1;
      printDistance(inches);
    }
  }
  delay(100);
}
 
int microsecondsToInches(long microseconds)
{
  return microseconds / 74 / 2;
}

int printDistance(int i) {
   int feet = 0;
  if(i>12) {
     feet = i/12;
     i = i % 12;
  }
    
    lcd.setCursor(0,1);
  if(feet == 0) {  
    lcd.print(i);
    lcd.print(" inches");
  } else {
    lcd.print(feet);
    lcd.print(" ft, ");
    lcd.print(i);
    lcd.print(" in");
  } 
}


