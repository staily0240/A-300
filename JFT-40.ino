#include <Arduino.h>

#define SSI_CLOCK_PIN     25
#define SSI_DATA_PIN      34
#define SSI_NUM_BITS      24

#define CLOCK_HIGH_TIME   1
#define CLOCK_LOW_TIME    1

#define ENCODER_OFFSET        10142794
#define ENCODER_MAX_COUNT     10725802
#define TOTAL_PULSES          (ENCODER_MAX_COUNT - ENCODER_OFFSET)
#define TRAVEL_DISTANCE_MM    320.0
#define PULSE_TO_MM           (TRAVEL_DISTANCE_MM / TOTAL_PULSES)

unsigned long lastReadTimeUs = 0;
const unsigned long readIntervalUs = 100000;

void beforeSetup() __attribute__((constructor));
void beforeSetup() {
  pinMode(SSI_CLOCK_PIN, OUTPUT);
  digitalWrite(SSI_CLOCK_PIN, LOW);
}

uint32_t readEncoderSSI() {
  uint32_t rawValue = 0;

  noInterrupts();
  digitalWrite(SSI_CLOCK_PIN, LOW);
  delayMicroseconds(CLOCK_LOW_TIME);

  for (int i = 0; i < SSI_NUM_BITS; i++) {
    digitalWrite(SSI_CLOCK_PIN, HIGH);
    delayMicroseconds(1);

    rawValue <<= 1;
    if (digitalRead(SSI_DATA_PIN)) rawValue |= 1;

    delayMicroseconds(CLOCK_HIGH_TIME - 1);
    digitalWrite(SSI_CLOCK_PIN, LOW);
    delayMicroseconds(CLOCK_LOW_TIME);
  }

  interrupts();
  return rawValue;
}

void setup() {
  pinMode(SSI_CLOCK_PIN, OUTPUT);
  pinMode(SSI_DATA_PIN, INPUT);
  digitalWrite(SSI_CLOCK_PIN, LOW);

  Serial.begin(115200);
  delay(2000);

  uint32_t v1 = 0, v2 = 0, v3 = 0;
  do {
    v1 = readEncoderSSI(); delay(100);
    v2 = readEncoderSSI(); delay(100);
    v3 = readEncoderSSI(); delay(100);
  } while (!(v1 == v2 && v2 == v3));

  Serial.println(v1);
  lastReadTimeUs = micros();
}

void loop() {
  unsigned long currentTimeUs = micros();

  if (currentTimeUs - lastReadTimeUs >= readIntervalUs) {
    lastReadTimeUs = currentTimeUs;

    uint32_t rawEncoderValue = readEncoderSSI();
    int32_t encoderOffsetCorrected = (int32_t)rawEncoderValue - ENCODER_OFFSET;
    float positionMM = encoderOffsetCorrected * PULSE_TO_MM;

    Serial.print("Raw: ");
    Serial.print(rawEncoderValue);
    Serial.print(" | mm: ");
    Serial.println(positionMM, 3);
  }
}
