#include <Wire.h>
#include <LiquidCrystal_I2C.h>

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
 boolean boot = false;
 String transmitter = "";
 String artist, title;
 char character;
 LiquidCrystal_I2C lcd(0x27,20,4);
 boolean line_read_complete = false;
 void setup() { 
   lcd.init();
   lcd.backlight();
   lcd.setCursor(0,0);
   lcd.print("    OpenJukebox  ");
   lcd.setCursor(0,1);
   lcd.print("     booting...   ");
   lcd.setCursor(0,3);
   lcd.print("       V.0.0      ");
   
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
   
   if (line_read_complete) {
     Serial.println("line read complete");
     lcd.clear();
    if(transmitter.indexOf(";")>0 & transmitter.substring(0,transmitter.indexOf(";")) != artist){
      artist = transmitter.substring(0,transmitter.indexOf(";"));
      Serial.println("artist diferent");
      transmitter = transmitter.substring(transmitter.indexOf(";")+1,transmitter.length());
    } else {
      transmitter = transmitter.substring(transmitter.indexOf(";")+1,transmitter.length());
    }
    //Serial.println(transmitter);
    if(transmitter.indexOf(";")>0 & transmitter.substring(0,transmitter.indexOf(";")) != title){
      title = transmitter.substring(0,transmitter.indexOf(";"));
      transmitter = transmitter.substring(transmitter.indexOf(";")+1,transmitter.length());
    } else {
      transmitter = transmitter.substring(transmitter.indexOf(";")+1,transmitter.length());
    }
    lcd.setCursor(0,0);
      lcd.print(artist + "/" + title);
    transmitter="";
    line_read_complete=false;  
   }
   
 } 
 
 
 void serialEvent() {
   
      
   while (Serial.available() > 0) {
     if(!boot)
       {
         boot = true;
         lcd.clear();
       }
      character = (char)Serial.read();
      transmitter += character;
      //Serial.println("Char");
      if(character == '\n'){
        line_read_complete = true;
        //Serial.println("Line complete");
      }
   }
   
 }
