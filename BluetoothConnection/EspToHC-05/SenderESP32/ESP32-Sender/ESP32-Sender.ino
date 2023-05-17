#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

String MACadd = "98:D3:41:F5:F6:6B";
uint8_t address[6]  = {0x98, 0xD3, 0x41, 0xF5, 0xf6, 0x6B};
String name = "HC-05";
const char* pin = "1234";
bool connected;

unsigned long previousMillis = 0;
const unsigned long interval = 10000; // 10 seconds

void setup() {
  Serial.begin(9600);

  SerialBT.begin("ESP32test", true);
  SerialBT.setPin(pin);
  Serial.println("The device started in master mode, make sure the remote BT device is on!");

  connected = SerialBT.connect(address);

  if (connected) {
    Serial.println("Connected Successfully!");
  } else {
    while (!SerialBT.connected(10000)) {
      Serial.println("Failed to connect. Make sure the remote device is available and in range, then restart the app.");
    }
  }
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    if (SerialBT.connected()) {
      const char* message = "Steff";
      SerialBT.println(message);
    }
  }
}
