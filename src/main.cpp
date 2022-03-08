#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <SPI.h>
#include <NTPClient.h>
#include <Wire.h>
#include <DS1307.h>
#include <MFRC522.h>
#include <U8g2lib.h>
#include <LittleFS.h>
//#include <FreeRTOS.h>

#define N 10
#define SS_PIN 15
#define RST_PIN 2
#define BUTTON 0

//Club
const char *ssid1 = "SCU_Makers";
const char *passwd1 = "iloveSCU";
//Room
const char *ssid2 = "Texas_Wifi";
const char *passwd2 = "96587431tex";
//home
const char *ssid3 = "AEHOME";
const char *passwd3 = "20196666";


std::vector<unsigned long> id{  0x4933058F,
                                0xF3888202,
                                0x731A0D03,
                                0xA35C3102,
                                0xA3B5F20C,
                                0x8350D90A,
                                0x531CFA0A,
                                0x33206D0C  };
//----------------------------------------Unit_Set-------------------------------------------------

/*OLED*/
U8G2_SSD1306_64X48_ER_1_HW_I2C u8g2( U8G2_R0, U8X8_PIN_NONE );

/*Wifi*/
WiFiUDP ntpUDP;
ESP8266WiFiMulti wiFiMulti;
ESP8266WebServer server(80);

/*RTC*/
DS1307 rtc;
NTPClient tc(ntpUDP, "ntp1.aliyun.com", 60 * 60 * 8, 30 * 60 * 1000);

/*FS*/
File uid;
String time_file = "/index.html";

/*RC522*/
MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
byte nuidPICC[4];

//---------------------------------------Function---------------------------------------------------

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

    u8g2.setFont( u8g2_font_crox3hb_tf );
    u8g2.firstPage();
    do
    {
        u8g2.drawStr(0, 25, hourStr);
        u8g2.drawStr(18, 25, ":");
        u8g2.drawStr(23, 25, minStr);
        u8g2.drawStr(41, 25, ":");
        u8g2.drawStr(45, 25, secStr);

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

void printDec(byte *buffer, byte bufferSize)
{
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

    unsigned long UID = getID();
    std::vector<unsigned long>::iterator it = std::find(id.begin(),id.end(),UID);

    if ((rfid.uid.uidByte[0] != nuidPICC[0] ||
         rfid.uid.uidByte[1] != nuidPICC[1] ||
         rfid.uid.uidByte[2] != nuidPICC[2] ||
         rfid.uid.uidByte[3] != nuidPICC[3]) && it != id.end() )
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
        uid = LittleFS.open(time_file , "a" );
        uid.print("No:");
        uid.print( std::distance(id.begin() , it ) + 1 , DEC );
        uidtime_datas( hour , min , sec );
        uid.println("</br>");
        uid.close();

        u8g2.setFont(u8g2_font_4x6_tf );
        u8g2.firstPage();
        do
        {
            u8g2.drawStr(0 , 25 , "Card had been ");
            u8g2.drawStr( 20 , 30 , " read") ;
        }while (u8g2.nextPage());
    }
    else
    {
        Serial.println(F("Card read previously,or the card can't be read"));
        u8g2.setFont(u8g2_font_4x6_tf );
        u8g2.firstPage();
        do
        {
            u8g2.drawStr(16 , 20 , "Card can't be");
            u8g2.drawStr( 16 , 35 , " read ");
        }while (u8g2.nextPage());
        return;
    }
}

void webPage()
{
    Serial.println("webPage");
    File file = LittleFS.open("/index.html", "r");
    size_t sent = server.streamFile(file, "text/html");
    file.close();
    return;
}

