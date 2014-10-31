#define MOT1 8
#define MOT2 9
#define LED 13

#define SWITCH A2

int buttonStateGo = HIGH;
int lastButtonStateGo = HIGH;
long lastDebounceTimeGo = 0;
long debounceDelayGo = 50;

int state = 0;
int stateChanged = false;

void setup() {
  Serial.begin(57600);
  pinMode(SWITCH, INPUT_PULLUP);
  pinMode(MOT1, OUTPUT);
  pinMode(MOT2, OUTPUT);
  pinMode(LED, OUTPUT);
  
  digitalWrite(LED, HIGH);
  delay(1000);
  digitalWrite(LED, LOW);
  digitalWrite(MOT1, LOW);
  digitalWrite(MOT2, LOW);
}

void loop() {
  
  if(goButtonPressed()) {
    state = (state+1)%4;
    stateChanged = true;
    Serial.println(state);
  }
  
  if(stateChanged) {
    if(state == 0 || state == 2) {motorStop();}
    else if(state == 1) {motorForward();}
    else if(state == 3) {motorReverse();}
    stateChanged = false;
  }
  
}

void motorStop() {
  digitalWrite(LED, LOW);
  digitalWrite(MOT1, LOW);
  digitalWrite(MOT2, LOW);
}

void motorForward() {
  digitalWrite(LED, HIGH);
  digitalWrite(MOT1, HIGH);
  digitalWrite(MOT2, LOW);
}

void motorReverse() {
  digitalWrite(LED, HIGH);
  digitalWrite(MOT1, LOW);
  digitalWrite(MOT2, HIGH);
}

boolean goButtonPressed(){
  int reading = digitalRead(SWITCH);
  boolean pressed = false;
  if (reading != lastButtonStateGo) {
    lastDebounceTimeGo = millis();
  } 
  
  if ((millis() - lastDebounceTimeGo) > debounceDelayGo) {
    if (reading != buttonStateGo) {
      buttonStateGo = reading;
      if (buttonStateGo == LOW) {
        pressed = true;
      }
    }
  }
  lastButtonStateGo = reading;
  return pressed;
}


