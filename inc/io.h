// ======================================================================
// Turing Machine Simulator
// Chandler Klüser, 2024
// ======================================================================

#ifndef IO_H
#define IO_H

#include <rules.h>
#include <stdio.h>

void printTape(uint8_t* TM_string,uint8_t TM_head);
void printTapeNum(uint8_t* TM_string,uint8_t TM_head,uint8_t TM_num);
void printTMStatus(uint8_t stepStatus);
void printTMStatusNum(uint8_t stepStatus,uint8_t TM_num);

#endif