#include <avr/io.h>

//Board LED is pin 13
#define LED_PIN 11

//Pin the button is on
#define BUTTON 3

/**
 * Setups up the timer interrupt to happen every 100 milliseconds.
 **/
void setupTimerInterrupt(){
   //Reset to defaults
  TCCR1A = TCCR1B = 0;
  // CTC with OCR1A as top
  TCCR1B |= (1 << WGM12);
  //Set clock to clk/1024
  TCCR1B |= (1 << CS12) | (1 << CS10);
  //Enable interrupt on match with OCR1A
  TIMSK1 = (1 << OCIE1A);
  
  //LAB CODE -- FILL IN
  //Configure OCR1A (the counter top) to generate an interrupt
  //every 500ms.
  OCR1A = 7800;
}

void toggleLED(){
  static uint8_t on = 0;
  if(on){
    digitalWrite(LED_PIN,HIGH);
  }else{
    digitalWrite(LED_PIN,LOW);
  }
  on = !on;
}

/**
 * This function is called whenever the timer interrupt match 1A
 * is generated.
 **/
SIGNAL(TIMER1_COMPA_vect){
  //LAB CODE
  //UNCOMMENT THIS LINE TO TOGGLE ON INTERRUPT
  toggleLED();
}


void setup() {
  Serial.begin(9600);
  Serial.print("Starting up...");

  //Setup the LED
  pinMode(LED_PIN,OUTPUT);
  digitalWrite(LED_PIN,LOW);
  
  //Setup the button
  pinMode(BUTTON,INPUT);
  //Pullup
  digitalWrite(BUTTON,HIGH);
  
  Serial.println("startup complete.");
  setupTimerInterrupt();  
}

void loop() {
  static unsigned long lastTimeMS = 0;
  unsigned long currentTimeMS = 0;
  //Manually toggle the LED every 2 seconds
  //Comment this out when enabling the interrupt version
//  currentTimeMS = millis();
//  if((currentTimeMS - lastTimeMS) >= 500){
//    toggleLED();
//    lastTimeMS = currentTimeMS;
//  }
  
  //If the button is pressed, print the ADC values
  if(digitalRead(BUTTON) == LOW){
    Serial.println("--------------------------------------------");
    Serial.println("--------------------------------------------");
    Serial.println("--------------------------------------------");
    Serial.print("The reading of analog port A0 is value: "); Serial.println(analogRead(A0));
    Serial.print("The reading of analog port A1 is value: "); Serial.println(analogRead(A1));
    Serial.print("The reading of analog port A2 is value: "); Serial.println(analogRead(A2));
    Serial.print("The reading of analog port A3 is value: "); Serial.println(analogRead(A3));
    Serial.print("The reading of analog port A4 is value: "); Serial.println(analogRead(A4));
    Serial.print("The reading of analog port A5 is value: "); Serial.println(analogRead(A5));
    Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
    for(uint8_t i = 0; i < 20; i++){
        Serial.print("The reading of digital pin ");
        Serial.print(i);
        Serial.print(" has a value of: ");
        Serial.println(digitalRead(i));
    }

  }

}
