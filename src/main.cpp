#include <Arduino.h>
#include <ESP32Boat.hpp>

#define PIN_PROP1_PWM 17
#define PIN_PROP2_PWM 16
#define PIN_SERVO_PWM 27

#define DISP_SERIAL

#define INV_UINT8_MAX 0.00390625f

ESP32Boat::ESP32Boat *boat;
uint8_t cnt = 0;

void setup() {
  boat = new ESP32Boat::ESP32Boat(PIN_SERVO_PWM, PIN_PROP1_PWM, PIN_PROP2_PWM);

  #ifdef DISP_SERIAL
  Serial.begin(115200);
  #endif
}

void loop() {
  auto angle = cnt * INV_UINT8_MAX * ESP32Boat::SERVO_ANGLE_RANGE + ESP32Boat::SERVO_MIN_ANGLE;
  auto speed = cnt * INV_UINT8_MAX * 200.0f -100.0f;
  boat -> changePropSpeed(speed);
  boat -> changeAngle(angle);

  cnt = cnt + 1;

  #ifdef DISP_SERIAL
  Serial.print("["); Serial.print(cnt); Serial.print("] ");
  Serial.print("Speed: "); Serial.print(speed);
  Serial.print("Angle: "); Serial.println(angle);
  #endif

  delay(10);
}