#include "WiFi.h"
#include "PubSubClient.h"
#include "DHTesp.h"
#include "config.h"


WiFiClientSecure espClient;
PubSubClient client(espClient);

DHTesp dht;

void callback(char* topic, byte* payload, unsigned int length) {

  String message;

  for(int i=0;i<length;i++){
    message += (char)payload[i];
  }

  Serial.print("Mensagem recebida em ");
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(message);

  if(String(topic) == topic_led_command){

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

    Serial.print("Tentando conexão MQTT...");
    
    if (client.connect(
        "ESP32Client",
        mqtt_user,
        mqtt_pass,
        topic_status,
        1,
        true,
        "OFFLINE")) {

      Serial.println("Conectado!");
      
      client.publish(
        topic_status,
        "ONLINE",
        true);

      client.subscribe(
        topic_led_command,
        1);

    } else {

      Serial.print("Falha. Código: ");
      Serial.println(client.state());
      Serial.println("Tentando novamente em 5 segundos...");
      
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

  client.setServer(mqtt_server, mqtt_port);

  client.setCallback(callback);
}

void loop() {

  if(!client.connected()){
    reconnect();
  }

  client.loop();

  TempAndHumidity data = dht.getTempAndHumidity();

  // Validar leitura do sensor
  if (isnan(data.temperature) || isnan(data.humidity)) {
    Serial.println("Erro ao ler sensor DHT");
    delay(3000);
    return;
  }

  // Publicar temperatura
  if (!client.publish(
      topic_temperature,
      String(data.temperature).c_str(),
      false)) {
    Serial.println("Falha ao publicar temperatura");
  }

  // Publicar umidade
  if (!client.publish(
      topic_humidity,
      String(data.humidity).c_str(),
      false)) {
    Serial.println("Falha ao publicar umidade");
  }

  Serial.print("Temperatura: ");
  Serial.print(data.temperature);
  Serial.print("°C | Umidade: ");
  Serial.print(data.humidity);
  Serial.println("%");

  delay(3000);
}