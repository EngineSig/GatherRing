#include <VirtualWire.h>
#include <string.h>
#include <ComputerCommand.h>
#include <EEPROM.h>
//TBD >>>> to search for pseudocode to complete
//byte message[VW_MAX_MESSAGE_LEN]; 
//byte messageLength = VW_MAX_MESSAGE_LEN;
//char stateString[14] ;
//char charArrayMessage[VW_MAX_MESSAGE_LEN];
//int waitingForACK[20];
//
////Computer Command Variables
//char serInString[30];  //Stores Computer Serial Terminal Messages
//String inputString;
//int char_num =0;
//boolean stringComplete = false;
//
//// Char arrays to process message
//char messageBody[VW_MAX_MESSAGE_LEN];
//char ackBraceletID[VW_MAX_MESSAGE_LEN];
//char* braceletsPlayed[5];
//
//// Keep time for timeout
//int time_start = 0;
//int time_last = 0;
//
////BraceletState
//int braceletState = 0;
//
////EEPROM Memory for storing Bracelet values
//int EEPROM_BraceletID=100; //location on EEPROM
//int EEPROM_PlayedNum = 101;//location on EEPROM
////EEPROM 0~X stores the bracelet IDs of Bracelet Numbers
//
//// Transmitter
//int potpin0 = 0;
//int potpin1 = 1;
//int val, val2;
//
//int led_transmitter_pin = 9;
//int led_handshake = 3;
////Debug Statements
//boolean transmitter_debug =false;
//boolean receiver_toggle = true;



void setup1(){
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
}

void loop1(){

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
  
  if(transmitter_debug){
    Serial.print("Pot 0: ");
    Serial.println(val);
    Serial.print("Pot 1: ");
    Serial.println(val2);  
  }
  if(val > 1000){
    //send("[CM][RESET]"); //your message  
    braceletState =1;
//    broadcast();
   // digitalWrite(led_handshake, HIGH);
  }else if (val <100){
   // send("RESET Complete"); 
    braceletState =0;
   // digitalWrite(led_handshake, LOW);
  }

/********Transmitter Code********/  
  
//############################## Add Gesture Listener  
//TBD
// Listen for Accelerometer Values
// Check to see if gesture is activated
// If Gesture turn_on -> Set to bracelet_state = 1
// If Gesture turn_off -> Set to bracelet_state =0
//############################## Add Gesture Listener

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!! Handshake Code
//TBD 
// If bracelet_state = 1
// Broadcast [P][BraceletID]
if(braceletState == 1 && random(2000)==1){
  broadcast();
}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!! Handshake Code

  
/********Watch Time *************/
//  if(millis()- time_start < 5000 && braceletState ==1 && millis()-time_last >1000){
//    Serial.println(">>>>>Broadcasting");      
//    //broadcast();    
//    time_last = millis();
//  }else if (millis()- time_start > 5000 && braceletState ==1){
//    Serial.println("Done Broadcasting"); 
//    braceletState =0; 
//  }
/********Watch Time *************/
   
  /********Receiver Code********/
}

//** GET BRACELETID **/
int getBraceletID(){
 return  EEPROM.read(EEPROM_BraceletID);
}

//** Broadcast Bracelet **/
void broadcast(){
    char confirmMessage[VW_MAX_MESSAGE_LEN];  
    memset(confirmMessage,0,sizeof(confirmMessage));
    int bracelet_num=EEPROM.read(EEPROM_BraceletID);
    char digit[3];
    itoa(bracelet_num, digit, 10);
    strcat(confirmMessage,"[P][");   
    strcat(confirmMessage,digit);
    strcat(confirmMessage,"]");
    strcat(confirmMessage,'\0');
   // Serial.println(confirmMessage);
   send(confirmMessage);   
}

/** Function for blinking **/
void blink(int led_pin, int counter){
 for(int index = 0; index<counter; index++){
  digitalWrite(led_transmitter_pin, HIGH); // Flash a light to show transmitting  
  delay(500);
  digitalWrite(led_transmitter_pin, LOW);
  delay(500);
 }   
}

/** Function for blinking **/

/** Function to Transmit via Radio **/
void send (char *message){
  vw_send((uint8_t *)message, strlen(message));
  digitalWrite(led_transmitter_pin, HIGH); // Flash a light to show transmitting
  vw_wait_tx(); 
  digitalWrite(led_transmitter_pin, LOW);
  delay(200);
}
/** Function to Transmit via Radio **/


/** Function to Help with Char Array manipulation **/
char* charArrayHelper(char* input){
  char confirmMessage[VW_MAX_MESSAGE_LEN];
  memset(confirmMessage,0,sizeof(confirmMessage));
  strcat(confirmMessage,"SET success ");
  return confirmMessage;
}
/** Function to Help with Char Array manipulation **/

