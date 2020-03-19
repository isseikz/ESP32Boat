#define DISP_SERIAL
// #define DISP_BLE

#include <Arduino.h>
#include <ESP32Boat.hpp>
#include <ESP32Boat_BLE.hpp>

#define PIN_PROP1_PWM 17
#define PIN_PROP2_PWM 16
#define PIN_SERVO_PWM 27
#define INV_UINT8_MAX 0.00390625f

ESP32Boat::ESP32Boat *boat;
ESP32Boat_BLE::ESP32Boat_BLE *ble;
uint8_t cnt = 0;
uint8_t cnt_last_updated = 0;

void setup() {
  #ifdef DISP_SERIAL
  Serial.begin(115200);
  #endif

  boat = new ESP32Boat::ESP32Boat(PIN_SERVO_PWM, PIN_PROP1_PWM, PIN_PROP2_PWM);
  ble  = new ESP32Boat_BLE::ESP32Boat_BLE();
}

void loop() {
  float angle, speed;

  if(ESP32Boat_BLE::deviceConnected)
  {
    if (ESP32Boat_BLE::isUpdated)
    {
      angle = ESP32Boat_BLE::angle;
      speed = ESP32Boat_BLE::speed;
      ESP32Boat_BLE::isUpdated = false;
      cnt_last_updated = 0;
    } else {
      cnt_last_updated ++;
    }

    if (cnt_last_updated > 5)
    {
      angle = speed = 0.0f;
    }
    
    
  } else {
    angle = cnt * INV_UINT8_MAX * ESP32Boat::SERVO_ANGLE_RANGE + ESP32Boat::SERVO_MIN_ANGLE;
    speed = cnt * INV_UINT8_MAX * 200.0f -100.0f;
  }

  boat -> changePropSpeed(speed);
  boat -> changeAngle(angle);

  cnt = cnt + 1;

  #ifdef DISP_SERIAL
  Serial.print("["); Serial.print(cnt); Serial.print("] ");
  Serial.print("Speed: "); Serial.print(speed);
  Serial.print("Angle: "); Serial.println(angle);
  #endif

  delay(100);
}