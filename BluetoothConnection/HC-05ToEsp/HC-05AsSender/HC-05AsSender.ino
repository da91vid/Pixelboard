#include <SoftwareSerial.h>

SoftwareSerial BTSerial(10, 11);  // RX, TX Pins of Arduino Nano
unsigned long previousMillis = 0;
const unsigned long interval = 5000;  // 5 seconds

void setup() {
  Serial.begin(9600);
  BTSerial.begin(9600);  // Baud rate for communication with HC-05
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    // Code to send the message every 5 seconds
    String messageToSend = "Hello, world!";
    BTSerial.println(messageToSend);
  }
}
