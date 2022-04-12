//
// Created by texas on 12/04/2022.
//

#include "../include/OLED.h"
#include <U8g2lib.h>
#include <SPI.h>
#include <hal.h>
#define N 10

U8G2_SSD1306_64X48_ER_1_HW_I2C u8g2( U8G2_R0, U8X8_PIN_NONE );

void u8g2_start()/*U8g2 : SSD1306*/
{
    pinMode(D1, OUTPUT);
    pinMode(D2, OUTPUT);
    digitalWrite(D1, 0);
    digitalWrite(D2, 0);
    u8g2.begin();
}

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

void u8g2Print_day(int hour, int min, int sec )
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

void u8g2_callback_display_get(){
    u8g2.setFont(u8g2_font_4x6_tf );
    u8g2.firstPage();
    do
    {
        u8g2.drawStr(0 , 25 , "Card had been ");
        u8g2.drawStr( 20 , 30 , " read") ;
    }while (u8g2.nextPage());
}

void u8g2_callback_display_unread(){
    u8g2.setFont(u8g2_font_4x6_tf );
    u8g2.firstPage();
    do
    {
        u8g2.drawStr(16 , 20 , "Card can't be");
        u8g2.drawStr( 16 , 35 , " read ");
    }while (u8g2.nextPage());
}