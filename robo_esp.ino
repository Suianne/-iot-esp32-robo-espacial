#include <Arduino.h>
#include <DHT.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <time.h>
#include <math.h>

// =====================================================
// 1. WIFI E BACKEND
// =====================================================
const char* WIFI_SSID = "NOME_DA_SUA_REDE";
const char* WIFI_SENHA = "SENHA_DA_SUA_REDE";

const char* URL_API =
    "http://10.229.155.210:8000/leituras";

// =====================================================
// 2. BUFFER DE LEITURAS
// =====================================================
const int TAMANHO_BUFFER = 20;

struct Leitura {
  String json;
};

Leitura buffer[TAMANHO_BUFFER];
int inicioBuffer = 0;
int quantidadeBuffer = 0;

// =====================================================
// 3. DHT22
// =====================================================
const int DHT_PIN = 7;
#define DHT_TYPE DHT22
DHT dht(DHT_PIN, DHT_TYPE);

// =====================================================
// 4. LDR
// =====================================================
const int LDR_PIN = 10;
const int LIMITE_LUZ = 2500;

// =====================================================
// 5. HC-SR04
// =====================================================
const int TRIG_PIN = 8;
const int ECHO_PIN = 9;
const float DISTANCIA_PRESENCA_CM = 100.0;

// =====================================================
// 6. LEDS
// =====================================================
const int LED_VERDE = 12;
const int LED_VERMELHO = 11;

// =====================================================
// 7. SERVO SG90
// =====================================================
const int SERVO_PIN = 6;
Servo servoExploracao;

int posicaoServo = 0;
int sentidoServo = 1;

unsigned long ultimoMovimentoServo = 0;
const unsigned long INTERVALO_SERVO = 1000;

// =====================================================
// 8. INTERVALO DE LEITURA
// =====================================================
unsigned long ultimaLeitura = 0;
const unsigned long INTERVALO_LEITURA = 2000;

// =====================================================
// 9. CONECTAR WIFI
// =====================================================
void conectarWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_SENHA);

  Serial.print("Conectando ao Wi-Fi...");

  unsigned long inicio = millis();

  while (WiFi.status() != WL_CONNECTED &&
         millis() - inicio < 15000) {
    Serial.print(".");
    delay(300);
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Wi-Fi conectado!");
    Serial.print("IP do ESP32: ");
    Serial.println(WiFi.localIP());

    configTime(0, 0, "pool.ntp.org", "time.google.com");
  } else {
    Serial.println("Wi-Fi indisponivel.");
  }
}

// =====================================================
// 10. TIMESTAMP UTC
// =====================================================
String obterTimestamp() {
  struct tm horario;

  if (!getLocalTime(&horario, 100)) {
    return "";
  }

  char timestamp[25];

  strftime(
    timestamp,
    sizeof(timestamp),
    "%Y-%m-%dT%H:%M:%SZ",
    &horario
  );

  return String(timestamp);
}

// =====================================================
// 11. MEDIR DISTANCIA
// =====================================================
float medirDistancia() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  unsigned long duracao = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duracao == 0) {
    return -1.0;
  }

  return duracao * 0.0343 / 2.0;
}

// =====================================================
// 12. MOVIMENTAR SERVO
// =====================================================
void movimentarServo() {
  if (millis() - ultimoMovimentoServo >= INTERVALO_SERVO) {
    ultimoMovimentoServo = millis();

    posicaoServo += 90 * sentidoServo;

    if (posicaoServo >= 180) {
      posicaoServo = 180;
      sentidoServo = -1;
    } else if (posicaoServo <= 0) {
      posicaoServo = 0;
      sentidoServo = 1;
    }

    servoExploracao.write(posicaoServo);
  }
}

// =====================================================
// 13. ADICIONAR LEITURA AO BUFFER
// =====================================================
void adicionarBuffer(const String& json) {
  if (quantidadeBuffer >= TAMANHO_BUFFER) {
    Serial.println("ERRO: buffer cheio. Leitura descartada.");
    return;
  }

  int posicao =
    (inicioBuffer + quantidadeBuffer) % TAMANHO_BUFFER;

  buffer[posicao].json = json;
  quantidadeBuffer++;

  Serial.print("Leituras pendentes: ");
  Serial.println(quantidadeBuffer);
}

// =====================================================
// 14. ENVIAR LEITURA PENDENTE
// =====================================================
void enviarBuffer() {
  if (quantidadeBuffer == 0) {
    return;
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Sem Wi-Fi. Envio adiado.");
    return;
  }

  String timestamp = obterTimestamp();

  if (timestamp.length() == 0) {
    Serial.println("Aguardando sincronizacao do horario.");
    return;
  }

  HTTPClient http;
  http.setConnectTimeout(2000);
  http.setTimeout(3000);

  if (!http.begin(URL_API)) {
    Serial.println("Falha ao iniciar HTTP.");
    return;
  }

  http.addHeader("Content-Type", "application/json");

  String json = buffer[inicioBuffer].json;

  // Preencher timestamp caso a leitura tenha sido
  // coletada antes da sincronizacao NTP.
  int inicioTimestamp = json.indexOf("\"timestamp\":\"");

  if (inicioTimestamp >= 0) {
    int valorInicio = inicioTimestamp + 13;
    int valorFim = json.indexOf("\"", valorInicio);

    if (valorFim == valorInicio) {
      json = json.substring(0, valorInicio)
           + timestamp
           + json.substring(valorFim);
    }
  }

  int codigoHTTP = http.POST(json);

  if (codigoHTTP >= 200 && codigoHTTP < 300) {
    Serial.print("Leitura enviada. HTTP ");
    Serial.println(codigoHTTP);

    Serial.print("Resposta: ");
    Serial.println(http.getString());

    buffer[inicioBuffer].json = "";
    inicioBuffer = (inicioBuffer + 1) % TAMANHO_BUFFER;
    quantidadeBuffer--;
  } else {
    Serial.print("Falha HTTP: ");
    Serial.println(codigoHTTP);
    Serial.println("Leitura mantida no buffer.");
  }

  http.end();
}

