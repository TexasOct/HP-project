//
// Created by texas on 12/04/2022.
//

#ifndef MAIN_CPP_FSYS_H
#define MAIN_CPP_FSYS_H

#include <LittleFS.h>

void LittleFS_Start();

void FS_write(int hour , int min , int sec , int number);

void openHtml( File& file );

void clientHandle();

#endif //MAIN_CPP_FSYS_H
