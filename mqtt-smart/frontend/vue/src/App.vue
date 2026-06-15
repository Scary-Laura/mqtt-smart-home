<template>
  <div class="container">
    <h1>📡 Smart Home MQTT</h1>

    <!-- Área de conexão condicional -->
    <div class="connection-area">
      <div v-if="connected" class="connected-info">
        <span class="connected-text">✅ Conectado</span>
        <
        <button @click="disconnectMqtt" class="disconnect-btn">Desconectar</button>
      </div>

      <div v-else class="disconnected-controls">
        <input 
          v-model="brokerUrl" 
          placeholder="wss://seu-broker.com:8884/mqtt" 
          class="broker-input"
        />
        <button @click="connectMqtt" class="connect-btn">Conectar</button>
      </div>
    </div>

    <!-- Status do ESP32 (via LWT) -->
    <div class="esp-status">
      <h2>Status do dispositivo</h2>
      <div class="led-indicator" :class="{ online: espOnline }">
        {{ espOnline ? 'ESP32 ONLINE' : 'ESP32 OFFLINE' }}
      </div>
    </div>

    <!-- Dados dos sensores -->
    <div class="sensors">
      <div class="sensor-card">
        <h3>🌡️ Temperatura</h3>
        <div class="value">{{ temperatura }} °C</div>
        <canvas id="tempChart"></canvas>
      </div>
      <div class="sensor-card">
        <h3>💧 Umidade</h3>
        <div class="value">{{ umidade }} %</div>
        <canvas id="humiChart"></canvas>
      </div>
    </div>

    <!-- Controle do LED -->
    <div class="controls">
      <h2>Controle o LED</h2>
      <button @click="sendLedCommand('ON')" class="led-btn on">🔴 Ligar LED</button>
      <button @click="sendLedCommand('OFF')" class="led-btn off">⚫ Desligar LED</button>
    </div>

    <!-- Log de mensagens -->
    <div class="log">
      <h2>📨 Últimas mensagens</h2>
      <ul>
        <li v-for="(msg, idx) in messageLog" :key="idx">{{ msg }}</li>
      </ul>
    </div>
  </div>
</template>

<script setup>
import { ref, onMounted, onUnmounted } from 'vue'
import mqtt from 'mqtt'
import Chart from 'chart.js/auto'

// Configurações
const brokerUrl = ref('wss://xxxxxx.s1.eu.hivemq.cloud:8884/mqtt') // Altere para o seu broker
const mqttTopicSensor = 'faculdade/grupo01/sensor/+'
const mqttTopicComando = 'faculdade/grupo01/comando/led'
const mqttTopicStatus = 'faculdade/grupo01/status'

let mqttClient = null
const connected = ref(false)
const espOnline = ref(false)
const temperatura = ref('--')
const umidade = ref('--')
const messageLog = ref([])

// Histórico para gráficos
const tempHistory = ref([])
const humiHistory = ref([])
let tempChart = null
let humiChart = null

// ------------------- MQTT -------------------
function connectMqtt() {
  if (mqttClient && connected.value) return

  const options = {
    clean: true,
    reconnectPeriod: 5000,
    clientId: 'web_' + Math.random().toString(16).substr(2, 8),
    // Se o broker exigir autenticação, descomente as linhas abaixo:
    username: 'mqtt-user-Haykonen',
    password: 'Haykonen1301'
  }

  mqttClient = mqtt.connect(brokerUrl.value, options)

  mqttClient.on('connect', () => {
    connected.value = true
    addLog(`✅ Conectado ao broker ${brokerUrl.value}`)
    // Assina os tópicos após conexão
    mqttClient.subscribe(mqttTopicStatus, { qos: 1 })
    mqttClient.subscribe(mqttTopicSensor, { qos: 0 })
  })

  mqttClient.on('message', (topic, payload) => {
    const msg = payload.toString()
    addLog(`📨 ${topic} : ${msg}`)

    if (topic === mqttTopicStatus) {
      espOnline.value = (msg === 'ONLINE')
      addLog(espOnline.value ? '🔌 ESP32 ficou ONLINE' : '💀 ESP32 ficou OFFLINE')
    }
    else if (topic === 'faculdade/grupo01/sensor/temperatura') {
      const temp = parseFloat(msg)
      if (!isNaN(temp)) {
        temperatura.value = temp.toFixed(1)
        tempHistory.value.push(temp)
        if (tempHistory.value.length > 20) tempHistory.value.shift()
        updateChart(tempChart, tempHistory.value)
      }
    }
    else if (topic === 'faculdade/grupo01/sensor/umidade') {
      const hum = parseFloat(msg)
      if (!isNaN(hum)) {
        umidade.value = hum.toFixed(1)
        humiHistory.value.push(hum)
        if (humiHistory.value.length > 20) humiHistory.value.shift()
        updateChart(humiChart, humiHistory.value)
      }
    }
  })

  mqttClient.on('error', (err) => {
    addLog(`❌ Erro MQTT: ${err.message}`)
    connected.value = false
  })

  mqttClient.on('close', () => {
    connected.value = false
    espOnline.value = false
    addLog('🔌 Desconectado do broker')
  })
}

