#include <FastLED.h>
#include <DHT.h>
#include <LEDText.h>


#define NUM_LEDS 256 // Total number of LEDs in the matrix (32x8 = 256)
#define LED_PIN 33 // Pin connected to the LED matrix
#define DHTPIN 14 // Pin connected to the DHT sensor
#define DHTTYPE DHT22 // DHT sensor type (DHT22 or DHT11)

CRGB leds[NUM_LEDS]; // Array to store LED colors
DHT dht(DHTPIN, DHTTYPE); // Initialize the DHT sensor
cLEDText ledText(NUM_LEDS, leds); // Initialize the LEDText object

void setup() {
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS); // Initialize LED matrix
  Serial.begin(9600);
  dht.begin(); // Initialize the DHT sensor
}

void loop() {
  // Wait for a few seconds between measurements
  delay(2000);

  float temperature = dht.readTemperature(); // Read temperature from DHT sensor
  float humidity = dht.readHumidity(); // Read humidity from DHT sensor

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println(F("Failed to read data from DHT sensor!"));
    return;
  }

  char text[16];
  snprintf(text, sizeof(text), "Temp: %.1fC Hum: %.1f%%", temperature, humidity);
  
  // Update LED matrix with text
  ledText.setTextAlignment(TEXT_ALIGN_CENTER);
  ledText.setText(text);
  ledText.setColor(CRGB::White);
  ledText.render();

  // Print temperature to serial monitor
  Serial.print(F("Temperature: "));
  Serial.print(temperature);
  Serial.print(F(" °C\tHumidity: "));
  Serial.print(humidity);
  Serial.println(F(" %"));
}
