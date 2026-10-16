#include <Arduino.h>

// ========================================
// PINOS DO ESP32-S3
// ========================================

// Mapeamento baseado nos testes do joystick
#define PINO_EIXO_VERTICAL 4
#define PINO_EIXO_HORIZONTAL 5

#define BOTAO_DESLIGAR 18

#define LED_VERDE 15
#define LED_VERMELHO 16

// ========================================
// CONFIGURACOES
// ========================================

const int ADC_MAXIMO = 4095;
const int CENTRO = 2048;
const int ZONA_MORTA = 600;

const unsigned long DEBOUNCE_MS = 50;
const unsigned long INTERVALO_ENVIO = 150;

// ========================================
// VARIAVEIS GLOBAIS
// ========================================

bool roboLigado = true;

int ultimaLeituraBotao = HIGH;
int estadoEstavelBotao = HIGH;

unsigned long tempoDebounce = 0;
unsigned long ultimoEnvio = 0;

// ========================================
// ATUALIZAR LEDS DE STATUS
// ========================================

void atualizarLEDs() {
  digitalWrite(LED_VERDE, roboLigado ? HIGH : LOW);
  digitalWrite(LED_VERMELHO, roboLigado ? LOW : HIGH);
}

// ========================================
// IDENTIFICAR DIRECAO DO JOYSTICK
// X: esquerda diminui; direita aumenta
// Y: baixo diminui; cima aumenta
// ========================================

String lerComando(int valorX, int valorY) {

  int dx = valorX - CENTRO;
  int dy = valorY - CENTRO;

  // Joystick centralizado
  if (abs(dx) < ZONA_MORTA &&
      abs(dy) < ZONA_MORTA) {
    return "Parado";
  }

  // Verifica qual eixo foi mais movimentado
  if (abs(dy) >= abs(dx)) {

    // Y aumenta: frente
    if (dy > ZONA_MORTA) {
      return "Frente";
    }

    // Y diminui: tras
    if (dy < -ZONA_MORTA) {
      return "Tras";
    }

  } else {

    // X diminui: esquerda
    if (dx < -ZONA_MORTA) {
      return "Esquerda";
    }

    // X aumenta: direita
    if (dx > ZONA_MORTA) {
      return "Direita";
    }
  }

  return "Parado";
}

// ========================================
// BOTAO DE LIGAR E DESLIGAR
// ========================================

void verificarBotao() {

  int leitura = digitalRead(BOTAO_DESLIGAR);

  if (leitura != ultimaLeituraBotao) {
    tempoDebounce = millis();
  }

  if (millis() - tempoDebounce > DEBOUNCE_MS) {

    if (leitura != estadoEstavelBotao) {
      estadoEstavelBotao = leitura;

      // O botao pressionado fornece LOW
      if (estadoEstavelBotao == LOW) {

        // Alterna o estado a cada pressionamento
        roboLigado = !roboLigado;

        atualizarLEDs();

        if (roboLigado) {
          Serial.println("Comando enviado: LIGAR");
          Serial.println("Status: robo ligado");
        } else {
          Serial.println("Comando enviado: DESLIGAR");
          Serial.println("Status: robo desligado");
        }
      }
    }
  }

  ultimaLeituraBotao = leitura;
}

// ========================================
// INICIALIZACAO
// ========================================

void setup() {

  Serial.begin(115200);

  analogReadResolution(12);

  pinMode(PINO_EIXO_VERTICAL, INPUT);
  pinMode(PINO_EIXO_HORIZONTAL, INPUT);

  pinMode(BOTAO_DESLIGAR, INPUT_PULLUP);

  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);

  atualizarLEDs();

  Serial.println("================================");
  Serial.println("CONTROLE REMOTO DO ROBO");
  Serial.println("X = horizontal");
  Serial.println("Y = vertical");
  Serial.println("Status: robo ligado");
  Serial.println("================================");
}

// ========================================
// LOOP PRINCIPAL
// ========================================

void loop() {

  // Continua permitindo ligar novamente
  // mesmo quando o sistema estiver desligado
  verificarBotao();

  if (!roboLigado) {
    delay(10);
    return;
  }

  // Leitura dos eixos conforme os testes
  int leituraVertical =
      analogRead(PINO_EIXO_VERTICAL);

  int leituraHorizontal =
      analogRead(PINO_EIXO_HORIZONTAL);

  // Coordenadas normalizadas
  // Esquerda: X diminui
  // Direita: X aumenta
  int valorX = ADC_MAXIMO - leituraHorizontal;

  // Cima: Y aumenta
  // Baixo: Y diminui
  int valorY = leituraVertical;

  // Identifica o comando
  String comando = lerComando(valorX, valorY);

  // Exibe as coordenadas e a direcao
  if (millis() - ultimoEnvio >= INTERVALO_ENVIO) {

    Serial.print("X: ");
    Serial.print(valorX);

    Serial.print(" | Y: ");
    Serial.print(valorY);

    Serial.print(" | Comando: ");
    Serial.println(comando);

    ultimoEnvio = millis();
  }

  delay(10);
}
