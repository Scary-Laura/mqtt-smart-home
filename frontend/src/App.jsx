import { useEffect, useState } from "react";
import mqttClient from "./mqttClient";

import {
  Line
} from "react-chartjs-2";

import {
  Chart,
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement,
  Tooltip,
  Legend
} from "chart.js";

Chart.register(
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement,
  Tooltip,
  Legend
);

function App() {

  const [temp,setTemp] = useState(0);

  const [hum,setHum] = useState(0);

  const [history,setHistory] = useState([]);

  useEffect(()=>{

    const handleConnect = ()=>{
      mqttClient.subscribe(
        "faculdade/grupo01/sensor/+"
      );

      mqttClient.subscribe(
        "faculdade/grupo01/status"
      );
    };

    const handleMessage = (topic,message)=>{
      const value = message.toString();

      if(topic.includes("temperatura")){
        setTemp(Number(value));

        setHistory(prev=>[
          ...prev.slice(-10),
          Number(value)
        ]);
      }

      if(topic.includes("umidade")){
        setHum(Number(value));
      }
    };

    mqttClient.on("connect", handleConnect);
    mqttClient.on("message", handleMessage);

    return () => {
      mqttClient.off("connect", handleConnect);
      mqttClient.off("message", handleMessage);
    };

  },[]);

  const ligarLed = ()=>{

    mqttClient.publish(
      "faculdade/grupo01/comando/led",
      "ON",
      {
        qos:1
      }
    );
  };

  const desligarLed = ()=>{

    mqttClient.publish(
      "faculdade/grupo01/comando/led",
      "OFF",
      {
        qos:1
      }
    );
  };

  return (

    <div style={{padding:"30px"}}>

      <h1>Smart Home MQTT</h1>

      <h2>Temperatura</h2>

      <h1>{temp} °C</h1>

      <h2>Umidade</h2>

      <h1>{hum} %</h1>

      <button onClick={ligarLed}>
        Ligar LED
      </button>

      <button onClick={desligarLed}>
        Desligar LED
      </button>

      <Line
        data={{
          labels: history.map((_,i)=>i+1),
          datasets:[
            {
              label:"Temperatura",
              data:history,
              borderColor: "#ff6384",
              backgroundColor: "rgba(255, 99, 132, 0.1)",
              tension: 0.4
            }
          ]
        }}
        options={{
          responsive: true,
          maintainAspectRatio: true,
          plugins: {
            legend: {
              display: true
            }
          }
        }}
      />

    </div>
  );
}

export default App;