/** Function to Process Commands from Computer **/
void command(char* command, char* message, int length){
  char confirmMessage[VW_MAX_MESSAGE_LEN];
  memset(confirmMessage,0,sizeof(confirmMessage));
  if(strcmp(command, "[TR]")==0){    
//    strcat(confirmMessage,"[ACKTR]");
    strcat(confirmMessage,message);
    strcat(confirmMessage,'\0');
    Serial.print("Echo string: ");
    Serial.println(confirmMessage);
    send(confirmMessage);
  } 
  if(strcmp(command, "[SET]")==0){
    char digit[3];
    digit[2]='\0';
    digit[1]=message[2];
    digit[0]=message[1];    
    strcat(confirmMessage,"SET success ");
    strcat(confirmMessage,message);
    strcat(confirmMessage,'\0');
    Serial.println(confirmMessage);
    EEPROM.write(EEPROM_BraceletID, atoi(digit));
    send(confirmMessage);
  }

  if(strcmp(command, "[RESET]")==0){
    EEPROM.write(EEPROM_PlayedNum, 0);
    Serial.println("RESET Complete");
    send("RESET Complete");
  } 
  if(strcmp(command, "[PROFILE]")==0){
    int bracelet_num=EEPROM.read(EEPROM_BraceletID);
    char digit[3];
    itoa(bracelet_num, digit, 10);
    strcat(confirmMessage,"[BRACELETID][");   
    strcat(confirmMessage,digit);
    strcat(confirmMessage,"]");
    strcat(confirmMessage,'\0');
    Serial.println(confirmMessage);
    send(confirmMessage);
  }

  if(strcmp(command, "[DATA]")==0){
    char digit[3];    
    int bracelet_totals = EEPROM.read(EEPROM_PlayedNum);
    int bracelet_num=EEPROM.read(EEPROM_BraceletID);        
    Serial.print("bracelet total: ");
    Serial.println(bracelet_totals);    
   
    if (bracelet_totals ==0){
     send("Bracelet has played 0 times"); 
    }
    for(int counter = 0; counter < bracelet_totals; counter ++){
      strcat(confirmMessage,"Played: #");    
      itoa(bracelet_num, digit, 10);    
      strcat(confirmMessage,digit);
      strcat(confirmMessage," with #");                 
      itoa(EEPROM.read(counter),digit, 10);
      strcat(confirmMessage,digit);      
      Serial.println(confirmMessage);   
      send(confirmMessage);            
      confirmMessage[0]='\0';
      delay(1500);
    }
  }   
} 

/** Function to Process Commands from Computer **/

/** Function to store Bracelet publicizing ID **/
void storeACK(char* self, char* sender){
    int self_id, sender_id;
    int bracelet_num=EEPROM.read(EEPROM_BraceletID);
    int bracelet_totals = EEPROM.read(EEPROM_PlayedNum);    
    char digit[3];
    digit[2]='\0';
    digit[1]=self[2];
    digit[0]=self[1]; 
    self_id = atoi(digit);
    
    digit[1] = sender[2];
    digit[0] = sender[1];
    sender_id = atoi(digit);
    Serial.print("Checking [ACK]");    
    Serial.print(" Self #");        
    Serial.println(bracelet_num);
    Serial.println(self_id);    
    if(bracelet_num == self_id && checkACK(sender_id)){
      Serial.println("Storing Values");
      Serial.println(sender_id);   
      Serial.println(bracelet_totals);
      EEPROM.write(bracelet_totals,sender_id);
      EEPROM.write(EEPROM_PlayedNum, bracelet_totals+1);
      Serial.println(EEPROM.read(EEPROM_PlayedNum));    
      send("Values Stored!");
    }
   braceletState =2;    
   stateBuddyFound();
   gestureCheck = true;
//    blink(led_transmitter_pin, 1);
}
/** Function to store Bracelet publicizing ID **/

/** Function parses Messages of format [STATE][MESSAGE] **/
int parseMessage(char* message_full, char* messageBody_parse, int length){
  int counter =0, messageBody_counter=0;
//  memset(stateString, '\0', 12);

  for(counter =0; counter < length; counter++){
   // Serial.print("Parsing Char: ");
   // Serial.println(message_full[counter]);       
    stateString[counter]=message_full[counter];
    if(message_full[counter]==']'){
       break;
     }
  }
  stateString[counter+1]='\0';
  counter++;
  Serial.println();
  for(; counter < length; counter++){
//    Serial.print("Parsing Body: ");
//    Serial.println(message_full[counter]);       
    messageBody_parse[messageBody_counter]=message_full[counter];
    messageBody_counter++;
  }
  messageBody_parse[messageBody_counter]='\0';

  Serial.print("Parsed State: ");
  Serial.println(stateString);
  Serial.print("Parsed Body: ");
  Serial.println(messageBody_parse);
  //printCharArray(stateString,12);
}

boolean checkACK(int value){
  boolean state = true;
  int bracelet_totals = EEPROM.read(EEPROM_PlayedNum);   
  for(int counter = 0; counter < bracelet_totals; counter ++){
//    Serial.print("Stored Value: ");
//    Serial.print(EEPROM.read(counter));
//    Serial.print(" checking with:");
//    Serial.println(value);
    if(value == EEPROM.read(counter)){
     //Serial.println("ACK already there");
     state = false; 
    }    
  }
  return state;
}

void checkSelf(){
    char confirmMessage[VW_MAX_MESSAGE_LEN];
    memset(confirmMessage,0,sizeof(confirmMessage));
    char digit[3];    
    int bracelet_totals = EEPROM.read(EEPROM_PlayedNum);
    int bracelet_num=EEPROM.read(EEPROM_BraceletID);        
    Serial.print("bracelet total: ");
    Serial.println(bracelet_totals);    
   
    if (bracelet_totals ==0){
//     send("Bracelet has played 0 times"); 
     Serial.println("Bracelet has played 0 times"); 
    }
    for(int counter = 0; counter < bracelet_totals; counter ++){
      strcat(confirmMessage,"Played: #");    
      itoa(bracelet_num, digit, 10);    
      strcat(confirmMessage,digit);
      strcat(confirmMessage," with #");                 
      itoa(EEPROM.read(counter),digit, 10);
      strcat(confirmMessage,digit);      
      Serial.println(confirmMessage);   
      send(confirmMessage);            
      confirmMessage[0]='\0';
      delay(1500);
    }
}

