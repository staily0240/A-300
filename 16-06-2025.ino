#include <Arduino.h>

#define SSI_CLOCK_PIN 25
#define SSI_DATA_PIN_X 34
#define SSI_DATA_PIN_Y 35
#define SSI_DATA_PIN_Z 32
#define SSI_NUM_BITS 24

#define ENCODER_OFFSET 10142794
#define ENCODER_MAX_COUNT 10725802
#define TRAVEL_DISTANCE_MM 320.0

unsigned long lastReadTimeUs = 0;
const unsigned long readIntervalUs = 45;

float mapFloat(long x, long in_min, long in_max, float out_min, float out_max) {
  return (float)(x - in_min) * (out_max - out_min) / (float)(in_max - in_min) + out_min;
}

void setup() {
  gpio_set_direction((gpio_num_t)SSI_CLOCK_PIN, GPIO_MODE_OUTPUT);
  gpio_set_direction((gpio_num_t)SSI_DATA_PIN_X, GPIO_MODE_INPUT);
  gpio_set_direction((gpio_num_t)SSI_DATA_PIN_Y, GPIO_MODE_INPUT);
  gpio_set_direction((gpio_num_t)SSI_DATA_PIN_Z, GPIO_MODE_INPUT);

  gpio_set_level((gpio_num_t)SSI_CLOCK_PIN, 1);

  Serial.begin(115200);
}

void loop() {
  if (micros() - lastReadTimeUs >= readIntervalUs) {
    lastReadTimeUs = micros();

    uint32_t rawX = 0, rawY = 0, rawZ = 0;

    for (int i = 0; i < SSI_NUM_BITS; i++) {
      gpio_set_level((gpio_num_t)SSI_CLOCK_PIN, 0);
      delayMicroseconds(1);

      gpio_set_level((gpio_num_t)SSI_CLOCK_PIN, 1);
      delayMicroseconds(1);

      rawX = (rawX << 1) | gpio_get_level((gpio_num_t)SSI_DATA_PIN_X);
      rawY = (rawY << 1) | gpio_get_level((gpio_num_t)SSI_DATA_PIN_Y);
      rawZ = (rawZ << 1) | gpio_get_level((gpio_num_t)SSI_DATA_PIN_Z);
    }

    float positionXMM = mapFloat(rawX, 10725802, 10142794, 0.0, TRAVEL_DISTANCE_MM);
    float positionYMM = mapFloat(rawY, 10725802, 10142794, 0.0, TRAVEL_DISTANCE_MM);
    float positionZMM = mapFloat(rawZ, 10725802, 10142794, 0.0, TRAVEL_DISTANCE_MM);

    Serial.print(" | mm = ");
    Serial.print(positionXMM, 3);

    Serial.print(" | mm = ");
    Serial.print(positionYMM, 3);

    Serial.print(" | mm = ");
    Serial.println(positionZMM, 3);
  }
}
