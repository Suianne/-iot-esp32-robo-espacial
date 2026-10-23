# 🤖 Robô Explorador Espacial — ESP32 e IoT

Projeto de robótica e Internet das Coisas (IoT) com ESP32, sensores e backend Python. O sistema realiza leituras do ambiente, envia os dados por Wi-Fi para uma API e armazena as informações em um banco de dados SQLite.

## 📌 Funcionalidades

- Leitura de temperatura e umidade com DHT22.
- Medição de luminosidade com LDR.
- Detecção de proximidade com sensor ultrassônico HC-SR04.
- Sinalização por LEDs e acionamento de servo motor.
- Envio de leituras via HTTP para uma API FastAPI.
- Armazenamento e consulta dos dados por meio de uma API REST.
- Simulação do controle remoto com ESP32-S3 e joystick no Velxio.

## 🧰 Componentes

| Componente | Quantidade | Função |
|---|---:|---|
| ESP32-S3 | 1 | Controlador principal |
| Sensor DHT22 | 1 | Temperatura e umidade |
| Sensor LDR | 1 | Luminosidade |
| Sensor ultrassônico HC-SR04 | 1 | Distância e proximidade |
| LED verde | 1 | Sinalização |
| LED vermelho | 1 | Sinalização |
| Resistores para LEDs | 2 | Limitação de corrente |
| Servo motor SG90 | 1 | Movimento mecânico |
| Protoboard e jumpers | Conforme necessário | Montagem do circuito |
| Fonte USB e alimentação adequada | Conforme necessário | Alimentação dos componentes |

## 🔌 1. Como montar o robô

### 1.1. Faça as conexões

Utilize o mapeamento de pinos definido no firmware `robo_esp.ino`.

| Componente | Pino do ESP32-S3 |
|---|---|
| DHT22 — sinal | GPIO 7 |
| LDR — saída analógica | GPIO 10 |
| HC-SR04 — TRIG | GPIO 8 |
| HC-SR04 — ECHO | GPIO 9 |
| LED verde | GPIO 12 |
| LED vermelho | GPIO 11 |
| Servo SG90 — sinal | GPIO 6 |

### 1.2. Cuidados com a montagem

1. Conecte o GND dos componentes ao GND comum do circuito.
2. Instale os resistores em série com os LEDs.
3. Alimente os sensores de acordo com suas especificações.
4. **Atenção ao HC-SR04:** o pino ECHO convencional pode fornecer 5 V. Utilize um divisor de tensão ou conversor de nível para adequar o sinal aos 3,3 V aceitos pelo ESP32-S3.
5. Para o servo motor, prefira uma fonte de alimentação de 5 V dimensionada para a corrente necessária e mantenha o GND da fonte em comum com o GND do ESP32.
6. Confira todas as conexões antes de energizar o circuito.

> O mapeamento acima documenta os sensores e atuadores presentes no firmware. As conexões de motores de locomoção, caso sejam utilizados, devem seguir o driver e os pinos definidos no código específico desses motores.

## 💻 2. Como configurar e executar o ESP32

