// ======================================================================
// Turing Machine Simulator
// Chandler Klüser, 2024
// ======================================================================

#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef MINGW
#include <windows.h>
#include <locale.h>
#include <wchar.h>
#endif

#ifdef MINGW
const wchar_t* PROGRAM_NAME= L"Turing Machine Simulator v1.0\n";
const wchar_t* AUTHOR_NAME = L"Chandler Klüser, 2024\n"; 
#else
const char* PROGRAM_NAME= "Turing Machine Simulator v1.0\n";
const char* AUTHOR_NAME = "Chandler Klüser, 2024\n"; 
#endif
#ifdef OPENMP
const char* OPENMP_SUPPORT_MESSAGE = "OpenMP Support Enabled\n";
#endif
const char* VERSION = "v1.0\n";

uint8_t isVerbose   = 0;
uint8_t DTM_mode    = 0;
uint8_t NDTM_mode   = 0;
uint8_t firstAccept = 0;
uint8_t TM_defined  = 0;
Parser_t p;

void parseArgs(int argc, char *argv[]);
void testTM();

#endif