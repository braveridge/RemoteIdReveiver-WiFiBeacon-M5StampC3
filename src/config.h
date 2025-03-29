/*
 * SPDX-FileCopyrightText: 2025 Braveridge co., ltd.
 * SPDX-License-Identifier: MIT
 */

#ifndef BVRR_M5_Config_h
#define BVRR_M5_Config_h

//  version
#define VERSION_DATA ((uint8_t[3]){1, 0, 0})

//  BLE
#define ADV_LOCAL_NAME  "BVRRM5C3"  //  BvRemoteId App scan this local name. less than 8 bytes

//  WiFi
#define WIFI_CHANNEL_MIN 1
#define WIFI_CHANNEL_MAX 14     // max Channel -> US = 11, EU = 13, Japan = 14

//  LED
#define LED_PIN  2
#define LED_DURATION 100
#define LED_INTERVAL 2000
#define LED_RGB_BLE_NOT_CONNECTED ((uint8_t[3]){0, 0, 255})
#define LED_RGB_BLE_CONNECTED ((uint8_t[3]){0, 100, 100})

#endif /* BVRR_M5_Config_h */
