/*
 * SPDX-FileCopyrightText: 2025 Braveridge co., ltd.
 * SPDX-License-Identifier: MIT
 */

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#include <esp_wifi.h>

#include "config.h"
#include "ble/BLEManager.h"
#include "wifi/WifiData.h"

Adafruit_NeoPixel pixels(1, LED_PIN, NEO_GRB + NEO_KHZ800);
bool ledState = LOW;
unsigned long ledPreviousMillis = 0;

BLEManager bleManager;
uint8_t currentWifiChannel = WIFI_CHANNEL_MIN;

void wifiReceiveCallback(void* buf, wifi_promiscuous_pkt_type_t type) {
	wifi_promiscuous_pkt_t *ppkt = (wifi_promiscuous_pkt_t*)buf;
	WifiMacHeader *mac = (WifiMacHeader *)ppkt->payload;
	WiFiElementData *elementData = (WiFiElementData *)mac->payload;
 	
	int len = ppkt->rx_ctrl.sig_len - sizeof(WifiMacHeader);
	if (len <= 0) return;

	while (len > 4) {
		switch (elementData->id) {
			case 0: // SSID
				if (elementData->length > MAX_SSID_LEN) {
					break;
				}

				// char ssid[MAX_SSID_LEN + 1];
				// strncpy(ssid, (char*)elementData->payload, elementData->length);
				// ssid[elementData->length] = 0;
				// printf("SSID = %s\n", ssid);
				break;

			case 221: {	// Vender Specific
				vendor_ie_data_t *vi = (vendor_ie_data_t *)elementData;

				// printf("Vendor OUI = %02X:%02X:%02X OUI Type = %2d\n",vi->vendor_oui[0],vi->vendor_oui[1],vi->vendor_oui[2],vi->vendor_oui_type);
				if (!isOuiRemoteId(vi)) {
					break;
				}

				// printf("Frame Ctl = %04X\n",mac->frameControl);
				// printf("Duration = %04X\n",mac->duration);
				// printf("DA    = %02X:%02X:%02X:%02X:%02X:%02X\n", mac->destinationAddress[0],mac->destinationAddress[1],mac->destinationAddress[2],mac->destinationAddress[3],mac->destinationAddress[4],mac->destinationAddress[5]);
				// printf("SA    = %02X:%02X:%02X:%02X:%02X:%02X\n",	mac->sourceAddress[0],mac->sourceAddress[1],mac->sourceAddress[2],mac->sourceAddress[3],mac->sourceAddress[4],mac->sourceAddress[5]);
				// printf("BSSID = %02X:%02X:%02X:%02X:%02X:%02X\n", mac->bssid[0],mac->bssid[1],mac->bssid[2],mac->bssid[3],mac->bssid[4],mac->bssid[5]);
				// printf("Seq No. = %d, Frg No. = %d\n", mac->sequenceNum, mac->fragmentNum);
				// printf("Timestamp = %d\n",mac->timestamp);
				// printf("Interval = %dTU\n", mac->beaconInterval);	
				// printf("Cpability = %04X\n", mac->capabilityInfo);		
				// printf("Payload length = %d\n", len);

				int notifyDataSize = elementData->length + 4;
				uint8_t notifyData[notifyDataSize];

				// data format
				notifyData[0] = 0x01;	// notify data type for Bv RemoteID App
				notifyData[1] = ppkt->rx_ctrl.channel;	// wifi channel
				notifyData[2] = ppkt->rx_ctrl.rssi;		// wifi rssi
				notifyData[3] = elementData->length;	// rid data length
				memcpy(notifyData + 4, elementData->payload, elementData->length);	// rid data

				printf("\n");
				printf("notify data : ");
				for (int i = 0; i < notifyDataSize; i++){
					printf("%02X ", notifyData[i]);
				}
				printf("\n\n");

				bleManager.notifyRidData(notifyData, notifyDataSize);
				break;
			}

			default:
				break;
		}

		int dlen = 2 + elementData->length;
		len -= dlen;
		elementData = (WiFiElementData *)((uint8_t *)elementData + dlen);
	}
	
	WiFi.scanDelete();
}

void showLedIfNeed() {
	unsigned long currentMillis = millis();
	long interval = ledState == HIGH ? LED_DURATION : LED_INTERVAL;
	if (currentMillis - ledPreviousMillis < interval) {
		return;
	}

	ledPreviousMillis = currentMillis;
	ledState = !ledState;

	if (ledState == HIGH) {
		uint32_t color = bleManager.isConnected 
			? pixels.Color(LED_RGB_BLE_CONNECTED[0], LED_RGB_BLE_CONNECTED[1], LED_RGB_BLE_CONNECTED[2]) 
			: pixels.Color(LED_RGB_BLE_NOT_CONNECTED[0], LED_RGB_BLE_NOT_CONNECTED[1], LED_RGB_BLE_NOT_CONNECTED[2]);
		pixels.setPixelColor(0, color);
	} else {
		pixels.setPixelColor(0, pixels.Color(0, 0, 0));
	}
	pixels.show();
}

uint8_t getNextWifiChannel() {
	currentWifiChannel++;	
	if (currentWifiChannel > WIFI_CHANNEL_MAX) {
		currentWifiChannel = WIFI_CHANNEL_MIN;
	}

	return currentWifiChannel;
}

void setup() {
	Serial.begin(115200);

	//	LED
	pixels.begin();

	//	WiFi
	WiFi.mode(WIFI_STA);
	esp_wifi_set_promiscuous(true);
	esp_wifi_set_promiscuous_rx_cb(&wifiReceiveCallback);

	//	BLE
	uint8_t versionData[3] = {VERSION_DATA[0], VERSION_DATA[1], VERSION_DATA[2]};
	bleManager.startBLE(ADV_LOCAL_NAME, versionData);	
}

void loop() {
	showLedIfNeed();

	uint8_t wifiScanChannel = getNextWifiChannel();
	ESP_ERROR_CHECK(esp_wifi_set_channel(wifiScanChannel, WIFI_SECOND_CHAN_NONE));
	printf("wifi scan channel : %d\n", wifiScanChannel);	

	delay(500);
}
