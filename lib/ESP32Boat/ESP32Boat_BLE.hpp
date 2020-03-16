#ifndef ESP32BOAT_BLE_HPP
#define ESP32BOAT_BLE_HPP

#include <BLEServer.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include <string.h>

#define DEVICE_NAME  "ESP32Boat"
#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

namespace ESP32Boat_BLE
{
    float received_speed = 0.0f;
    float received_angle = 0.0f;

    bool deviceConnected = false;
    bool oldDeviceConnected = false;

    bool isUpdated = false;

    float speed, angle;
    
    class ServerCallbacks : public BLEServerCallbacks
    {
        void onConnect(BLEServer* pServer)
        {
            deviceConnected = true;
        }

        void onDisconnect(BLEServer *pServer)
        {
            deviceConnected = false;
        }
    };

    class CharacteristicCallbacks : public BLECharacteristicCallbacks
    {
        void onWrite(BLECharacteristic *pCharacteristic)
        {
            uint8_t *pBuffer = pCharacteristic -> getData();
            switch (pBuffer[0])
            {
            case 0:
                memcpy(&speed, &pBuffer[1], 4);
                memcpy(&angle, &pBuffer[5], 4);

                received_speed = speed;
                received_angle = angle;

                isUpdated = true;

                #ifdef DISP_BLE
                Serial.print("[Control] ");
                Serial.print(speed);
                Serial.print(", ");
                Serial.println(angle);
                #endif
                break;
            
            default:
                break;
            }
        }
    };

    class ESP32Boat_BLE
    {
    private:
        BLEServer *pServer;
        BLECharacteristic *pTxCharacteristic;
        uint8_t txValue = 0;

    public:
        ESP32Boat_BLE(/* args */);
        ~ESP32Boat_BLE();
    };
    
    ESP32Boat_BLE::ESP32Boat_BLE(/* args */)
    {
        #ifdef DISP_BLE
        Serial.begin(115200);
        #endif

        BLEDevice::init(DEVICE_NAME);
        pServer = BLEDevice::createServer();
        pServer -> setCallbacks(new ServerCallbacks());

        BLEService *pService = pServer -> createService(SERVICE_UUID);

        pTxCharacteristic = pService -> createCharacteristic(
            CHARACTERISTIC_UUID_TX,
            BLECharacteristic::PROPERTY_NOTIFY
        );
        pTxCharacteristic -> addDescriptor(new BLE2902());

        BLECharacteristic *pRxCharacteristic = pService -> createCharacteristic(
            CHARACTERISTIC_UUID_RX,
            BLECharacteristic::PROPERTY_WRITE
        );
        pRxCharacteristic -> setCallbacks(new CharacteristicCallbacks());

        pService -> start();

        pServer -> getAdvertising() -> start();

    }
    
    ESP32Boat_BLE::~ESP32Boat_BLE()
    {
    }
    
}

#endif