function disconnectMqtt() {
  if (mqttClient) {
    mqttClient.end()
    mqttClient = null
    connected.value = false
    espOnline.value = false
  }
}

function sendLedCommand(command) {
  if (!connected.value || !mqttClient) {
    addLog('⚠️ Não conectado ao broker. Conecte primeiro.')
    return
  }
  mqttClient.publish(mqttTopicComando, command, { qos: 1, retain: false }, (err) => {
    if (err) addLog(`❌ Falha ao publicar: ${err.message}`)
    else addLog(`📤 Comando enviado: ${command} (QoS 1)`)
  })
}

function addLog(msg) {
  messageLog.value.unshift(msg)
  if (messageLog.value.length > 10) messageLog.value.pop()
}

// ------------------- GRÁFICOS -------------------
function initCharts() {
  const tempCtx = document.getElementById('tempChart').getContext('2d')
  const humiCtx = document.getElementById('humiChart').getContext('2d')

  tempChart = new Chart(tempCtx, {
    type: 'line',
    data: { labels: [], datasets: [{ label: 'Temperatura (°C)', data: [], borderColor: 'red', fill: false }] },
    options: { responsive: true, maintainAspectRatio: true }
  })
  humiChart = new Chart(humiCtx, {
    type: 'line',
    data: { labels: [], datasets: [{ label: 'Umidade (%)', data: [], borderColor: 'blue', fill: false }] },
    options: { responsive: true, maintainAspectRatio: true }
  })
}

function updateChart(chart, data) {
  if (!chart) return
  const labels = data.map((_, i) => i + 1)
  chart.data.labels = labels
  chart.data.datasets[0].data = data
  chart.update()
}

// ------------------- CICLO DE VIDA -------------------
onMounted(() => {
  initCharts()
  // Não conecta automaticamente. O usuário deve clicar em "Conectar".
})

onUnmounted(() => {
  if (mqttClient) mqttClient.end()
})
</script>

<style scoped>
* {
  font-family: system-ui, 'Segoe UI', sans-serif;
}
.container {
  max-width: 1200px;
  margin: 0 auto;
  padding: 20px;
  background: #f5f7fb;
  min-height: 100vh;
}
h1 {
  color: #2c3e50;
  text-align: center;
}
.connection-area {
  background: white;
  padding: 15px;
  border-radius: 12px;
  margin-bottom: 20px;
  display: flex;
  justify-content: center;
  align-items: center;
  gap: 25px;
  box-shadow: 0 2px 5px rgba(0,0,0,0.1);
}
.broker-input {
  flex: 2;
  padding: 8px;
  border: 1px solid #ccc;
  border-radius: 6px;
  margin-right: 15px;
  min-width: 250px;
}
.connect-btn, .disconnect-btn {
  background: #3498db;
  color: white;
  border: none;
  padding: 8px 20px;
  border-radius: 6px;
  cursor: pointer;
  font-weight: bold;
}
.disconnect-btn {
  background: #e74c3c;
}
.connected-text {
  font-weight: bold;
  color: #27ae60;
  background: #e8f8f5;
  padding: 5px 12px;
  border-radius: 20px;
}
.esp-status {
  background: white;
  padding: 15px;
  border-radius: 12px;
  margin-bottom: 20px;
  text-align: center;
}
.led-indicator {
  font-weight: bold;
  padding: 10px;
  border-radius: 30px;
  background: #e74c3c;
  color: white;
  display: inline-block;
  width: 200px;
}
.led-indicator.online {
  background: #27ae60;
}
.sensors {
  display: flex;
  flex-wrap: wrap;
  gap: 20px;
  margin-bottom: 30px;
}
.sensor-card {
  background: white;
  flex: 1;
  padding: 15px;
  border-radius: 12px;
  box-shadow: 0 2px 8px rgba(0,0,0,0.1);
  text-align: center;
}
.sensor-card .value {
  font-size: 2rem;
  font-weight: bold;
  margin: 10px 0;
}
canvas {
  max-height: 200px;
  width: 100%;
}
.controls {
  background: white;
  padding: 20px;
  border-radius: 12px;
  text-align: center;
  margin-bottom: 20px;
}
.led-btn {
  margin: 0 10px;
  padding: 10px 25px;
  font-size: 1rem;
  border: none;
  border-radius: 6px;
  cursor: pointer;
  color: white;
}
.led-btn.on {
  background: #e67e22;
}
.led-btn.off {
  background: #7f8c8d;
}
.log {
  background: #2c3e50;
  color: white;
  padding: 15px;
  border-radius: 12px;
  max-height: 250px;
  overflow-y: auto;
}
.log ul {
  margin: 0;
  padding-left: 20px;
}
.log li {
  font-family: monospace;
  margin-bottom: 5px;
}
@media (max-width: 700px) {
  .sensors { flex-direction: column; }
  .connection-area { flex-direction: column; }
  .broker-input { width: 100%; }
}
</style>