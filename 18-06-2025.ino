#include <Arduino.h>
//Clock
#define SSI_CLOCK_PIN_1 17
#define SSI_CLOCK_PIN_3 16
#define SSI_CLOCK_PIN_4 22
//AntiClock
#define SSI_CLOCK_PIN_2 19
#define SSI_CLOCK_PIN_5 18
#define SSI_CLOCK_PIN_6 21

#define SSI_DATA_PIN_X 34
#define SSI_DATA_PIN_Y 35
#define SSI_DATA_PIN_Z 32
#define SSI_NUM_BITS 24

#define ENCODER_OFFSET 10142794
#define ENCODER_MAX_COUNT 10725802
#define TRAVEL_DISTANCE_MM 320.0

unsigned long lastReadTimeUs = 0;
const unsigned long readIntervalUs = 1;

float mapFloat(long x, long in_min, long in_max, float out_min, float out_max) {
  return (float)(x - in_min) * (out_max - out_min) / (float)(in_max - in_min) + out_min;
}

void setup() {
  Serial.begin(115200);

  gpio_set_direction((gpio_num_t)SSI_CLOCK_PIN_1, GPIO_MODE_OUTPUT);
  gpio_set_direction((gpio_num_t)SSI_CLOCK_PIN_2, GPIO_MODE_OUTPUT);
  gpio_set_direction((gpio_num_t)SSI_CLOCK_PIN_5, GPIO_MODE_OUTPUT);
  gpio_set_direction((gpio_num_t)SSI_CLOCK_PIN_6, GPIO_MODE_OUTPUT);
  gpio_set_direction((gpio_num_t)SSI_CLOCK_PIN_3, GPIO_MODE_OUTPUT);
  gpio_set_direction((gpio_num_t)SSI_CLOCK_PIN_4, GPIO_MODE_OUTPUT);
  gpio_set_direction((gpio_num_t)SSI_DATA_PIN_X, GPIO_MODE_INPUT);
  gpio_set_direction((gpio_num_t)SSI_DATA_PIN_Y, GPIO_MODE_INPUT);
  gpio_set_direction((gpio_num_t)SSI_DATA_PIN_Z, GPIO_MODE_INPUT);

  gpio_set_level((gpio_num_t)SSI_CLOCK_PIN_1, 1);
  gpio_set_level((gpio_num_t)SSI_CLOCK_PIN_3, 1);
  gpio_set_level((gpio_num_t)SSI_CLOCK_PIN_4, 1);
  gpio_set_level((gpio_num_t)SSI_CLOCK_PIN_2, 0);
  gpio_set_level((gpio_num_t)SSI_CLOCK_PIN_5, 0);
  gpio_set_level((gpio_num_t)SSI_CLOCK_PIN_6, 0);
}


void loop() {
    uint32_t rawX = 0, rawY = 0, rawZ = 0;
    delayMicroseconds(1);

    for (int i = 0; i < SSI_NUM_BITS; i++) {
      gpio_set_level((gpio_num_t)SSI_CLOCK_PIN_1, 0);
      gpio_set_level((gpio_num_t)SSI_CLOCK_PIN_3, 0);
      gpio_set_level((gpio_num_t)SSI_CLOCK_PIN_4, 0);
      gpio_set_level((gpio_num_t)SSI_CLOCK_PIN_2, 1);
      gpio_set_level((gpio_num_t)SSI_CLOCK_PIN_5, 1);
      gpio_set_level((gpio_num_t)SSI_CLOCK_PIN_6, 1);
      delayMicroseconds(1);

      gpio_set_level((gpio_num_t)SSI_CLOCK_PIN_1, 1);
      gpio_set_level((gpio_num_t)SSI_CLOCK_PIN_3, 1);
      gpio_set_level((gpio_num_t)SSI_CLOCK_PIN_4, 1);
      gpio_set_level((gpio_num_t)SSI_CLOCK_PIN_2, 0);
      gpio_set_level((gpio_num_t)SSI_CLOCK_PIN_5, 0);
      gpio_set_level((gpio_num_t)SSI_CLOCK_PIN_6, 0);
      delayMicroseconds(1);

      rawX = (rawX << 1) | gpio_get_level((gpio_num_t)SSI_DATA_PIN_X);
      rawY = (rawY << 1) | gpio_get_level((gpio_num_t)SSI_DATA_PIN_Y);
      rawZ = (rawZ << 1) | gpio_get_level((gpio_num_t)SSI_DATA_PIN_Z);
    }

    float positionXMM = mapFloat(rawX, 10725802, 10142794, 0.0, TRAVEL_DISTANCE_MM);
    float positionYMM = mapFloat(rawY, 10725802, 10142794, 0.0, TRAVEL_DISTANCE_MM);
    float positionZMM = mapFloat(rawZ, 10725802, 10142794, 0.0, TRAVEL_DISTANCE_MM);

    Serial.print(" | Z mm = ");
    Serial.print(positionXMM, 3);

    Serial.print(" | Y mm = ");
    Serial.print(positionYMM, 3);

    Serial.print(" | X mm = ");
    Serial.println(positionZMM, 3);
}
