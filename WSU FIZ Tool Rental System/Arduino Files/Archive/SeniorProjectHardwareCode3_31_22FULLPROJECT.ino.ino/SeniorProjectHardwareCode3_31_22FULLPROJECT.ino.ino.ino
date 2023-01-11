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
 * Selectors:
 * 0 = Idle mode
 * 1 = RFID read
 * 2 = RFID write
 * 3 = Barcode read
 * 
 * 003300002222
 * 220000000987
 * 
 * Barcode Scanner Connected to Pins 5 and 6
*/

#include <SPI.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>

#define RST_PIN         9           // Configurable, see typical pin layout above
#define SS_PIN          10          // Configurable, see typical pin layout above

SoftwareSerial mySerial(6,5); //Sets up RX and TX ports

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance
int Sel = 0;

String inString = "";    // string to hold input

void setup() {
  Serial.begin(9600);                                           // Initialize serial communications with the PC
  SPI.begin();                                                  // Init SPI bus
  mySerial.begin(9600);
  mfrc522.PCD_Init();                                              // Init MFRC522 card
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {

  // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  //some variables we need
  byte block;
  byte len;
  MFRC522::StatusCode status;

  while (Serial.available() == 0) {
    // Wait for User to Input Data
    char input = mySerial.read();
    delay(5);
  }

  if (Sel == 0){
    digitalWrite(LED_BUILTIN,LOW);
    Sel = Serial.parseInt();
  }else if(Sel == 3){//Checks for barcode write
    digitalWrite(LED_BUILTIN,HIGH);
    delay(1000);
  
    while (mySerial.available() == 0) {
      //Waiting for barcode to scan
    }
    
    while(mySerial.available())
    {
      char input = mySerial.read();
      Serial.write(input);
      delay(5);
    }
    Serial.println();
    digitalWrite(LED_BUILTIN,HIGH);
    delay(1000);
    
    Sel = 0;
}else if ( Sel == 1){//Checks for RFID read mode
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! mfrc522.PICC_IsNewCardPresent()) {}

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  byte buffer1[18];
  
  len = 18;

  byte buffer2[18];
  block = 1;

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 1, &key, &(mfrc522.uid)); //line 834
  if (status != MFRC522::STATUS_OK) {
    return;
  }

  status = mfrc522.MIFARE_Read(block, buffer2, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  //PRINT Number
  for (uint8_t i = 0; i < 13; i++) {
    Serial.write(buffer2[i] );
  }
  
  delay(500); //change value if you want to read cards faster
  Sel = 0;

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  
 }else if( Sel == 2){//Checks for RFID write mode

  byte buffer[34];
  Serial.setTimeout(20000L) ;     // wait until 20 seconds for input from serial
  len = Serial.readBytesUntil('#', (char *) buffer, 30) ; // read number from serial
  for (byte i = len; i < 30; i++) buffer[i] = ' ';     // pad with spaces

  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  while ( ! mfrc522.PICC_IsNewCardPresent()) {}
  // Select one of the cards
  while ( ! mfrc522.PICC_ReadCardSerial()) {}

  block = 1;

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 4, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  
  // Write block
  status = mfrc522.MIFARE_Write(4, buffer, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  Sel = 0;
  mfrc522.PICC_HaltA(); // Halt PICC
  mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD
}

//Sel = 0;
 
}


void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.write(buffer[i] < 0x10 ? "0" : "");
        Serial.write(buffer[i]);
    }
}
