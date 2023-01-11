/* 
 * Pin layout used:
 * --------------------------------
 *             MFRC522      Arduino
 *             Reader/PCD   Uno/101
 * Signal      Pin          Pin
 * --------------------------------
 * RST/Reset   RST          9 
 * SPI SS      SDA(SS)      10
 * SPI MOSI    MOSI         11
 * SPI MISO    MISO         12
 * SPI SCK     SCK          13
 * GND/Ground  GND          GND
 * 3.3v        3.3v         3.3v
 * 
 * 
 * 
 * 
 * LED indicator colors
 * white = read
 * green flash = successful read/write
 * blue = write
 * red = idle
 */

//imports
#include <SoftwareSerial.h>
#include <SPI.h>
#include <MFRC522.h>

SoftwareSerial mySerial(6,5); //Sets up RX and TX ports
//byte buffer[34];
//byte len = 0;
int state =0;
int buttonState = 0;
bool serialNeedsClearing = true;
bool firstRFIDRead = true;
bool IDReadytoWrite = false;


#define RST_PIN         9
#define SS_PIN          10

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

MFRC522::MIFARE_Key key;


void setup() {
  Serial.begin(9600);
  Serial.setTimeout(1);
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(2, INPUT);
  mySerial.begin(9600);
  SPI.begin(); //Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card
  // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
}

