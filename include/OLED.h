//
// Created by texas on 12/04/2022.
//

#ifndef MAIN_CPP_OLED_H
#define MAIN_CPP_OLED_H

void u8g2_start();

void u8g2Print_day(int hour, int min, int sec );

void u8g2_callback_display_get();

void u8g2_callback_display_unread();

#endif //MAIN_CPP_OLED_H
