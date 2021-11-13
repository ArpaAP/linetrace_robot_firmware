#include <SoftwareSerial.h>
#define TX_PIN 2
#define RX_PIN 3

SoftwareSerial BTSerial(TX_PIN, RX_PIN);

void setup() {
  Serial.begin(9600);
  BTSerial.begin(9600);
  
}

void loop() {
  if (BTSerial.available()) {
    Serial.write(BTSerial.read());
  }
  if (Serial.available()) {
    BTSerial.write(Serial.read());
  }
}
