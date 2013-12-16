

//read a string from the serial and store it in an array
//you must supply the array variable
int readSerialString (char *strArray) {
//  memset(strArray, 0, sizeof(strArray));
  int i = 0;
//  if(!Serial.available()) {
//    memset(strArray, 0, sizeof(strArray));
//    return 0;
//  }else 
  if(Serial.available()>0){
    //Serial.print("Buffer Ready:");
    while (Serial.available()) {
      //Serial.print("Available:");
      //Serial.println(Serial.available());
      char input = Serial.read();      
      if(input =='!'){
//        Serial.println("!!!!!!!!!!!!");
//        Serial.println(strArray);
        char_num=0;
        return 1;
      }            
      strArray[char_num] = input;
//      Serial.print("Reading Char: ");
//      Serial.println(char_num);        
//      Serial.println(input);
      char_num++;
      strArray[char_num] ='\0';
    
    }
  }

  return 0;
}

void printCharArray(char* charArray, int length){
  Serial.print("CharArray>> ");
  for(int i = 0; i< length; i++){
   Serial.print('[');
   Serial.print(i);
   Serial.print(']'); 
   Serial.print('<');
   Serial.print(charArray[i]);
   Serial.println('>');    
  }  
}