1. Instale a [Arduino IDE](https://www.arduino.cc/en/software).
2. Adicione o suporte para placas ESP32 pelo Gerenciador de Placas.
3. Instale as bibliotecas necessárias pelo Gerenciador de Bibliotecas:
   - `DHT sensor library`, de Adafruit;
   - `ESP32Servo`.
4. Abra o arquivo `robo_esp.ino`.
5. Configure o nome e a senha da rede Wi-Fi utilizada pelo ESP32.
6. Configure a URL da API com o endereço IP da máquina que executa o backend e a porta `8000`:

   ```cpp
   const char* WIFI_SSID = "NOME_DA_REDE";
   const char* WIFI_SENHA = "SENHA_DA_REDE";
   const char* URL_API = "http://IP_DA_MAQUINA:8000/leituras";
   ```

7. Selecione a placa e a porta serial corretas.
8. Compile e envie o código para o ESP32.
9. Abra o Monitor Serial em `115200 baud` para acompanhar a conexão Wi-Fi, as leituras dos sensores e o envio dos dados.

O ESP32 e o computador que executa o backend precisam estar em redes com comunicação entre si. Substitua `IP_DA_MAQUINA` pelo endereço IPv4 acessível a partir do ESP32. Não utilize o IP de uma sessão de rede anterior sem verificá-lo novamente.

## 🐍 3. Como executar o backend Python

O backend utiliza FastAPI para receber as leituras enviadas pelo ESP32 e SQLite para armazenar os registros.

### 3.1. Pré-requisitos

- Python 3 instalado.
- Git, caso deseje clonar o repositório.
- Conexão de rede entre o ESP32 e a máquina do backend.

### 3.2. Baixe o projeto

Se ainda não tiver o repositório localmente, execute:

```bash
git clone https://github.com/Suianne/-iot-esp32-robo-espacial.git
cd -iot-esp32-robo-espacial
```

Entre na pasta do backend:

```bash
cd backend_sensores
```

### 3.3. Instale as dependências

Crie e ative um ambiente virtual para isolar as dependências.

No Windows:

```powershell
python -m venv .venv
.venv\Scripts\Activate.ps1
```

Instale as dependências declaradas no projeto:

```bash
python -m pip install -r requirements.txt
```

Se o projeto não tiver um arquivo `requirements.txt`, instale as dependências utilizadas pela aplicação, incluindo FastAPI e Uvicorn, além das bibliotecas de banco de dados exigidas pelo código.

### 3.4. Inicie a API

Na pasta que contém o arquivo `main.py`, execute:

```bash
python -m uvicorn main:app --host 0.0.0.0 --port 8000
```

Quando o servidor iniciar, a API estará disponível na porta `8000`.

Para abrir a documentação interativa, acesse:

```text
http://localhost:8000/docs
```

Para permitir acesso a partir de outro dispositivo, utilize o endereço IP da máquina que está executando o backend:

```text
http://IP_DA_MAQUINA:8000/docs
```

Se o backend estiver em uma máquina virtual, verifique a configuração de rede e as regras de firewall para permitir a comunicação.

## 📊 4. Como consultar os dados salvos

### 4.1. Pela documentação da API

Com o backend em execução:

1. Acesse `http://localhost:8000/docs` ou o endereço IP correspondente.
2. Localize o endpoint `GET /leituras`.
3. Clique em **Try it out**.
4. Clique em **Execute**.
5. Consulte os registros retornados pela API.

### 4.2. Pela URL da API

Também é possível consultar as leituras pelo navegador ou por uma ferramenta HTTP:

```text
GET http://localhost:8000/leituras
```

Se estiver consultando a partir de outro computador, substitua `localhost` pelo IP da máquina que hospeda o backend.

### 4.3. Envio de dados

O firmware envia as leituras para:

```text
POST http://IP_DA_MAQUINA:8000/leituras
Content-Type: application/json
```

O conteúdo JSON deve seguir o formato aceito pelo backend. O firmware envia informações como timestamp, temperatura, umidade, luminosidade, presença e probabilidade calculada.

### 4.4. Persistência dos registros

Os dados são armazenados no banco SQLite utilizado pelo backend. Os registros já gravados permanecem disponíveis após o encerramento do servidor, desde que o arquivo do banco seja preservado.

O nome e o caminho do arquivo SQLite dependem da configuração da aplicação. Consulte o código do backend para localizar o arquivo efetivamente utilizado.

## 🧪 5. Testes e validação

Para verificar o funcionamento completo do sistema:

1. Inicie o backend Python.
2. Ligue o ESP32 e confirme a conexão Wi-Fi no Monitor Serial.
3. Observe se as leituras dos sensores são realizadas.
4. Verifique se o firmware informa que os registros foram enviados com sucesso.
5. Acesse `GET /leituras` e confirme que os dados aparecem na resposta.
6. Reinicie o backend e consulte novamente os registros para verificar a persistência no SQLite.

## 🎮 6. Simulação do controle remoto no Velxio

O repositório também documenta a simulação de um controle remoto com ESP32-S3, joystick analógico, LEDs e botão.

A simulação permite testar os comandos de direção — frente, trás, esquerda e direita — e o comando `DESLIGAR`.

Link da simulação informado no projeto: [Controle com joystick](https://velxio.dev/angelo-oliveira/joystick/).

## 📁 7. Estrutura do repositório

```text
-iot-esp32-robo-espacial/
├── backend_sensores/   # API Python e persistência dos dados
├── codigo_joystick.ino # Código do controle remoto
├── robo_esp.ino        # Firmware do ESP32
└── README.md           # Documentação do projeto
```

## 🛠️ Tecnologias utilizadas

- C++ e Arduino IDE
- ESP32-S3
- Python
- FastAPI
- Uvicorn
- SQLite
- HTTP e JSON
- Velxio para simulação