// =====================================================
// 15. CONFIGURACAO INICIAL
// =====================================================
void setup() {
  Serial.begin(115200);

  pinMode(LDR_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  digitalWrite(TRIG_PIN, LOW);
  analogReadResolution(12);

  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);

  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_VERMELHO, LOW);

  dht.begin();

  servoExploracao.setPeriodHertz(50);
  servoExploracao.attach(SERVO_PIN, 500, 2400);
  servoExploracao.write(0);

  ultimaLeitura = millis();
  ultimoMovimentoServo = millis();

  Serial.println();
  Serial.println("=== ROBO EXPLORADOR - ETAPA 03 ===");
  Serial.println("Sensores, LEDs, servo e HTTP POST");

  conectarWiFi();
}

// =====================================================
// 16. LOOP PRINCIPAL
// =====================================================
void loop() {
  movimentarServo();

  // Reconectar ao Wi-Fi periodicamente.
  static unsigned long ultimaTentativaWiFi = 0;

  if (WiFi.status() != WL_CONNECTED &&
      millis() - ultimaTentativaWiFi >= 10000) {
    ultimaTentativaWiFi = millis();
    conectarWiFi();
  }

  // Coletar leituras a cada 2 segundos.
  if (millis() - ultimaLeitura < INTERVALO_LEITURA) {
    return;
  }

  ultimaLeitura = millis();

  // -----------------------------------------------
  // 1. LER SENSORES
  // -----------------------------------------------
  float temperatura = dht.readTemperature();
  float umidade = dht.readHumidity();
  int luz = analogRead(LDR_PIN);
  float distancia = medirDistancia();

  bool dhtValido =
    !isnan(temperatura) && !isnan(umidade);

  bool presencaDetectada =
    distancia >= 0 &&
    distancia <= DISTANCIA_PRESENCA_CM;

  // -----------------------------------------------
  // 2. CALCULAR PROBABILIDADE DE VIDA
  // -----------------------------------------------
  int probabilidade = 0;

  if (dhtValido &&
      temperatura >= 15 && temperatura <= 30) {
    probabilidade += 25;
  }

  if (dhtValido &&
      umidade >= 40 && umidade <= 70) {
    probabilidade += 25;
  }

  if (luz > LIMITE_LUZ) {
    probabilidade += 20;
  }

  if (presencaDetectada) {
    probabilidade += 30;
  }

  probabilidade = constrain(probabilidade, 0, 100);

  bool alerta = probabilidade > 75;

  // -----------------------------------------------
  // 3. CONTROLAR LEDS
  // -----------------------------------------------
  digitalWrite(LED_VERDE, alerta ? LOW : HIGH);
  digitalWrite(LED_VERMELHO, alerta ? HIGH : LOW);

  // -----------------------------------------------
  // 4. EXIBIR LEITURAS
  // -----------------------------------------------
  Serial.println();
  Serial.println("=== LEITURA DOS SENSORES ===");

  if (dhtValido) {
    Serial.print("Temperatura: ");
    Serial.print(temperatura, 1);
    Serial.println(" C");

    Serial.print("Umidade: ");
    Serial.print(umidade, 1);
    Serial.println(" %");
  } else {
    Serial.println("DHT22: leitura indisponivel");
  }

  Serial.print("Luminosidade: ");
  Serial.println(luz);

  if (distancia >= 0) {
    Serial.print("Distancia: ");
    Serial.print(distancia, 1);
    Serial.println(" cm");
  } else {
    Serial.println("Distancia: leitura invalida");
  }

  Serial.print("Presenca: ");
  Serial.println(presencaDetectada ? 1 : 0);

  Serial.print("Probabilidade de vida: ");
  Serial.print(probabilidade);
  Serial.println("%");

  Serial.println(alerta ? "Estado: ALERTA" : "Estado: NORMAL");

  // -----------------------------------------------
  // 5. MONTAR JSON E GUARDAR NO BUFFER
  // -----------------------------------------------
  String timestamp = obterTimestamp();

  String json = "{\"timestamp\":\"" + timestamp + "\"";

  if (dhtValido) {
    json += ",\"temperatura_c\":" + String(temperatura, 1);
    json += ",\"umidade_pct\":" + String(umidade, 1);
  } else {
    json += ",\"temperatura_c\":null";
    json += ",\"umidade_pct\":null";
  }

  json += ",\"luminosidade\":" + String(luz);
  json += ",\"presenca\":" + String(presencaDetectada ? 1 : 0);
  json += ",\"probabilidade_vida\":" + String(probabilidade);
  json += "}";

  adicionarBuffer(json);

  // -----------------------------------------------
  // 6. TENTAR ENVIAR A LEITURA MAIS ANTIGA
  // -----------------------------------------------
  enviarBuffer();
}
