#include <Arduino.h>
#include <BluetoothSerial.h>

// Clock
#define SSI_CLOCK_PIN_X 17
#define SSI_CLOCK_PIN_Y 16
#define SSI_CLOCK_PIN_Z 22
// AntiClock
#define SSI_DENIED_CLOCK_PIN_X 19
#define SSI_DENIED_CLOCK_PIN_Y 18
#define SSI_DENIED_CLOCK_PIN_Z 21
// Data
#define SSI_DATA_PIN_X 32
#define SSI_DATA_PIN_Y 35
#define SSI_DATA_PIN_Z 34
#define SSI_NUM_BITS 24

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("A-300");

  // Init GPIO
  gpio_set_direction((gpio_num_t)SSI_CLOCK_PIN_X, GPIO_MODE_OUTPUT);
  gpio_set_direction((gpio_num_t)SSI_DENIED_CLOCK_PIN_X, GPIO_MODE_OUTPUT);
  gpio_set_direction((gpio_num_t)SSI_DENIED_CLOCK_PIN_Y, GPIO_MODE_OUTPUT);
  gpio_set_direction((gpio_num_t)SSI_DENIED_CLOCK_PIN_Z, GPIO_MODE_OUTPUT);
  gpio_set_direction((gpio_num_t)SSI_CLOCK_PIN_Y, GPIO_MODE_OUTPUT);
  gpio_set_direction((gpio_num_t)SSI_CLOCK_PIN_Z, GPIO_MODE_OUTPUT);
  gpio_set_direction((gpio_num_t)SSI_DATA_PIN_X, GPIO_MODE_INPUT);
  gpio_set_direction((gpio_num_t)SSI_DATA_PIN_Y, GPIO_MODE_INPUT);
  gpio_set_direction((gpio_num_t)SSI_DATA_PIN_Z, GPIO_MODE_INPUT);

  gpio_set_level((gpio_num_t)SSI_CLOCK_PIN_X, 1);
  gpio_set_level((gpio_num_t)SSI_CLOCK_PIN_Y, 1);
  gpio_set_level((gpio_num_t)SSI_CLOCK_PIN_Z, 1);
  gpio_set_level((gpio_num_t)SSI_DENIED_CLOCK_PIN_X, 0);
  gpio_set_level((gpio_num_t)SSI_DENIED_CLOCK_PIN_Y, 0);
  gpio_set_level((gpio_num_t)SSI_DENIED_CLOCK_PIN_Z, 0);
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();
    if (c == '1') {
      uint32_t rawX = 0, rawY = 0, rawZ = 0;

      for (int i = 0; i < SSI_NUM_BITS; i++) {
        gpio_set_level((gpio_num_t)SSI_CLOCK_PIN_X, 0);
        gpio_set_level((gpio_num_t)SSI_CLOCK_PIN_Y, 0);
        gpio_set_level((gpio_num_t)SSI_CLOCK_PIN_Z, 0);
        gpio_set_level((gpio_num_t)SSI_DENIED_CLOCK_PIN_X, 1);
        gpio_set_level((gpio_num_t)SSI_DENIED_CLOCK_PIN_Y, 1);
        gpio_set_level((gpio_num_t)SSI_DENIED_CLOCK_PIN_Z, 1);
        delayMicroseconds(3);

        gpio_set_level((gpio_num_t)SSI_CLOCK_PIN_X, 1);
        gpio_set_level((gpio_num_t)SSI_CLOCK_PIN_Y, 1);
        gpio_set_level((gpio_num_t)SSI_CLOCK_PIN_Z, 1);
        gpio_set_level((gpio_num_t)SSI_DENIED_CLOCK_PIN_X, 0);
        gpio_set_level((gpio_num_t)SSI_DENIED_CLOCK_PIN_Y, 0);
        gpio_set_level((gpio_num_t)SSI_DENIED_CLOCK_PIN_Z, 0);
        delayMicroseconds(3);

        rawX = (rawX << 1) | gpio_get_level((gpio_num_t)SSI_DATA_PIN_X);
        rawY = (rawY << 1) | gpio_get_level((gpio_num_t)SSI_DATA_PIN_Y);
        rawZ = (rawZ << 1) | gpio_get_level((gpio_num_t)SSI_DATA_PIN_Z);
      }

      delayMicroseconds(21);

      Serial.print('>'); //Marcador
      Serial.print(rawX);
      Serial.print("|");
      Serial.print(rawY);
      Serial.print("|");
      Serial.println(rawZ);
      Serial.flush();
      delay(5); 

      SerialBT.print(rawX);
      SerialBT.print("|");
      SerialBT.print(rawY);
      SerialBT.print("|");
      SerialBT.println(rawZ);
    }
  }
}
