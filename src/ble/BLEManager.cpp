/*
 * SPDX-FileCopyrightText: 2025 Braveridge co., ltd.
 * SPDX-License-Identifier: MIT
 */

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLE2902.h>

#include "BLEManager.h"

#define BLE_MANAGER_SERVICE_UUID				"833bbc00-588e-4ca2-9cd3-717200016c6c"
#define BLE_MANAGER_CHARACTERISTIC_RID_UUID		"833bbc01-588e-4ca2-9cd3-717200016c6c"
#define BLE_MANAGER_CHARACTERISTIC_VERSION_UUID	"833bbc02-588e-4ca2-9cd3-717200016c6c"

class BvBLEServerCallbacks : public BLEServerCallbacks {
    BLEManager *pBLEManager = nullptr;

    void onConnect(BLEServer *pBLEServer) override {
        Serial.println("BLE connected");
        pBLEServer->getAdvertising()->stop();
        pBLEManager->isConnected = true;
    };

    void onDisconnect(BLEServer *pBLEServer) override {
        Serial.println("BLE disconnected");
        pBLEServer->getAdvertising()->start();
        pBLEManager->isConnected = false;
    }

public:
    BvBLEServerCallbacks(BLEManager *pBLEManager) : pBLEManager(pBLEManager) {
        this->pBLEManager = pBLEManager;
    }
};

class BvBLECharacteristicCallbacks : public BLECharacteristicCallbacks {
    BLEManager *pBLEManager = nullptr;
    uint8_t versionData[3];

    void onRead(BLECharacteristic *pCharacteristic){
        if (pCharacteristic == pBLEManager->pCharacteristicVersion) {
            pCharacteristic->setValue(versionData, sizeof(versionData));
        }
    }

    // void onWrite(BLECharacteristic *pCharacteristic) {}

public:
    BvBLECharacteristicCallbacks(BLEManager *pBLEManager, uint8_t versionData[3]) : pBLEManager(pBLEManager) {
        this->pBLEManager = pBLEManager;
        memcpy(this->versionData, versionData, sizeof(this->versionData));
    }
};

void BLEManager::startBLE(std::string pLocalName, uint8_t versionData[3]) {
    BLEDevice::init(pLocalName);
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new BvBLEServerCallbacks(this));

    BLEService *pServiceData = pServer->createService(BLE_MANAGER_SERVICE_UUID);
    pCharacteristicRid = pServiceData->createCharacteristic(
        BLE_MANAGER_CHARACTERISTIC_RID_UUID,
        BLECharacteristic::PROPERTY_NOTIFY);
    pCharacteristicRid->addDescriptor(new BLE2902());

    pCharacteristicVersion = pServiceData->createCharacteristic(
        BLE_MANAGER_CHARACTERISTIC_VERSION_UUID,
        BLECharacteristic::PROPERTY_READ);
    pCharacteristicVersion->setCallbacks(new BvBLECharacteristicCallbacks(this, versionData));
    pServiceData->start();

    pServer->getAdvertising()->addServiceUUID(BLE_MANAGER_SERVICE_UUID);
    pServer->getAdvertising()->start();
    Serial.println("Waiting BLE connection");
}

void BLEManager::notifyRidData(uint8_t *data, int length) {
    pCharacteristicRid->setValue(data, length);
    pCharacteristicRid->notify();
}
