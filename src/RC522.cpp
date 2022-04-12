//
// Created by texas on 12/04/2022.
//

#include "../include/RC522.h"
#include "hal.h"
#include "FSys.h"
#include "OLED.h"
#include <MFRC522.h>


MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
byte nuidPICC[4];


void RFID_start(){
    rfid.PCD_Init(); // Init MFRC522
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

void detect_card(){
    // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
    if ( ! rfid.PICC_IsNewCardPresent())
        return;

    // Verify if the NUID has been readed
    if ( ! rfid.PICC_ReadCardSerial())
        return;
}

void write_data( int hour , int min , int sec , std::vector<unsigned long> &id )
{

    unsigned long UID = getID();
    auto it = std::find(id.begin(),id.end(),UID);

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
        FS_write();
        u8g2_callback_display_get();
    }
    else
    {
        Serial.println(F("Card read previously,or the card can't be read"));
        u8g2_callback_display_unread();
    }
}

void Halt_Detect(){
    // Halt PICC
    rfid.PICC_HaltA();

    // Stop encryption on PCD
    rfid.PCD_StopCrypto1();
}