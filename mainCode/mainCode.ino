
#include <Wire.h>
#include <ADXL345.h>
#include <VirtualWire.h>
#include <string.h>
#include <computerCommand.h>
#include <radio.h>
#include <neoPixel.h>
#include <EEPROM.h>
#include <Adafruit_NeoPixel.h>



/*************** Variables for Radio ********/
//TBD >>>> to search for pseudocode to complete
byte message[VW_MAX_MESSAGE_LEN]; 
byte messageLength = VW_MAX_MESSAGE_LEN;
char stateString[14] ;
char charArrayMessage[VW_MAX_MESSAGE_LEN];
int waitingForACK[20];

//Computer Command Variables
char serInString[30];  //Stores Computer Serial Terminal Messages
String inputString;
int char_num =0;
boolean stringComplete = false;

// Char arrays to process message
char messageBody[VW_MAX_MESSAGE_LEN];
char ackBraceletID[VW_MAX_MESSAGE_LEN];
char* braceletsPlayed[5];

// Keep time for timeout
int time_start = 0;
int time_last = 0;
int broadcast_timer =0;

//BraceletState
int braceletState = 0;
int PIN = 2;
//EEPROM Memory for storing Bracelet values
int EEPROM_BraceletID=100; //location on EEPROM
int EEPROM_PlayedNum = 101;//location on EEPROM
//EEPROM 0~X stores the bracelet IDs of Bracelet Numbers

// Transmitter
int potpin0 = 0;
int potpin1 = 1;
int val, val2;

int led_transmitter_pin = 9;
int led_handshake = 3;
//Debug Statements
boolean transmitter_debug =false;
boolean receiver_toggle = true;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, PIN, NEO_GRB + NEO_KHZ800);
/*************** Variables for Radio ********/

/** Gesture Variables **/
const float alpha = 0.5;
int var1 = 0;
int var2 = 80;
int var3;
int var4; 

double fXg = 0;
double fYg = 0;
double fZg = 0;
ADXL345 acc;
boolean gestureCheck = true;
/** Gesture Variables **/

//int braceletState =0;

void setup()
{
/** Accelerometer  **/
	acc.begin();
        braceletState =0;
        gestureCheck = true;
/** Accelerometer **/
  Serial.begin(9600);
  Serial.println("Device is ready");
  //Receiver Setup
  vw_setup(2000); //bps
  vw_set_rx_pin(7);
  vw_rx_start(); 
  
  //Transmitter Setup
  vw_set_tx_pin(6);
  
  pinMode(13,OUTPUT);
  digitalWrite(13,LOW);
  //reserve space for serial string read
  inputString.reserve(200);
  
  if(EEPROM.read(EEPROM_BraceletID) == 255)
    EEPROM.write(EEPROM_BraceletID,10); //Every new bracelet is default #10

/**** show State **/
  strip.begin();
  stateIdle();

/**** show State **/  
}

