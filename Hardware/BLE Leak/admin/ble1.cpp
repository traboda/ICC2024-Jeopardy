//Author: br34dcrumb

#include "BLEDevice.h"
#include "BLEServer.h"
#include "BLEUtils.h"
#include "esp_gap_ble_api.h"
#include "BLE2902.h"
#include "Arduino.h"
#include "string.h"
#include "my_data.h"  // const char* myData; provided binary data
#include <stdint.h>
#include "bleanimation.h"

#define I2C_SDA_PIN 17
#define I2C_SCL_PIN 18

#define BLE_NAME "icc_hw_ble"

const int dataLength = 680;

int currentIndex = 0;
const int chunkSize = 20; //Size of each notif

bool deviceConnected = false;
bool notif = false;

extern CustomMatrix matrix;
BLECharacteristic* pCharacteristic3;

void slice(const char* src, char* dst, int start, int len) {
    memcpy(dst, src + start, len);
}

char* prepare_notification() {
    static char buffer[chunkSize];

    if (currentIndex < dataLength) {
        int remainingLength = dataLength - currentIndex;
        int len = (remainingLength > chunkSize) ? chunkSize : remainingLength;

        slice(myData, buffer, currentIndex, len);
        currentIndex += len;
        return buffer;
    } else {
        Serial.println("No more data to send.");
        return nullptr;
    }
}

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        Serial.println("Device connected");
        deviceConnected = true;
    }

    void onDisconnect(BLEServer* pServer) {
        Serial.println("Device disconnected");
        deviceConnected = false;
    }
};

class MyCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pCharacteristic1) {
        String valueString = pCharacteristic1->getValue().c_str();

        for (size_t i = 0; i < valueString.length(); i++) {
            Serial.print((int)valueString[i], HEX);
            Serial.print(" ");
        }
        Serial.println();

        if (valueString.length() > 0) {
            Serial.println("Write detected, preparing notification...");

            char* notificationData = prepare_notification();
            if (notificationData != nullptr) {
                pCharacteristic3->setValue((uint8_t*)notificationData, chunkSize);
                pCharacteristic3->notify();
                Serial.println("Notification sent.");
                notif = true;
            }
        } else {
            Serial.println("No write detected");
        }
    }
};

void ble_main() {
    // Initialize BLE
    BLEDevice::init(BLE_NAME);
    BLEServer* pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    // First Service
    BLEService* pService1 = pServer->createService(BLEUUID("A81E5742-B8DB-424F-8102-8942A01CEEB2"));

    // First Characteristic - Used to trigger the notifications
    BLECharacteristic* pCharacteristic1 = pService1->createCharacteristic(
        BLEUUID("A81E5743-B8DB-424F-8102-8942A01CEEB2"),
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
    );
    pCharacteristic1->setCallbacks(new MyCallbacks());
    pCharacteristic1->addDescriptor(new BLE2902());
    pCharacteristic1->setValue("Write to trigger data notification.");
    pService1->start();  // Start the service

    // Second Service
    BLEService* pService2 = pServer->createService(BLEUUID("B12E5742-B8DB-424F-8102-8942A01CEEB2"));

    // Second Characteristic
    BLECharacteristic* pCharacteristic2 = pService2->createCharacteristic(
        BLEUUID("B12E5743-B8DB-424F-8102-8942A01CEEB2"),
        BLECharacteristic::PROPERTY_READ
    );
    // XXX: Change flag here
    pCharacteristic2->setValue("Secret: 24 29 87 7B 99 EC E9 B6 60 97 E4 0D A1 41 7E D8 DC 8B FC 22 FB E8 57 07 7C 6B 66 5A 45 5E 14");
    pService2->start(); // Start the service

    // Third Service for Notifications
    BLEService* pService3 = pServer->createService(BLEUUID("C23E5742-B8DB-424F-8102-8942A01CEEB2"));

    // Third Characteristic - Used for sending notifications
    pCharacteristic3 = pService3->createCharacteristic(
        BLEUUID("C23E5743-B8DB-424F-8102-8942A01CEEB2"),
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
    );
    pCharacteristic3->addDescriptor(new BLE2902());
    pCharacteristic3->setValue("Data notifications will be sent here.");
    pService3->start();  // Start the service

    // BLE Advertising
    BLEAdvertising* pAdvertising = pServer->getAdvertising();
    pAdvertising->addServiceUUID(pService1->getUUID());
    pAdvertising->addServiceUUID(pService2->getUUID());
    pAdvertising->addServiceUUID(pService3->getUUID());
    pAdvertising->start();

    Serial.println("BLE server with three services started. Waiting for connections...");

    matrix.autoplay_off();
    while (true) {
        if (!deviceConnected) {
            matrix.setTextSize(1);
            matrix.setTextWrap(false);
            matrix.setTextColor(100);
            for (int8_t x=16; x>=-67; x--) {
                matrix.clear();
                matrix.setCursor(x,0);
                matrix.print("Wassup BLE");
                delay(100);
            }
        }
        else {
            //Animation code for IS31FL3731
            if (!notif) {
                blink_eye(); delay(2000);
            }
            else {
                draw_frame(angry); delay(1000);
                notif = false;
            }
        }
    }

}

extern "C" {
    uint8_t ble1() {
        Serial.println("Starting BLE...");
        ble_main();
        return 0;
    }
}
