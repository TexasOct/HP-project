#include <Arduino.h>

#include <SPI.h>
#include <Wire.h>

#include "OLED.h"
#include "RC522.h"
#include "FSys.h"
#include "WiFi-NTP.h"
#include "WebServer.h"
#include "hal.h"
//#include <FreeRTOS.h>


std::vector<unsigned long> id{  0x4933058F,
                                0xF3888202,
                                0x731A0D03,
                                0xA35C3102,
                                0xA3B5F20C,
                                0x8350D90A,
                                0x531CFA0A,
                                0x33206D0C  };

void setup()
{
    Serial.begin(9600);
    SPI.begin(); // Init SPI bus
    pinMode(0 , INPUT_PULLUP);

    RFID_start();

    u8g2_start();

    WiFi_Start();

    LittleFS_Start();

    webStart();
}

void loop()
{
    uint8_t sec, min, hour, day, month;
    uint16_t year;

    wifiMulti_run();

    clientHandle();

    rtcGetTime( sec, min, hour, day, month, year );// get time from RTC

    if ( digitalRead(BUTTON) == LOW )
    {
        printWiFi();
    }

    u8g2Print_day( hour, min, sec );// OLED print

    if (detect_card() == 1)
    {
        return;
    }

    write_data( hour , min , sec , id);

    Halt_Detect();

    delay(2000);
}