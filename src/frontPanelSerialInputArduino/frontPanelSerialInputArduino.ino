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
 String artist, title, combined;
 char character;
 int elapsedTime = 0;
 int totalTime = 0;
 int state = 0;
 int kbit_rate = 0;
 int currentPositionInPlaylist = 0;
 int totalCountofSongsInPlaylist = 0;
 int volume =0;
 boolean contentChanged = false, combinedChanged;
 unsigned long time, idleTime;
 
 boolean consume,single, randomMode = false;
 LiquidCrystal_I2C lcd(0x27,20,4);
 boolean line_read_complete = false;
 void setup() { 
   time = millis();
   idleTime = millis();
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
   
   if (line_read_complete & countSeperator()==11 & transmitter != ";;;;;;;;;;;\n") {
     //lcd.clear();
    if(transmitter.indexOf(";")>0 & transmitter.substring(0,transmitter.indexOf(";")) != artist){
      artist = transmitter.substring(0,transmitter.indexOf(";"));
      contentChanged = true;
      combinedChanged = true;
      nextValue();
    } else {
      nextValue();
    }

    if(transmitter.indexOf(";")>0 & transmitter.substring(0,transmitter.indexOf(";")) != title){
      title = transmitter.substring(0,transmitter.indexOf(";"));
      contentChanged = true;
      combinedChanged = true;
      nextValue();
    } else {
      nextValue();
    }

      
    if(transmitter.indexOf(";")>0 & transmitter.substring(0,transmitter.indexOf(";")).toInt() != elapsedTime){
      elapsedTime = transmitter.substring(0,transmitter.indexOf(";")).toInt();
      contentChanged = true;
      nextValue();
    } else {
      nextValue();
    }
    
    if(transmitter.indexOf(";")>0 & transmitter.substring(0,transmitter.indexOf(";")).toInt() != totalTime){
       totalTime =  transmitter.substring(0,transmitter.indexOf(";")).toInt();
       contentChanged = true;
       nextValue();
    } else {
       nextValue();
    }
    
     if(transmitter.indexOf(";")>0 & toBoolean(transmitter.substring(0,transmitter.indexOf(";"))) != consume){
      consume = toBoolean(transmitter.substring(0,transmitter.indexOf(";")));
      contentChanged = true;
      nextValue();
    } else {
      nextValue();
    }
    
        if(transmitter.indexOf(";")>0 & toBoolean(transmitter.substring(0,transmitter.indexOf(";"))) != randomMode){
      randomMode = toBoolean(transmitter.substring(0,transmitter.indexOf(";")));
      contentChanged = true;
      nextValue();
    } else {
      nextValue();
    }
    
    
     if(transmitter.indexOf(";")>0 & toBoolean(transmitter.substring(0,transmitter.indexOf(";"))) != single){
      single = toBoolean(transmitter.substring(0,transmitter.indexOf(";")));
      contentChanged = true;
      nextValue();
    } else {
      nextValue();
    }
    
    if(transmitter.indexOf(";")>0 & transmitter.substring(0,transmitter.indexOf(";")).toInt() != state){
      state = transmitter.substring(0,transmitter.indexOf(";")).toInt();
      contentChanged = true;
      nextValue();
    } else {
      nextValue(); 
    }
    
    if(transmitter.indexOf(";")>0 & transmitter.substring(0,transmitter.indexOf(";")).toInt() != kbit_rate){
      kbit_rate = transmitter.substring(0,transmitter.indexOf(";")).toInt();
      contentChanged = true;
      nextValue();
    } else {
      nextValue(); 
    }
    
     if(transmitter.indexOf(";")>0 & transmitter.substring(0,transmitter.indexOf(";")).toInt() != currentPositionInPlaylist){
      currentPositionInPlaylist = transmitter.substring(0,transmitter.indexOf(";")).toInt();
      contentChanged = true;
      nextValue();
    } else {
      nextValue(); 
    }
    
        if(transmitter.indexOf(";")>0 & transmitter.substring(0,transmitter.indexOf(";")).toInt() != totalCountofSongsInPlaylist){
      totalCountofSongsInPlaylist = transmitter.substring(0,transmitter.indexOf(";")).toInt();
      contentChanged = true;
      nextValue();
    } else {
      nextValue(); 
    }
    
        if(transmitter.length()>1 & transmitter.toInt() != volume){
      volume = transmitter.toInt();
      contentChanged = true;
      //nextValue();
      transmitter = "";
    } else {
      transmitter = "";
      //nextValue(); 
    }
    
    /*
    Serial.println(artist + " - " + title);
    Serial.print(elapsedTime);Serial.print("/");Serial.println(totalTime);
    if(consume)Serial.println("Consume"); else Serial.println( "---");
    if(randomMode)Serial.println("Random"); else Serial.println("---");
    if(single)Serial.println("Single"); else Serial.println("---");
    if(state == 0) Serial.println("Playing"); else if(state == 1) Serial.println("Pause"); else Serial.println("Stop");
    Serial.print(kbit_rate);Serial.println(" kbit/s");
    Serial.print("Track:");Serial.print(currentPositionInPlaylist);Serial.print("/");Serial.println(totalCountofSongsInPlaylist);
    Serial.print("Volume:");Serial.println(volume); 
   */
   if(contentChanged & boot){
      lcd.display();
      lcd.backlight();
      lcd.clear();
      idleTime=millis();

      //lcd.print((artist + "/" + title).substring(0,19)  );
      lcd.setCursor(0,1);

      lcd.print(elapsedTime/60);lcd.print(":");lcd.print(elapsedTime%60);lcd.print("/");lcd.print(totalTime/60);lcd.print(":");lcd.print(totalTime%60);
      
      lcd.setCursor(15,1); 
      if(state == 2) lcd.print("Play"); else if(state == 1) lcd.print("Pause"); else lcd.print("Stop");
      lcd.setCursor(0,2);
      if(consume)lcd.print("CSM"); else lcd.print( "---");
      lcd.setCursor(3,2);
      if(randomMode)lcd.print("RND"); else lcd.print("---");
      lcd.setCursor(6,2);
      if(single)lcd.print("SNG"); else lcd.print("---");
      lcd.setCursor(16,2); lcd.print("kbps");
      if(kbit_rate <1000) lcd.setCursor(13,2); else lcd.setCursor(12,2);
      lcd.print(kbit_rate);
      lcd.setCursor(13,3);
      lcd.print("Vol:"); 
      if(volume==100)lcd.setCursor(17,3); else if(volume <10) lcd.setCursor(19,3); else lcd.setCursor(18,3);
      lcd.print(volume);
      
      lcd.setCursor(0,3);
      lcd.print("Title:");lcd.print(currentPositionInPlaylist);lcd.print("/");lcd.print(totalCountofSongsInPlaylist);
      contentChanged == false;
   }
     
   
     if(combinedChanged){
       combined = artist + "/" + title + " ";
       if(combined.length()<20){
         for(int i=combined.length();i<=20;i++){
          combined = combined + " "; 
         }
       }
       combinedChanged = false;
     }
     

  
      
 

     
    line_read_complete=false;  
   } else if(line_read_complete & countSeperator() != 11 & boot) {
     transmitter = "";
     line_read_complete=false;
   }  
      if(millis() - idleTime < 20000 &boot) { // 20000 for 20sec//300000ms for 5minutes to blank display
        if(millis()-time >= 200 &boot){
          lcd.display();
          lcd.backlight();
          lcd.setCursor(0,0);
          lcd.print("                  ");
          lcd.setCursor(0,0);
          lcd.print(combined.substring(0,20));
          shiftLeft();
          time = millis();
        }
      } else if(boot){
        lcd.clear();
        lcd.noBacklight();
        lcd.noDisplay();
      }
      /*
      lcd.setCursor(0,0);
      lcd.print("                  ");
      lcd.setCursor(0,0);
      lcd.print(combined.substring(0,20));
      //shiftLeft(); 
      */ 
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
      /*
      int j=0;
      for(int i=0; i<transmitter.length();i++){
        if(transmitter[i]==';'){j++;}
      }
      if(j>=11){line_read_complete = true;}
      */
      if(character == '\n'){
        line_read_complete = true;
        //Serial.println("Line complete");
      }
   }
   
 }
 
 void nextValue(){
   transmitter = transmitter.substring(transmitter.indexOf(";")+1,transmitter.length());
 }
 
boolean toBoolean(String value){
  if(value.toInt() == 0){
    return false;
  } else {
    return true;
  }
}

int countSeperator(){
  int seperator=0;
  for(int i=0; i<transmitter.length();i++){
    if(transmitter[i]==';'){seperator++;}
  }
  return seperator;
}

void shiftLeft(){
  if(combined.length()>0)
  {
  char tmp;
  tmp=combined[0]; //last character
  for(int i=0;i<combined.length()-1;i++){
    combined[i]=combined[i+1];
    //Serial.print(combined[i]);
   }
  combined[combined.length()-1]=tmp;
  }
}