void loop()
{
///////////////////////////////////////////////////////////////////////////////////////////////// Radio Commands/////////////////////////////////////////////////  
/********Computer Serial Command Code********/    
 int temp;
 temp = readSerialString(serInString); 
 if(serInString[0] =='%' && temp >0){
//   serial_chars= readSerialString(serInString); 
   //printCharArray(serInString,30);
   Serial.println(&serInString[1]);
  parseMessage(&serInString[1],messageBody, messageLength);


// Set Receive State
  if(strcmp(stateString, "[RX]") ==0){

    if(strcmp(messageBody,"[true]")==0){
      receiver_toggle = true;
      Serial.println("Turning ON Receiver");
    }else if(strcmp(messageBody,"[false]")==0){
      receiver_toggle = false;
      Serial.println("Turning OFF Receiver");
    }
  }
// Set Receive State
  if(strcmp(stateString, "[B]") ==0){
    if(strcmp(messageBody,"[true]")==0){
      braceletState = 1;
      Serial.println("Turning ON Bracelet");
    }else if(strcmp(messageBody,"[false]")==0){
      braceletState = 0;
      Serial.println("Turning OFF Bracelet");
      stateIdle();
    }
  }  
  
// Transmit Data from Console
  if(strcmp(stateString, "[TX]") ==0){
    Serial.println("Sending String");
    Serial.println(messageBody);
    send(messageBody);
  }
// Issue command directly to bracelet  
  if(strcmp(stateString,"[CM]")==0){
     parseMessage(messageBody,messageBody, messageLength);      
     char digit[3];
     digit[2]='\0';
     digit[1]=stateString[2];
     digit[0]=stateString[1];
     Serial.println(digit);
     if(strcmp(stateString, "[PROFILE]")==0){
       command(stateString, messageBody, messageLength);
     }else if(getBraceletID() == atoi(digit)){
       Serial.println("Setting Command"); 
       parseMessage(messageBody,messageBody, messageLength);                 
       command(stateString, messageBody, messageLength);
     }
  }
// Receive Bracelet Publicity ID
  if(strcmp(stateString,"[ACK]")==0){
     parseMessage(messageBody,messageBody, messageLength);      
     Serial.println("Receive Bracelet ACK"); 
     storeACK(stateString, messageBody);
  }
  if(strcmp(stateString,"[P]")==0){
     parseMessage(messageBody,messageBody, messageLength);      
     Serial.println("Receive Bracelet Publicize");      
     time_last= millis();
     time_start = millis();
//     braceletState = 1;
  }
  if(strcmp(stateString,"[X]")==0){
    braceletState=1;
    send("[QQ]");
  }
  
  if(strcmp(stateString,"[Z]")==0){
    braceletState=0;
    send("[OFF]");
    send("[OFF]");
    send("[OFF]");
  }
  if(strcmp(stateString,"[CLEAR]")==0){
    send("[CM][11][RESET]");    
    send("[CM][11][RESET]");    
    EEPROM.write(EEPROM_PlayedNum, 0);
  }

  if(strcmp(stateString,"[CHECK]")==0){
    checkSelf();
    send("[CM][11][DATA]!");
  }
  
 }
/********Computer Serial Command Code********/    
  
/********Transmitter Code********/   
  val = analogRead(potpin0);
//  val2 = analogRead(potpin1);
  
//  if(transmitter_debug){
//    Serial.print("Pot 0: ");
//    Serial.println(val);
//    Serial.print("Pot 1: ");
//    Serial.println(val2);  
//  }
//  if(val > 1000){
//    //send("[CM][RESET]"); //your message  
//    braceletState =1;
////    broadcast();
//   // digitalWrite(led_handshake, HIGH);
//  }else if (val <100){
//   // send("RESET Complete"); 
//    braceletState =0;
//   // digitalWrite(led_handshake, LOW);
//  }

/********Transmitter Code********/  

/********Receiver Code********/
  messageLength = VW_MAX_MESSAGE_LEN;
  if (vw_get_message(message, &messageLength)&& receiver_toggle ){
      Serial.print("Received: ");
      Serial.println(String(messageLength,DEC));
        for (int i = 0; i < messageLength; i++){
          Serial.write(message[i]);
          charArrayMessage[i]=message[i];
        }
      charArrayMessage[messageLength] = '\0';     
        
      Serial.println();
      
      //Serial.print("Compairing String: ");
      //Serial.println(charArrayMessage);
     
      parseMessage(charArrayMessage,messageBody, messageLength);      
      
// Trigger Action if receive special [CM] from computer
      if(strcmp(stateString,"[CM]")==0){
         parseMessage(messageBody,messageBody, messageLength);      
         char digit[3];
         digit[2]='\0';
         digit[1]=stateString[2];
         digit[0]=stateString[1];
         Serial.println(digit);
         if(strcmp(stateString, "[PROFILE]")==0){
           command(stateString, messageBody, messageLength);
         }else if(getBraceletID() == atoi(digit)){
           Serial.println("Setting Command"); 
           parseMessage(messageBody,messageBody, messageLength);                 
           command(stateString, messageBody, messageLength);
         }
      }
//TBD 
// Receive Bracelet Publicity [P} 
      if(strcmp(stateString,"[P]")==0){
//         parseMessage(messageBody,messageBody, messageLength);      
         Serial.println("Receive[P] Publicize"); 
         //TBD How do I distinguish between multiple [ACK] and not writing redundant values checker?
//         send("[RECEIVED][ACK]");
         for(int counter =0; counter<3;counter++){
           char sendMessage[VW_MAX_MESSAGE_LEN];
           memset(sendMessage,0,sizeof(sendMessage));
           strcat(sendMessage,"[ACK]");   
           strcat(sendMessage,messageBody);
           char digit[3];
           itoa(getBraceletID(), digit, 10);
           strcat(sendMessage,"["); 
           strcat(sendMessage,digit);
           strcat(sendMessage,"]");   
           //Serial.println(sendMessage);
//           blink(led_handshake,3);
           send(sendMessage);
         }
      }        
//TBD Send multiple [ACK] ??
// Test echo start broadcast
      if(strcmp(stateString,"[QQ]")==0){
         parseMessage(messageBody,messageBody, messageLength);      
         Serial.println("Setting Action!!!!!!!!!!"); 
         braceletState=1;
         //TBD How do I distinguish between multiple [ACK] and not writing redundant values checker?
      }      
      if(strcmp(stateString,"[OFF]")==0){
         parseMessage(messageBody,messageBody, messageLength);      
         Serial.println(">>>>>Turning Off"); 
         braceletState=0;
         //TBD How do I distinguish between multiple [ACK] and not writing redundant values checker?
      }


// Receive Bracelet ACK ID
      if(strcmp(stateString,"[ACK]")==0){
         parseMessage(messageBody,messageBody, messageLength);      
         Serial.println("Receive ACK Publicize"); 
         //TBD How do I distinguish between multiple [ACK] and not writing redundant values checker?
         storeACK(stateString, messageBody);
      }      
      Serial.println();
      //delay(500);
  }    
/********Receiver Code********/

///////////////////////////////////////////////////////////////////////////////////////////////// Radio Commands/////////////////////////////////////////////////  
///////////////////////////////////////////////////////////////////////////////////////////////Gesture/////////////////////////////////////////////////////////////  
	double pitch, roll, Xg, Yg, Zg;
 
/***************************** Check for Gesture Activation ********************/        
//             Serial.print("?????What is this value: ");
//             Serial.println(braceletState);     
//>>>>>> Measure Pitch 
	acc.read(&Xg, &Yg, &Zg);
	//Low Pass Filter
	fXg = Xg * alpha + (fXg * (1.0 - alpha));
	fYg = Yg * alpha + (fYg * (1.0 - alpha));
	fZg = Zg * alpha + (fZg * (1.0 - alpha));

	//Roll & Pitch Equations
	roll  = (atan2(-fYg, fZg)*180.0)/M_PI;
	pitch = (atan2(fXg, sqrt(fYg*fYg + fZg*fZg))*180.0)/M_PI;
        var3 = pitch;          
        if (gestureCheck==true && var3 <-70) {         
//        Serial.print("&&&&&&&&&&&&&&&&&&&&&&&&Loop");
//>>>>>> Measure Pitch

        delay (100);             
    //>>>>>> Measure Pitch
            acc.read(&Xg, &Yg, &Zg);
    	//Low Pass Filter
      	fXg = Xg * alpha + (fXg * (1.0 - alpha));
      	fYg = Yg * alpha + (fYg * (1.0 - alpha));
      	fZg = Zg * alpha + (fZg * (1.0 - alpha));
      
      	//Roll & Pitch Equations
      	roll  = (atan2(-fYg, fZg)*180.0)/M_PI;
      	pitch = (atan2(fXg, sqrt(fYg*fYg + fZg*fZg))*180.0)/M_PI;
    //>>>>>> Measure Pitch
//            Serial.print("Var4: ");
//            Serial.print( var4);
//            Serial.print( "    Var3: ");
//            Serial.println(var3);
            var4 = pitch;              
            while (var4 > var3 ) {            
      //        Serial.println("its in the loop");          
                var3 = var4;        
                acc.read(&Xg, &Yg, &Zg);
      //>>>>>> Measure Pitch
          	//Low Pass Filter
          	fXg = Xg * alpha + (fXg * (1.0 - alpha));
          	fYg = Yg * alpha + (fYg * (1.0 - alpha));
          	fZg = Zg * alpha + (fZg * (1.0 - alpha));    
          	//Roll & Pitch Equations
          	roll  = (atan2(-fYg, fZg)*180.0)/M_PI;
          	pitch = (atan2(fXg, sqrt(fYg*fYg + fZg*fZg))*180.0)/M_PI;
      //>>>>>> Measure Pitch  
            var4 = pitch;        
//             Serial.print(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>What is this value: ");
//             Serial.println(var4);        

             if (var4 < var1 + 20 && var4 > var1 - 20) {
                  Serial.println(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>Gesture Activation Triggered ");
                  delay(1000);            
                  gestureCheck = false;
                  stateLocked();
                  if(braceletState ==0){
                   braceletState =1; 
                  }else if(braceletState ==2){
                   braceletState =0;
                   stateIdle();
                   gestureCheck = true;
                  }
                  break; // break out of while
                }

          delay(10);
          }
        }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////        
        if(braceletState == 1 && random(1500)==1){
            broadcast();            
            Serial.println("Broadcast Activation State!!!!!!!!!!!!!!!!");                     
        }
        else {
//         Serial.println("nothing is happening.");          
        }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////        
//	delay(10);
}


