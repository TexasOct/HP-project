//
// Created by texas on 12/04/2022.
//

#ifndef MAIN_CPP_RC522_H
#define MAIN_CPP_RC522_H

#include <vector>

void RFID_start();

void write_data(int hour , int min , int sec , std::vector<unsigned long> &id );

void detect_card();

void Halt_Detect();

#endif //MAIN_CPP_RC522_H
