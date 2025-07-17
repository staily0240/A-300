#define RXD2 16
#define TXD2 17

String input = "";

// Endereços
const uint8_t slaveID = 1;
const uint16_t reg_freq  = 0x0201;
const uint16_t coil_run  = 0x0048;
const uint16_t coil_stop = 0x004B;

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8E1, RXD2, TXD2);

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
    sendWriteCoil(coil_run, true);
  } else if (cmd == "stop") {
    sendWriteCoil(coil_stop, true);
  } else if (cmd.startsWith("freq:")) {
    int val = cmd.substring(5).toInt();
    if (val >= 0 && val <= 8000) {
      sendWriteRegister(reg_freq, val);
    } else {
      Serial.println(">>> Valor fora do intervalo (0–8000)");
    }
  } else {
    Serial.println(">>> Comando inválido.");
  }
}

// Função para escrever em coil (função 0x05)
void sendWriteCoil(uint16_t address, bool on) {
  uint8_t frame[8];
  frame[0] = slaveID;
  frame[1] = 0x05; // Função: Write Single Coil
  frame[2] = address >> 8;
  frame[3] = address & 0xFF;
  frame[4] = on ? 0xFF : 0x00;
  frame[5] = 0x00;

  uint16_t crc = modbus_crc(frame, 6);
  frame[6] = crc & 0xFF;
  frame[7] = crc >> 8;

  Serial2.write(frame, 8);

  if (on) {
    if (address == coil_run) Serial.println(">>> Motor LIGADO");
    else if (address == coil_stop) Serial.println(">>> Motor DESLIGADO");
  }
}

// Função para escrever em registrador (função 0x06)
void sendWriteRegister(uint16_t address, uint16_t value) {
  uint8_t frame[8];
  frame[0] = slaveID;
  frame[1] = 0x06; // Função: Write Single Register
  frame[2] = address >> 8;
  frame[3] = address & 0xFF;
  frame[4] = value >> 8;
  frame[5] = value & 0xFF;

  uint16_t crc = modbus_crc(frame, 6);
  frame[6] = crc & 0xFF;
  frame[7] = crc >> 8;

  Serial2.write(frame, 8);

  Serial.print(">>> Frequência definida: ");
  Serial.print(value / 10.0);
  Serial.println(" Hz");
}

// Cálculo CRC16 padrão Modbus RTU
uint16_t modbus_crc(uint8_t *data, uint8_t len) {
  uint16_t crc = 0xFFFF;
  for (uint8_t i = 0; i < len; i++) {
    crc ^= data[i];
    for (uint8_t j = 0; j < 8; j++) {
      if (crc & 1)
        crc = (crc >> 1) ^ 0xA001;
      else
        crc = crc >> 1;
    }
  }
  return crc;
}
