#ifndef ESP32BOAT_HPP
#define ESP32BOAT_HPP

#include <Arduino.h>

#define PROP_PWM_CH1 2
#define PROP_PWM_CH2 3
#define PROP_PWM_FREQ 50
#define PROP_PWM_BIT 8
#define PROP_PWM_MIN 10  // [%]
#define PROP_PWM_MAX 100 // [%]

#define SERVO_PWM_CH 1
#define SERVO_PWM_FREQ 50
#define SERVO_PWM_BIT 16
#define SERVO_PWM_MIN 1.0 // [ms]
#define SERVO_PWM_MAX 2.0 // [ms]
#define SERVO_ANGLE2RANGE 0.0002777777f // = 1 / 90.0 * 0.025

namespace ESP32Boat
{
    const float SERVO_MAX_ANGLE =  90.0;
    const float SERVO_MIN_ANGLE = -90.0;
    const float SERVO_ANGLE_RANGE = SERVO_MAX_ANGLE - SERVO_MIN_ANGLE;

    struct PWMConfig {
    int pin;
    int ch;
    int frequencyHz;
    int resolution;
    int valMin;
    int valMax;
    };

    class ESP32Boat
    {
    private:
    PWMConfig *cfgServo;
    PWMConfig *cfgProp1;
    PWMConfig *cfgProp2;
    public:
    ESP32Boat(int servo_pin, int prop_pin_1, int prop_pin_2);
    ~ESP32Boat();
    void changePropSpeed(float speed_percent);
    void changeAngle(float angle_deg);
    };

    ESP32Boat::ESP32Boat(int servo_pin, int prop_pin_1, int prop_pin_2)
    {
        cfgServo = (PWMConfig *) malloc(sizeof(PWMConfig));
        cfgServo -> pin = servo_pin;
        cfgServo -> ch = SERVO_PWM_CH;
        cfgServo -> frequencyHz = SERVO_PWM_FREQ;
        cfgServo -> resolution = (int) pow(2.0, SERVO_PWM_BIT);
        cfgServo -> valMin = (int) (SERVO_PWM_MIN * 0.001f * SERVO_PWM_FREQ * cfgServo -> resolution);
        cfgServo -> valMax = (int) (SERVO_PWM_MAX * 0.001f * SERVO_PWM_FREQ * cfgServo -> resolution);
        ledcSetup(cfgServo -> ch, cfgServo -> frequencyHz, SERVO_PWM_BIT);
        ledcAttachPin(cfgServo -> pin, cfgServo -> ch);

        cfgProp1 = (PWMConfig *) malloc(sizeof(PWMConfig));
        cfgProp1 -> pin = prop_pin_1;
        cfgProp1 -> ch = PROP_PWM_CH1;
        cfgProp1 -> frequencyHz = PROP_PWM_FREQ;
        cfgProp1 -> resolution = (int) pow(2.0, PROP_PWM_BIT);
        cfgProp1 -> valMin = (int) (PROP_PWM_MIN * cfgProp1 -> resolution);
        cfgProp1 -> valMax = (int) (PROP_PWM_MAX * cfgProp1 -> resolution);
        ledcSetup(cfgProp1 -> ch, cfgProp1 -> frequencyHz, PROP_PWM_BIT);
        ledcAttachPin(cfgProp1 -> pin, cfgProp1 -> ch);

        cfgProp2 = (PWMConfig *) malloc(sizeof(PWMConfig));
        cfgProp2 -> pin = prop_pin_2;
        cfgProp2 -> ch = PROP_PWM_CH2;
        cfgProp2 -> frequencyHz = PROP_PWM_FREQ;
        cfgProp2 -> resolution = (int) pow(2.0, PROP_PWM_BIT);
        cfgProp2 -> valMin = (int) (PROP_PWM_MIN * cfgProp2 -> resolution);
        cfgProp2 -> valMax = (int) (PROP_PWM_MAX * cfgProp2 -> resolution);
        ledcSetup(cfgProp2 -> ch, cfgProp1 -> frequencyHz, PROP_PWM_BIT);
        ledcAttachPin(cfgProp2 -> pin, cfgProp2 -> ch);

        changeAngle(10.0);
        changePropSpeed(10.0);
        delay(500);

        changeAngle(-10.0);
        changePropSpeed(-10.0);
        delay(500);

        
    }

    ESP32Boat::~ESP32Boat()
    {
        free(cfgServo);
        free(cfgProp1);
        free(cfgProp2);
    }

    void ESP32Boat::changePropSpeed(float speed_percent)
    {
        auto cfgProp = (speed_percent > 0) ? cfgProp2 : cfgProp1;
        auto gnd = (speed_percent > 0) ? cfgProp1 : cfgProp2;
        speed_percent = abs(speed_percent);
        speed_percent = (speed_percent > 100) ? 100 : speed_percent;
        uint32_t duty = (uint32_t) speed_percent * cfgProp -> resolution * 0.01;
        ledcWrite(cfgProp -> ch, duty);
        ledcWrite(gnd -> ch, 0);
    }

    void ESP32Boat::changeAngle(float angle_deg)
    {
        angle_deg = (angle_deg > SERVO_MAX_ANGLE) ? SERVO_MAX_ANGLE : angle_deg;
        angle_deg = (angle_deg < SERVO_MIN_ANGLE) ? SERVO_MIN_ANGLE : angle_deg;
        uint32_t duty = (uint32_t) ((angle_deg * SERVO_ANGLE2RANGE + 0.0625) * cfgServo -> resolution);
        ledcWrite(cfgServo -> ch, duty);

        #ifdef DISP_SERIAL
        Serial.println(angle_deg);
        Serial.println(duty);
        #endif
    }
} // namespace ESP32Boat

#endif
