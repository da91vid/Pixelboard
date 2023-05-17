#include <SoftwareSerial.h>

SoftwareSerial BTSerial(10, 11);  // RX, TX Pins of Arduino Nano

void setup() {
  Serial.begin(9600);
  BTSerial.begin(9600);  // Baud rate for communication with HC-05
}

void loop() {
  if (BTSerial.available()) {
    String receivedMessage = BTSerial.readString();
    Serial.print("Received message: ");
    Serial.println(receivedMessage);
  }
}
