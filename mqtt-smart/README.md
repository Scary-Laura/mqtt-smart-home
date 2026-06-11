# ESP32 Smart Home

# Integrantes do grupo:
- Larissa Campos Cardoso
- Gabriel Haykonem
- Izak Vasconcelos
- Matheus Henrique
- Laura Rebeca

## Arquivos de Configuração

### config.h
Arquivo de configuração centralizado que contém:
- Credenciais MQTT (baseadas em `.env`)
- Tópicos MQTT
- Definição de pinos (DHT22 e LED)

**Nota**: Este arquivo é gerado a partir das credenciais do `.env`. Não edite manualmente em produção.

## Como Usar

### 1. Configurar Arduino IDE / VS Code Arduino Extension

#### Instalar ESP32:
```
Tools → Board → Boards Manager → Buscar "ESP32" → Instalar "esp32 by Espressif Systems"
```

### 2. Selecionar a Placa
```
Tools → Board → ESP32 Dev Module
Tools → Flash Frequency → 40MHz
Tools → Upload Speed → 115200
```

### 3. Instalar Bibliotecas Necessárias
```
Sketch → Include Library → Manage Libraries

Buscar e instalar:
- PubSubClient (Nick O'Leary)
- DHTesp (Bernd Giesecke)
```

## Monitoramento

Abra o Serial Monitor para ver logs de Debug:
```
Tools → Serial Monitor (Ctrl+Shift+M)
Velocidade: 115200
```

## Tópicos MQTT

- **Status**: `faculdade/grupo01/status` (ONLINE/OFFLINE)
- **Comando LED**: `faculdade/grupo01/comando/led` (ON/OFF)
- **Temperatura**: `faculdade/grupo01/sensor/temperatura` (°C)

### Falha de Conexão MQTT
- Verifique credenciais em `config.h`
- Verifique a porta (8883 para SSL)
- Veja o código de erro no Serial Monitor

### Sensor não lê valores
- Verifique conexão do DHT22
- Certifique-se de que está configurado como DHT22 (não DHT11)

## Segurança

⚠️ **Importante**:
- Nunca commite `.env` ou `config.h` com credenciais reais
- Em produção, use certificado SSL válido em vez de `setInsecure()`
- Considere usar provisioning de credenciais via OTA

## Hospedagem:

