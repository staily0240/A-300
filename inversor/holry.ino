#include <ModbusMaster.h>

#define RXD2 16
#define TXD2 17

ModbusMaster node;
String input = "";

// Escravo e endereços reais
const uint8_t slaveID = 1;
const uint16_t reg_freq = 513;     // 0x0201
const uint16_t coil_run  = 0x0048; // Endereço de operação (Run)
const uint16_t coil_stop = 0x004B; // Endereço de parada (Stop)

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8E1, RXD2, TXD2);

  node.begin(slaveID, Serial2);
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);

  Serial.println(">>> Holry H100 via Modbus - Comandos:");
  Serial.println("  run");
  Serial.println("  stop");
  Serial.println("  freq:300  (30.0 Hz)");
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      input.trim();
      processCommand(input);
      input = "";
    } else {
      input += c;
    }
  }
}

void processCommand(String cmd) {
  cmd.toLowerCase();

  if (cmd == "run") {
    uint8_t result = node.writeSingleCoil(coil_run, 0xFF01);
    if (result == node.ku8MBSuccess) Serial.println(">>> Motor LIGADO");
    else {
      Serial.print("Erro Modbus RUN: 0x");
      Serial.println(result, HEX);
    }
  } else if (cmd == "stop") {
    uint8_t result = node.writeSingleCoil(coil_stop, 0xFF01);
    if (result == node.ku8MBSuccess) Serial.println(">>> Motor DESLIGADO");
    else {
      Serial.print("Erro Modbus STOP: 0x");
      Serial.println(result, HEX);
    }
  } else if (cmd.startsWith("freq:")) {
    int val = cmd.substring(5).toInt();
    if (val >= 0 && val <= 8000) {
      uint8_t result = node.writeSingleRegister(reg_freq, val);
      if (result == node.ku8MBSuccess) {
        Serial.print(">>> Frequência definida: ");
        Serial.print(val / 10.0);
        Serial.println(" Hz");
      } else {
        Serial.print("Erro Modbus FREQ: 0x");
        Serial.println(result, HEX);
      }
    } else {
      Serial.println(">>> Valor fora do intervalo (0–8000)");
    }
  } else {
    Serial.println(">>> Comando inválido.");
  }
}
