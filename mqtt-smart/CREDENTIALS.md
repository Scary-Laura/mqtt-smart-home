# Sincronização de Credenciais

Este arquivo ajuda a manter as credenciais sincronizadas entre `.env` e `esp32/config.h`.

## Mapeamento de Campos

| .env | config.h | Descrição |
|------|----------|-----------|
| `HOST` | `mqtt_server` | Endereço do broker HiveMQ |
| `PORT` | `mqtt_port` | Porta MQTT (8883 para SSL) |
| `USERNAME` | `mqtt_user` | Usuário MQTT |
| `PASSWORD` | `mqtt_pass` | Senha MQTT |

## Como Atualizar Credenciais

1. Edite `.env` com as novas credenciais
2. Copie os valores para `esp32/config.h`
3. Recompile e faça upload para o ESP32

## Exemplo

### .env
```env
HOST=meu-broker.hivemq.cloud
PORT=8883
USERNAME=meu_usuario
PASSWORD=minha_senha
```

### esp32/config.h
```cpp
const char* mqtt_server = "meu-broker.hivemq.cloud";
const int mqtt_port = 8883;
const char* mqtt_user = "meu_usuario";
const char* mqtt_pass = "minha_senha";
```

## ⚠️ Segurança

- **Nunca commite `.env` ou `config.h` com credenciais reais no Git**
- Use `.gitignore` para ignorar esses arquivos
- Considere usar um sistema de gerenciamento de secrets para produção
