#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// WiFi-Einstellungen
const char* ssid = "HTL-Z";
const char* password = "36Hf5HF6J4";

// MQTT-Einstellungen
const char* mqttServer = "demo.thingsboard.io";
const int mqttPort = 1883;
const char* mqttUsername = "03xr7WoaqndySRCljMW9";
const char* mqttTopic = "v1/devices/me/telemetry";

#define DHTPIN 14
#define DHTTYPE DHT22

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);

void connectToWiFi() {
  Serial.print("Verbinde mit WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nVerbunden mit WiFi!");
}

void connectToMQTT() {
  client.setServer(mqttServer, mqttPort);
  while (!client.connected()) {
    Serial.print("Verbinde mit MQTT...");
    if (client.connect("ESP32Client", mqttUsername, NULL)) {
      Serial.println("Verbunden mit MQTT!");
    } else {
      Serial.print("Fehler beim Verbinden mit MQTT - ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void publishTemperature(float temperature) {
  String payload = "{\"temperature\": " + String(temperature) + "}";

  client.publish(mqttTopic, payload.c_str());
  Serial.print("Temperatur veröffentlicht: ");
  Serial.println(temperature);
}

void setup() {
  Serial.begin(115200);
  connectToWiFi();
  connectToMQTT();
  dht.begin();
}

void loop() {
  if (!client.connected()) {
    connectToMQTT();
  }

  float temperature = dht.readTemperature();

  if (isnan(temperature)) {
    Serial.println(F("Failed to read temperature from DHT sensor!"));
    return;
  }

  publishTemperature(temperature);

  client.loop();

  delay(100);  // Warte 5 Sekunden vor dem nächsten Publish
}
