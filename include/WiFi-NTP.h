//
// Created by texas on 12/04/2022.
//

#ifndef MAIN_CPP_WIFI_NTP_H
#define MAIN_CPP_WIFI_NTP_H

void WiFi_Start();

void wifiMulti_run();

void rtcGetTime(uint8_t &sec,  uint8_t &min, uint8_t &hour, uint8_t &day, uint8_t &month, uint16_t &year);

void printWiFi();

#endif //MAIN_CPP_WIFI_NTP_H
