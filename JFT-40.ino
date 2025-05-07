#include <Arduino.h>

// ====== CONFIGURAÇÃO DE HARDWARE ======
#define SSI_CLK     25
#define SSI_DATA    34
#define SSI_BITS    24
#define T_CLK_HIGH  5
#define T_CLK_LOW   5

// ====== CONFIGURAÇÃO DE CONVERSÃO ======
#define OFFSET_FIXO     10142794
#define PULSOS_TOTAIS   (10725802.0 - 10142794.0)  // 583008 pulsos
#define COMPRIMENTO_MM  320.0
#define PULSO_PARA_MM   (COMPRIMENTO_MM / PULSOS_TOTAIS)  // ≈ 0.0005487

// ====== FUNÇÃO DE LEITURA SSI ======
uint32_t readSSI() {
  uint32_t value = 0;

  noInterrupts();
  digitalWrite(SSI_CLK, LOW);
  delayMicroseconds(T_CLK_LOW);

  for (int i = 0; i < SSI_BITS; i++) {
    digitalWrite(SSI_CLK, HIGH);
    delayMicroseconds(T_CLK_HIGH);

    value <<= 1;
    if (digitalRead(SSI_DATA)) value |= 1;

    digitalWrite(SSI_CLK, LOW);
    delayMicroseconds(T_CLK_LOW);
  }

  interrupts();
  return value;
}

// ====== SETUP ======
void setup() {
  pinMode(SSI_CLK, OUTPUT);
  pinMode(SSI_DATA, INPUT_PULLDOWN);
  digitalWrite(SSI_CLK, LOW);

  Serial.begin(115200);
  delay(1000);

  Serial.println("🔧 Leitura da régua JFT-40 iniciada...");
}

// ====== LOOP COM CONVERSÃO PARA MM ======
void loop() {
  const uint8_t N = 5;  // número de leituras a serem suavizadas
  static uint32_t buffer[N] = {0};
  static uint8_t index = 0;

  // Lê nova posição bruta
  buffer[index] = readSSI();
  index = (index + 1) % N;

  // Calcula média
  uint32_t soma = 0;
  for (uint8_t i = 0; i < N; i++) {
    soma += buffer[i];
  }

  uint32_t media_pulsos = soma / N;

  // Aplica offset e conversão
  int32_t pos_corrigida = (int32_t)media_pulsos - OFFSET_FIXO;
  float pos_mm = pos_corrigida * PULSO_PARA_MM;

  Serial.print("📏 Posição suavizada (mm): ");
  Serial.println(pos_mm, 3);

  delay(100);
}

