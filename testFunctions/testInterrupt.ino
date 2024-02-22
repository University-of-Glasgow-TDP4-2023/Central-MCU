const int ledPin = 3; // Change this to the desired pin number
const int interruptPin = 6;  // Change this to the desired pin number

void setup() {
  pinMode(ledPin, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin), interruptHandler, LOW); 
}

void loop() {
  digitalWrite(ledPin, HIGH);
}


void interruptHandler() {
  digitalWrite(ledPin, LOW);
}
