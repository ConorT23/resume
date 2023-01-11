int inByte = 0;

void setup() {
  Serial.begin(9600);
  pinMode(13, OUTPUT);
}

void loop() {

//  if (Serial.available()) {
//    inByte = Serial.read();
//    Serial.write(inByte);
//    digitalWrite(13, HIGH);
//    delay(1000);
//  }

    Serial.println("test");
    delay(3000);
}
