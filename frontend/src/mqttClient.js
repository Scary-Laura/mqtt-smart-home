import mqtt from "mqtt";

const client = mqtt.connect(
  "wss://2b2a5c9afe2b4eb3835d03b81369c5e3.s1.eu.hivemq.cloud/mqtt",
  {
    username: "USUARIO",
    password: "SENHA"
  }
);

export default client;