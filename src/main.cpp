#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <SPI.h>
#include <NTPClient.h>
#include <Wire.h>
#include <DS1307.h>
#include <MFRC522.h>
#include <U8g2lib.h>
#include <LittleFS.h>

#define N 10
#define SS_PIN 15
#define RST_PIN 2
#define BUTTON 0
int temp = 0;

//Club
const char *ssid = "SCU_Makers";
const char *passwd = "iloveSCU";
//Room
/*const char *ssid = "Texas_Wifi";
const char *passwd = "96587431tex";*/
//home
/*const char *ssid = "AEHOME";
const char *passwd = "20196666";*/

//----------------------------------------Unit_Set-------------------------------------------------

/*OLED*/
U8G2_SSD1306_64X48_ER_1_HW_I2C u8g2( U8G2_R0, U8X8_PIN_NONE );

/*Wifi*/
WiFiUDP ntpUDP;

/*RTC*/
DS1307 rtc;
NTPClient tc(ntpUDP, "ntp1.aliyun.com", 60 * 60 * 8, 30 * 60 * 1000);

/*FS*/
File uid;
String time_file = "/dir/time.txt";

/*RC522*/
MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
byte nuidPICC[4];

//---------------------------------------Function---------------------------------------------------

/*void SerialTest( int hour, int min, int sec, int day )
{

    Serial.print("\nTime: ");

    if (hour <= 9)
    {
        Serial.print("0");
        Serial.print(hour, DEC);
    }
    else
    {
        Serial.print(hour, DEC);
    }

    Serial.print(":");

    if (min <= 9)
    {
        Serial.print("0");
        Serial.print(min, DEC);
    }
    else
    {
        Serial.print(min, DEC);
    }

    Serial.print(":");

    if (sec <= 9)
    {
        Serial.print("0");
        Serial.println(sec, DEC);
    }
    else
    {
        Serial.println(sec, DEC);
    }
    //delay( 500 );
}*/

void int_to_Str(int num, char *Str)
{//Trans the int to str that u8g2 could to strDraw
    char str[N] = {'\0'};
    if (num <= 9)
    {
        Str[0] = '0';
        itoa( num , str , 10 );
        strcat( Str , str );
    }
    else
    {
        itoa( num , Str , 10 );
    }
}

void u8g2Print_day(int hour, int min, int sec)
{

    char secStr[N] = {'\0'};
    char minStr[N] = {'\0'};
    char hourStr[N] = {'\0'};

    int_to_Str( hour , hourStr );
    int_to_Str( min , minStr );
    int_to_Str( sec , secStr );

    u8g2.setFont( u8g2_font_4x6_tf );
    u8g2.firstPage();
    do
    {
        u8g2.drawStr(0, 30, hourStr);
        u8g2.drawStr(18, 30, ":");
        u8g2.drawStr(23, 30, minStr);
        u8g2.drawStr(41, 30, ":");
        u8g2.drawStr(45, 30, secStr);

    }while (u8g2.nextPage());

}

void uidtime_datas( int hour, int min, int sec )
{
    uid.print(" Time: ");

    if (hour <= 9)
    {
        uid.print("0");
        uid.print(hour, DEC);
    }
    else
    {
        uid.print(hour, DEC);
    }

    uid.print(":");

    if (min <= 9)
    {
        uid.print("0");
        uid.print(min, DEC);
    }
    else
    {
        uid.print(min, DEC);
    }

    uid.print(":");

    if (sec <= 9)
    {
        uid.print("0");
        uid.print(sec, DEC);
    }
    else
    {
        uid.print(sec, DEC);
    }
}

void printHex(byte *buffer, byte bufferSize)
{
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}

void printDec(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], DEC);
    }
}

unsigned long getID()
{//get card uid from SPI
    unsigned long hex_num;
    hex_num  = rfid.uid.uidByte[0] << 24;
    hex_num += rfid.uid.uidByte[1] << 16;
    hex_num += rfid.uid.uidByte[2] << 8;
    hex_num += rfid.uid.uidByte[3];
    return hex_num;
}

void write_data( int hour , int min , int sec )
{
    if (rfid.uid.uidByte[0] != nuidPICC[0] ||
        rfid.uid.uidByte[1] != nuidPICC[1] ||
        rfid.uid.uidByte[2] != nuidPICC[2] ||
        rfid.uid.uidByte[3] != nuidPICC[3] )
    {
        Serial.println(F("A new card has been detected."));

        // Store NUID into nuidPICC array
        for (byte i = 0; i < 4; i++) {
            nuidPICC[i] = rfid.uid.uidByte[i];
        }

        Serial.println(F("The NUID tag is:"));
        Serial.print(F("In hex: "));
        printHex(rfid.uid.uidByte, rfid.uid.size);
        Serial.println();
        Serial.print(F("In dec: "));
        printDec(rfid.uid.uidByte, rfid.uid.size);
        Serial.println();
    }
    else
    {
        Serial.println(F("Card read previously."));
        return;
    }

    unsigned long UID = getID();
    uid = LittleFS.open(time_file , "a" );
    uid.print("UID:");
    uid.print( UID , HEX );
    uidtime_datas( hour , min , sec );
    uid.println();
    uid.close();

    u8g2.setFont(u8g2_font_4x6_tf );
    u8g2.firstPage();
    do
    {
        u8g2.drawStr(0, 30, "Card had been read");
    }while (u8g2.nextPage());
}

void list_file()
{
    uid = LittleFS.open(time_file , "r" );

    if( !uid )
    {
        Serial.println("Can't read the file!");
        u8g2.setFont(u8g2_font_6x12_me);
        u8g2.firstPage();
        do
        {
            u8g2.drawStr(0, 30, "Can't read the file!");
        }while (u8g2.nextPage());
        delay(2000);
        return ;
    }
    else
    {
        while ( uid.available() )
        {
            Serial.write( uid.read() );
        }
        Serial.print("\n");
        uid.close();
    }
}

//----------------------------------------Main_Frame-------------------------------------------------

void setup()
{
    Serial.begin(9600);
    SPI.begin(); // Init SPI bus
    rfid.PCD_Init(); // Init MFRC520

     /*U8g2 : SSD1306*/
    pinMode(D1, OUTPUT);
    pinMode(D2, OUTPUT);
    digitalWrite(D1, 0);
    digitalWrite(D2, 0);
    u8g2.begin();

    /*RTC & NTP*/
    WiFi.begin(ssid, passwd);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWIFI Start");
    delay(1000);
    Serial.println("Init RTC...");
    rtc.begin();
    tc.begin();
    tc.update();
    rtc.set(int(tc.getSeconds()),
            int(tc.getMinutes()),
            int(tc.getHours()),
            int(tc.getDay()),
            0,
            0);// set time from NTPClient
    rtc.start(); // start RTC

    /*LittleFS*/
    if( LittleFS.begin() )
    {
        Serial.println("LittleFS Start");
    }else
    {
        Serial.println("Failed to start LittleFS");
    }
    LittleFS.format();
}

void loop()
{
    uint8_t sec, min, hour, day, month;
    uint16_t year;

    rtc.get( &sec, &min, &hour, &day, &month, &year );// get time from RTC

    u8g2Print_day( hour, min, sec );// OLED print

    // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
    if ( ! rfid.PICC_IsNewCardPresent())
        return;

    // Verify if the NUID has been readed
    if ( ! rfid.PICC_ReadCardSerial())
        return;

    write_data( hour , min , sec );

    list_file();

    // Halt PICC
    rfid.PICC_HaltA();

    // Stop encryption on PCD
    rfid.PCD_StopCrypto1();

    delay(2000);
}