void WiFi_Start()/*NTP request && WiFi*/
{
    int count = 0;
    wiFiMulti.addAP(ssid1,passwd1);
    wiFiMulti.addAP(ssid2,passwd2);
    wiFiMulti.addAP(ssid3,passwd3);

    while ( wiFiMulti.run() != WL_CONNECTED && count <= 20 )
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
void u8g2_start()/*U8g2 : SSD1306*/
{
    pinMode(D1, OUTPUT);
    pinMode(D2, OUTPUT);
    digitalWrite(D1, 0);
    digitalWrite(D2, 0);
    u8g2.begin();
}
void LittleFS_Start()/*LittleFS*/
{
    if( LittleFS.begin() )
    {
        LittleFS.format();
        Serial.println("LittleFS Start");
        uid = LittleFS.open(time_file , "a");
        uid.println("Strat write:</br>");
        uid.close();
    }
    else
    {
        Serial.println("Failed to start LittleFS");
    }
}
/*Test Function*/

/*String getContentType(String filename)
{
    if(server.hasArg("download")) return "application/octet-stream";
    else if(filename.endsWith(".htm")) return "text/html";
    else if(filename.endsWith(".html")) return "text/html";
    else if(filename.endsWith(".css")) return "text/css";
    else if(filename.endsWith(".js")) return "application/javascript";
    else if(filename.endsWith(".png")) return "image/png";
    else if(filename.endsWith(".gif")) return "image/gif";
    else if(filename.endsWith(".jpg")) return "image/jpeg";
    else if(filename.endsWith(".ico")) return "image/x-icon";
    else if(filename.endsWith(".xml")) return "text/xml";
    else if(filename.endsWith(".pdf")) return "application/x-pdf";
    else if(filename.endsWith(".zip")) return "application/x-zip";
    else if(filename.endsWith(".gz")) return "application/x-gzip";
    return "text/plain";
}*/

/*void handleNotFound()
{
    String path = server.uri();
    Serial.print("load url:");
    Serial.println(path);
    String contentType = getContentType(path);
    String pathWithGz = path + ".gz";

    if(LittleFS.exists(pathWithGz) || LittleFS.exists(path))
    {
        if(LittleFS.exists(pathWithGz))
            path += ".gz";
        File file = LittleFS.open(path, "r");
        size_t sent = server.streamFile(file, contentType);
        file.close();
        return;
    }

    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for ( uint8_t i = 0; i < server.args(); i++ ) {
        message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
    }
    server.send ( 404, "text/plain", message );
}*/

/*void list_file()
{
    uid = LittleFS.open(time_file , "r" );

    if( !uid )
    {
        Serial.println("Can't read the file!");
        u8g2.setFont( u8g2_font_4x6_tr );
        u8g2.firstPage();
        do
        {
            u8g2.drawStr(0 , 10 , "Can't read the file!");
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
        delay(200);
    }
}*/

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
//----------------------------------------Main_Frame-------------------------------------------------

void setup()
{
    Serial.begin(9600);
    SPI.begin(); // Init SPI bus
    rfid.PCD_Init(); // Init MFRC52

    u8g2_start();

    WiFi_Start();

    LittleFS_Start();

    /*WEB Server*/
    server.on("/" , webPage);
    server.onNotFound([]() {server.send( 404 , "text/plain" , "File not found" );});
    server.begin();
    Serial.println("HTTP Server Start");
}

void loop()
{
    uint8_t sec, min, hour, day, month;
    uint16_t year;
    wiFiMulti.run();

    //Process the web request
    server.handleClient();

    rtc.get( &sec, &min, &hour, &day, &month, &year );// get time from RTC

    u8g2Print_day( hour, min, sec );// OLED print

    if ( digitalRead(BUTTON) == LOW )
    {
        //list_file();  //Use in test version
        Serial.println("File Send");
        Serial.println(WiFi.localIP());
        delay(1000);
    }

    // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
    if ( ! rfid.PICC_IsNewCardPresent())
        return;

    // Verify if the NUID has been readed
    if ( ! rfid.PICC_ReadCardSerial())
        return;

    write_data( hour , min , sec );

    // Halt PICC
    rfid.PICC_HaltA();

    // Stop encryption on PCD
    rfid.PCD_StopCrypto1();

    delay(2000);
}

/*Next version for FreeRTOS*/
/*
void taskMain( void* taskMain )
{
    uint8_t sec, min, hour, day, month;
    uint16_t year;
    while(1)
    {
        wiFiMulti.run();

        rtc.get( &sec, &min, &hour, &day, &month, &year );// get time from RTC

        // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
        if ( ! rfid.PICC_IsNewCardPresent())
            return;

        // Verify if the NUID has been readed
        if ( ! rfid.PICC_ReadCardSerial())
            return;

        write_data( hour , min , sec );

        // Halt PICC
        rfid.PICC_HaltA();

        // Stop encryption on PCD
        rfid.PCD_StopCrypto1();
    }
}

void taskButtom( void* subtask )
{
    Serial.begin(9600);
    pinMode(0 , INPUT_PULLUP);
    while (1)
    {
        if ( digitalRead(BUTTON) == LOW )
        {
            //list_file();  //Use in test version
            Serial.println("File Send");
            Serial.println(WiFi.localIP());
            vTaskDelay( 1000 , portTICK_PERIOD_MS );
        }
    }
};
*/