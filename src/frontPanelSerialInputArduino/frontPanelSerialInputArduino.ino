 int val; 
 int encoder0PinA = 2;
 int encoder0PinB = 3;
 int encoder0PinALast = LOW;
 int buttonPin = 4;
 int buttonPinLast = HIGH;
 
 int encoder1PinA = 5;
 int encoder1PinB = 6;
 int encoder1PinALast = LOW;
 int button1Pin = 7;
 int button1PinLast = HIGH;
 int n = LOW;

 void setup() { 
   pinMode(encoder0PinA,INPUT);
   digitalWrite(encoder0PinA, HIGH);
   pinMode(encoder0PinB,INPUT);
   digitalWrite(encoder0PinB, HIGH);
   pinMode(buttonPin, INPUT);
   digitalWrite(buttonPin, HIGH);
   
   pinMode(encoder1PinA,INPUT);
   digitalWrite(encoder1PinA, HIGH);
   pinMode(encoder1PinB,INPUT);
   digitalWrite(encoder1PinB, HIGH);
   pinMode(button1Pin, INPUT);
   digitalWrite(button1Pin, HIGH);
   Serial.begin (9600);
 } 

 void loop() { 
   n = digitalRead(encoder0PinA);
   if ((encoder0PinALast == LOW) && (n == HIGH)) {
     if (digitalRead(encoder0PinB) == LOW) {
       Serial.println("1");
     } else {
       Serial.println("2");
     }
   } 
   encoder0PinALast = n;
   
   n = digitalRead(buttonPin);
   if ((n == LOW) && (buttonPinLast == HIGH)){
     Serial.println("3");
   }
   buttonPinLast = n;
   
   n = digitalRead(encoder1PinA);
   if ((encoder1PinALast == LOW) && (n == HIGH)) {
     if (digitalRead(encoder1PinB) == LOW) {
       Serial.println("4");
     } else {
       Serial.println("5");
     }
   } 
   encoder1PinALast = n;
   
   n = digitalRead(button1Pin);
   if ((n == LOW) && (button1PinLast == HIGH)){
     Serial.println("6");
   }
   button1PinLast = n;
   
 } 
