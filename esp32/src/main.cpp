#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <DHTesp.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";

const char* mqtt_server = "2b2a5c9afe2b4eb3835d03b81369c5e3.s1.eu.hivemq.cloud";

const char* mqtt_user = "larissagrva";
const char* mqtt_password = "Gcyrino1.";

WiFiClientSecure wifiClient;
PubSubClient client(wifiClient);

DHTesp dht;

#define DHT_PIN 15
#define LED_PIN 2

void callback(char* topic, byte* payload, unsigned int length) {

  String message;

  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  if (String(topic) == "faculdade/grupo01/comando/led") {

    if (message == "ON") {
      digitalWrite(LED_PIN, HIGH);
    } else {
      digitalWrite(LED_PIN, LOW);
    }
  }
}

void reconnect() {

  while (!client.connected()) {

    if (client.connect(
            "ESP32Client",
            mqtt_user,
            mqtt_password,
            "faculdade/grupo01/status",
            1,
            true,
            "OFFLINE")) {

      client.publish(
        "faculdade/grupo01/status",
        "ONLINE",
        true);

      client.subscribe(
        "faculdade/grupo01/comando/led",
        1);
    }

    delay(2000);
  }
}

void setup() {

  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);

  dht.setup(DHT_PIN, DHTesp::DHT22);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  wifiClient.setInsecure();

  client.setServer(mqtt_server, 8883);

  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  TempAndHumidity data = dht.getTempAndHumidity();

  client.publish(
      "faculdade/grupo01/sensor/temperatura",
      String(data.temperature).c_str());

  client.publish(
      "faculdade/grupo01/sensor/umidade",
      String(data.humidity).c_str());

  delay(3000);
}