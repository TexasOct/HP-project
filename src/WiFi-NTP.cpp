//
// Created by texas on 12/04/2022.
//

#include "../include/WiFi-NTP.h"
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <DS1307.h>


//Club
const char *ssid1 = "SCU_Makers";
const char *passwd1 = "iloveSCU";
//Room
const char *ssid2 = "Texas_Wifi";
const char *passwd2 = "96587431tex";
//home
const char *ssid3 = "AEHOME";
const char *passwd3 = "20196666";

/*Wifi*/
WiFiUDP ntpUDP;
ESP8266WiFiMulti wiFiMulti;


/*RTC*/
DS1307 rtc;
NTPClient tc(ntpUDP, "ntp1.aliyun.com", 60 * 60 * 8, 30 * 60 * 1000);


void WiFi_Start()//NTP request && WiFi
{
    int count = 0;
    wiFiMulti.addAP(ssid1,passwd1);
    wiFiMulti.addAP(ssid2,passwd2);
    wiFiMulti.addAP(ssid3,passwd3);

    while ( wiFiMulti.run() != WL_CONNECTED )
    {
        Serial.print(".");
        delay(500);
        count++;
    }

    if ( count <= 20  )
    {
        Serial.print("\nWIFI Start,the ip is : ");
        Serial.println( WiFi.localIP() );
        Serial.print( "The SSID is : " );
        Serial.println(WiFi.SSID() );
    }
    else if (count > 20)
    {
        Serial.println("\nWIFI isn't connect");
    }

    delay(1000);
    Serial.println("Init RTC...");
    rtc.begin();
    if ( count <= 30 )
    {
        tc.begin();
        tc.update();
    }
    rtc.set(int(tc.getSeconds()),
            int(tc.getMinutes()),
            int(tc.getHours()),
            int(tc.getDay()),
            0,
            0);// set time from NTPClient
    rtc.start(); // start RTC
}

void wifiMulti_run(){
    wiFiMulti.run();
}

void rtcGetTime(uint8_t &sec,  uint8_t &min, uint8_t &hour, uint8_t &day, uint8_t &month, uint16_t &year){
    rtc.get( &sec, &min, &hour, &day, &month, &year );// get time from RTC
}

void printWiFi(){
    Serial.print("Local IP : ");
    Serial.println(WiFi.localIP());
    delay(1000);
}

