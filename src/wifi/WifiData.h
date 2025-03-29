/*
 * SPDX-FileCopyrightText: 2025 Braveridge co., ltd.
 * SPDX-License-Identifier: MIT
 */

#ifndef BVRR_M5_WifiData_h
#define BVRR_M5_WifiData_h

#pragma pack(push, 1)
typedef struct {
	uint16_t frameControl;
	uint16_t duration = 0;
	uint8_t destinationAddress[6];
	uint8_t sourceAddress[6];
	uint8_t bssid[6];
	uint16_t fragmentNum:4;
	uint16_t sequenceNum:12;
	uint64_t timestamp;
	uint16_t beaconInterval;
	uint16_t capabilityInfo;
	uint8_t payload[0];
} WifiMacHeader;

typedef struct {
	uint8_t id;
	uint8_t length;
	uint8_t payload[0];
} WiFiElementData;
#pragma pack(pop)

inline bool isOuiRemoteId(vendor_ie_data_t *vi) {
	return vi->vendor_oui[0] == 0xFA && vi->vendor_oui[1] == 0x0B && vi->vendor_oui[2] == 0xBC && vi->vendor_oui_type == 0x0D;
}

#endif // BVRR_M5_WifiData_h
