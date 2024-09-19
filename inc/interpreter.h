// ======================================================================
// Turing Machine Simulator
// Chandler Klüser, 2024
// ======================================================================

#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stdio.h>
#include <stdlib.h>
#include <rules.h>

#define MAX_LINE_LENGTH 255

typedef struct {
    // State Origin Name
    uint8_t* current_state_name;
    // Read Symbol
    uint8_t read_symbol;
    // Write Symbol
    uint8_t write_symbol;
    // State Destination Name
    uint8_t* new_state_name;
    // Move of the Head of Turing Machine (Left, Right or Stick in Position)
    uint8_t head_move;
} MoveParser_t;

typedef struct {
    uint8_t** tapes;
    uint8_t* heads;
    uint8_t size;
} HeadParser_t;

typedef struct {
    Move_t* moves;
    uint8_t moves_size;
    State_t* states;
    uint8_t states_size;
} AutomataParser_t;

typedef struct {
    uint8_t** tapes;
    uint8_t tapes_size;
    uint8_t* heads;
    uint8_t head_size;
    uint8_t* initial_state;
    uint8_t** accept_states;
    uint8_t accept_states_size;
    MoveParser_t* move_parser;
    uint8_t mparser_size;
} Parser_t;

Parser_t parseFile(const int8_t *filename);
AutomataParser_t parserToAutomata(Parser_t p);
HeadParser_t parserToHeadParser(Parser_t p);
TM_t DTM(uint8_t* tape, uint8_t head,AutomataParser_t a);
void cleanBuffer(char* buffer, char* clean_buffer);
void wipeOffSubstring(char* input, char* output, const char* substring);
void countCommas(char* line_buffer,uint8_t* positions,uint8_t* count);
void errorLineMessage(uint32_t line_number);

#endif