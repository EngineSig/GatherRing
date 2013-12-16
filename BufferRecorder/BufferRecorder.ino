int potPin = 0;     // set pin for potentiometer

int recordData[20]; // Array will store key input presses with 1 equal pressed and 0 equal not pressed
char serInString[10]; 
int value;
void setup() {
   //pinMode(ledPin, OUTPUT); 
   Serial.begin(9600);
   Serial.println("ready");
}

void loop() {

   readSerialString(serInString); 
   //Serial.println(analogRead(potPin));    
   if(serInString[0] =='r'){
     Serial.println("RECORD in 3 Sec");
     Serial.println("3");
     delay(1000);
     Serial.println("2");
     delay(1000);
     Serial.println("1");
     delay(1000);
     for(int counter = 0; counter < 20; counter++){
       value = analogRead(potPin);
       recordData[counter] = value;
       Serial.println(value);
       delay(100);     
       }
   Serial.println("RECORD DONE");
   serInString[0] ='a';
   }
   if(serInString[0] =='p'){
     Serial.println("PRINT");     
       for(int counter1 = 0; counter1 < 20; counter1++){
         Serial.print('[');
         Serial.print(counter1);
         Serial.print("][");
         Serial.print(String(recordData[counter1]));
         Serial.println(']');
       }
   Serial.print("PRINT DONE");    
   serInString[0] ='a';
   }
   
   delay(1000);
}


//read a string from the serial and store it in an array
//you must supply the array variable
void readSerialString (char *strArray) {
  int i = 0;
  if(!Serial.available()) {
    return;
  }
  while (Serial.available()) {
    strArray[i] = Serial.read();
    i++;
  }
}
