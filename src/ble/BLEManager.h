/*
 * SPDX-FileCopyrightText: 2025 Braveridge co., ltd.
 * SPDX-License-Identifier: MIT
 */

#ifndef BVRR_M5_BLEManager_h
#define BVRR_M5_BLEManager_h

#include <BLEServer.h>

class BLEManager {
public:
    bool isConnected = false;
    BLEServer *pServer;
    BLECharacteristic *pCharacteristicRid;
    BLECharacteristic *pCharacteristicVersion;

    void startBLE(std::string pLocalName, uint8_t versionData[3]);
    void notifyRidData(uint8_t *data, int length);
};

#endif // BVRR_M5_BLEManager_h
