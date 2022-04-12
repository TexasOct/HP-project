//
// Created by texas on 12/04/2022.
//

#include "FSys.h"
#include <LittleFS.h>

File uid;
String time_file = "/index.html";

void LittleFS_Start()/*LittleFS*/
{
    if( LittleFS.begin() )
    {
        LittleFS.format();
        Serial.println("LittleFS Start");
        uid = LittleFS.open(time_file , "a");
        uid.println("Start write:</br>");
        uid.close();
    }
    else
    {
        Serial.println("Failed to start LittleFS");
    }
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

void FS_write(int hour , int min , int sec , std::vector<unsigned long> id ,  ){
    uid = LittleFS.open(time_file , "a" );
    uid.print("No:");
    uid.print( std::distance(id.begin() , it ) + 1 , DEC );
    uidtime_datas( hour , min , sec );
    uid.println("</br>");
    uid.close();
}

void openHtml( File& file )
{
    file = LittleFS.open("/index.html", "a");
}