void loop() {

  //Look for user input to change state
  if (Serial.available() && state != 3){
    String a=Serial.readStringUntil('\n');
    if(a=="k"){
      state = 1;
      RGB_color(0, 255, 0);
    }else if(a=="j"){
     state = 0;
     RGB_color(0, 0, 0);
    }else if(a=="b"){
     state = 2;
    }else if(a=="c"){
     state = 3;
     RGB_color(255, 0, 0);
     //Play tone
    tone(8, 500);
    delay(100);
    noTone(8);
    delay(80);
    tone(8, 500);
    delay(100);
    noTone(8);
    delay(80);
    tone(8, 500);
    delay(100);
    noTone(8);
    }else{
  
    }
  }

  //Serial.println(state);                //Debug for state code

  if(buttonState == LOW){
      while(mySerial.available()){
        char clr = mySerial.read();
      }
      serialNeedsClearing = false;
      //Serial.println("clearing");
    }

  buttonState = digitalRead(2);
  if(buttonState == HIGH){

  

  //Check state and execute
  if(state == 1){ //Read RFID

    RGB_color(0, 255, 0);
    
    serialNeedsClearing = true;

    //buttonState = digitalRead(2);
    delay(500);  
    
    if(mySerial.available() && buttonState == HIGH){
        while(mySerial.available()){
         char input = mySerial.read();
         Serial.write(input);
         delay(5);
        }
        RGB_color(0, 0, 0);
        tone(8, 1200);
        delay(100);
        RGB_color(0, 255, 0);
        noTone(8);
        
        Serial.write('\n');
      }
//  }else if(state == 2){ //Read Tag


    // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
    if ( ! mfrc522.PICC_IsNewCardPresent())
        return;

    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial())
        return;

    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);

    // Check for compatibility
    if (    piccType != MFRC522::PICC_TYPE_MIFARE_MINI
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_1K
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
//        Serial.println(F("This sample only works with MIFARE Classic cards."));
        return;
    }

    // In this sample we use the second sector,
    // that is: sector #1, covering block #4 up to and including block #7
    byte sector         = 1;
    byte blockAddr      = 4;
    byte dataBlock[]    = {
        0x01, 0x02, 0x03, 0x04, //  1,  2,   3,  4,
        0x05, 0x06, 0x07, 0x08, //  5,  6,   7,  8,
        0x09, 0x0a, 0xff, 0x0b, //  9, 10, 255, 11,
        0x0c, 0x0d, 0x0e, 0x0f  // 12, 13, 14, 15
    };
    byte trailerBlock   = 7;
    MFRC522::StatusCode status;
    byte buffer[18];
    byte size = sizeof(buffer);

    // Authenticate using key A
    //Serial.println(F("Authenticating using key A..."));
    status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }

    status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr, buffer, &size);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Read() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    }
    for (uint8_t i = 0; i < 12; i++) {
     Serial.write(buffer[i]);
    }

    for (uint8_t i = 0; i < 12; i++) {
     buffer[i]='0';
    }

    Serial.println();

    //Play tone
    RGB_color(0, 0, 0);
    tone(8, 1000);
    delay(100);
    RGB_color(0, 255, 0);
    noTone(8);

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();

  }    
  }else if(state == 3){ //Write Tag
  char done = ' ';
  byte len = 0;
  byte blockAddr      = 4;
  MFRC522::StatusCode status;
  byte buffer[34];
  Serial.setTimeout(1000L) ;     // wait until 4 seconds for input from serial 000000000001
   /* 
    //Play tone
    tone(8, 500);
    delay(100);
    noTone(8);
    delay(80);
    tone(8, 500);
    delay(100);
    noTone(8);
    delay(80);
    tone(8, 500);
    delay(100);
    noTone(8);
    */

    delay(500);

    len = Serial.readBytesUntil('\n', (char *) buffer, 30) ; // read number from serial
    //Serial.print(len);
    for (byte i = len; i < 30; i++) buffer[i] = ' ';     // pad with spaces

       buttonState = digitalRead(2);
 /*
   while(buttonState == LOW){
    buttonState = digitalRead(2);
     if (Serial.available()){
      String a=Serial.readStringUntil('\n');
        if(a=="j"){
          state = 0;
          RGB_color(0, 0, 0);
          return;
        }else{
  
        }
      }
   }
  */

  //while(buttonState == HIGH){
    
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  while ( ! mfrc522.PICC_IsNewCardPresent() || buttonState == LOW) {
    if (Serial.available()){
    String a=Serial.readStringUntil('\n');
    if(a=="k"){
      state = 1;
      RGB_color(0, 255, 0);
      return;
    }else if(a=="j"){
     state = 0;
     RGB_color(0, 0, 0);
     return;
    }else if(a=="b"){
     state = 2;
    }else if(a=="c"){
     state = 3;
     RGB_color(255, 0, 0);
     return;
    }else{
  
    }
  }
  buttonState = digitalRead(2);
    }
  // Select one of the cards
  while ( ! mfrc522.PICC_ReadCardSerial() || buttonState == LOW) {
    if (Serial.available()){
    String a=Serial.readStringUntil('\n');
    if(a=="k"){
      state = 1;
      RGB_color(0, 255, 0);
      return;
    }else if(a=="j"){
     state = 0;
     RGB_color(0, 0, 0);
     return;
    }else if(a=="b"){
     state = 2;
    }else if(a=="c"){
     state = 3;
     RGB_color(255, 0, 0);
     return;
    }else{
  
    }
  }
  buttonState = digitalRead(2);
    }
   

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 4, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    state = 0;
    RGB_color(0, 0, 0);
    done = 'f';
    Serial.println(done);
    //Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  
  // Write block
  status = mfrc522.MIFARE_Write(blockAddr, buffer, 16);
  if (status != MFRC522::STATUS_OK) {
    state = 0;
    RGB_color(0, 0, 0);
    done = 'f';
    Serial.println(done);
    //Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  //Serial.println("Done");
    //Play tone
    RGB_color(0, 0, 0);
    tone(8, 1200);
    delay(100);
    RGB_color(255, 0, 0);
    noTone(8);
    RGB_color(0, 0, 0);

  mfrc522.PICC_HaltA(); // Halt PICC
  mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD

  done = 'w';
  Serial.println(done);
  state = 0;
  return;

  //}
    
  }else{
  
    //digitalWrite(LED_BUILTIN,LOW);
    serialNeedsClearing = true;
  }

}

void RGB_color(int red_light_value, int green_light_value, int blue_light_value)
 {
  analogWrite(A0, red_light_value);
  analogWrite(A1, green_light_value);
  analogWrite(A2, blue_light_value);
}
