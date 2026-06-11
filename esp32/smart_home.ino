#include <WiFi.h>
#include <PubSubClient.h>
#include <DHTesp.h>
#include "config.h"

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

const char* mqtt_server = MQTT_SERVER;

const char* mqtt_user = MQTT_USER;
const char* mqtt_pass = MQTT_PASS;

WiFiClientSecure espClient;
PubSubClient client(espClient);

DHTesp dht;

#define DHTPIN 15
#define LEDPIN 2

void callback(char* topic, byte* payload, unsigned int length) {

  String message;

  for(int i=0;i<length;i++){
    message += (char)payload[i];
  }

  if(String(topic) == "faculdade/grupo01/comando/led"){

    if(message == "ON"){
      digitalWrite(LEDPIN,HIGH);
      Serial.println("LED ligado");
    }else{
      digitalWrite(LEDPIN,LOW);
      Serial.println("LED desligado");
    }
  }
}

void reconnect() {

  while (!client.connected()) {

    if (client.connect(
        "ESP32Client",
        mqtt_user,
        mqtt_pass,
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

    } else {

      delay(5000);
    }
  }
}

void setup() {

  pinMode(LEDPIN,OUTPUT);

  Serial.begin(115200);

  dht.setup(DHTPIN,DHTesp::DHT22);

  WiFi.begin(ssid,password);

  while(WiFi.status()!=WL_CONNECTED){
    delay(500);
  }

  // ⚠️ SEGURANÇA: setInsecure() desativa verificação de certificado SSL
  // Em produção, use certificado CA válido:
  // espClient.setCACert(ca_cert); // definir CA certificate apropriado
  espClient.setInsecure();

  client.setServer(mqtt_server,8883);

  client.setCallback(callback);
}

void loop() {

  if(!client.connected()){
    reconnect();
  }

  client.loop();

  TempAndHumidity data = dht.getTempAndHumidity();

  client.publish(
      "faculdade/grupo01/sensor/temperatura",
      String(data.temperature).c_str(),
      false);

  client.publish(
      "faculdade/grupo01/sensor/umidade",
      String(data.humidity).c_str(),
      false);

  delay(3000);
}