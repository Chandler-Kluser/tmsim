// ======================================================================
// Turing Machine Simulator
// Chandler Klüser, 2024
// ======================================================================

#include <stddef.h>
#include <stdlib.h>
#include <rules.h>

/// @brief Simple, but necessary void function that moves a TM Tape
/// @param head         TM Tape String
/// @param head_move    TM Move: 0, 1 or 2
void moveHead(uint8_t* head,uint8_t head_move) {
    switch (head_move) {
    case MOVE_LEFT:
        (*head)--;
        break;
    case MOVE_RIGHT:
        (*head)++;
        break;
    // MOVE_WAIT elsewhere (does nothing)
    default:
        break;
    }
}

/// @brief Returns valid move index in moves array given a string, its head and TM states not valid for non-deterministic
/// TM because it only returns a single move, not an array
/// @param string           DTM Tape String
/// @param TM_str_head      DTM Tape Head
/// @param moves            DTM Moves Array
/// @param moves_size       DTM Moves Length
/// @param current_state    DTM Current State Pointer in Automata
/// @return                 Move_t* Pointing to Valid Move_t, NULL elsewhere
Move_t* findValidMove(uint8_t* string, uint8_t TM_str_head, Move_t* moves,uint8_t moves_size, State_t* current_state) {
    // moves array must have at least one element
    if (moves_size==0) return NULL;
    for (uint8_t i = 0;i<moves_size;i++) {
        if (moves[i].current_state==current_state) {
            if (moves[i].read_symbol==string[TM_str_head]) return &(moves[i]);
        }
    }
    // there is no move valid
    return NULL;
}

/// @brief Memory allocates one more character to tape string, shifts all tape characters one position right and
/// writes blank space in the first array element
/// @param TM_str       Tape String Pointer
/// @param TM_str_head  Tape Head Pointer
/// @param TM_str_size  Tape String Length
void appendTMTapeLeft(uint8_t** TM_str, uint8_t** TM_str_head, uint8_t* TM_str_size) { 
    // TO DO: pass size as a pointer so the tape size can be updated
    *TM_str = (uint8_t*)realloc(*TM_str, (*TM_str_size+1)*sizeof(uint8_t));
    // Shift existing values to the right
    for (int16_t i=*TM_str_size-1; i >= 0; i--) (*TM_str)[i+1]=(*TM_str)[i];
    // Add white space to first element
    (*TM_str)[0]=' ';
    // Finish array with 0
    (*TM_str)[*TM_str_size+1]=(uint8_t)0;
    // shift string head to the right
    (*TM_str_head)++;
    // Update TM string size
    (*TM_str_size)++;
}

/// @brief Memory allocates one more character to tape string and
/// writes blank space in the last array element
/// @param TM_str       Tape String Pointer
/// @param TM_str_head  Tape Head Pointer
/// @param TM_str_size  Tape String Length
void appendTMTapeRight(uint8_t** TM_str, uint8_t** TM_str_head, uint8_t* TM_str_size) {
    // TO DO: pass size as a pointer so the tape size can be updated
    *TM_str = (uint8_t*)realloc(*TM_str, (*TM_str_size+1)*sizeof(uint8_t));
    // Add white space to last element
    (*TM_str)[*TM_str_size]=' ';
    // Finish array with 0
    (*TM_str)[*TM_str_size+1]=(uint8_t)0;
    // Update TM string size
    (*TM_str_size)++;
    // Not necessary to move string head to the left
}

/// @brief Checks state type from a DTM. If running, moves head, changes its tape and goes to the next state
/// @param TM_str           DTM Tape String Pointer
/// @param TM_str_head      DTM Tape Head Pointer
/// @param TM_str_size      DTM Tape Length Pointer
/// @param TM_states        DTM States Pointer
/// @param state_head       DTM Current State Pointer
/// @param states_size      DTM States Length
/// @param moves            DTM Moves Pointer
/// @param moves_size       DTM Moves Length
/// @return                 0, if valid state transition.
///                         1, if DTM is in a Valid Accept State
///                         2, if DTM stops
uint8_t runStepTM(uint8_t** TM_str, uint8_t* TM_str_head, uint8_t* TM_str_size, State_t* TM_states, State_t** state_head, uint8_t states_size, Move_t* moves,uint8_t moves_size) {
    // Returns 1 if TM is in an Accept State
    if ((*state_head)->type==STATE_ACCEPT) return (uint8_t)1;
    Move_t* step_move = findValidMove(*TM_str,*TM_str_head,moves,moves_size,*state_head);
    // If no move valid, returns 2. Updates state head, elsewhere
    if (step_move==NULL) return (uint8_t)2; else *state_head = step_move->new_state;
    // replace old symbol from tape for new symbol
    (*TM_str)[*TM_str_head]=step_move->write_symbol;
    // moves head
    if (step_move->head_move==MOVE_LEFT  && *TM_str_head==0) appendTMTapeLeft(TM_str,&TM_str_head,TM_str_size);
    if (step_move->head_move==MOVE_RIGHT && *TM_str_head==*TM_str_size-1) appendTMTapeRight(TM_str,&TM_str_head,TM_str_size);
    // Updates String Head
    moveHead(TM_str_head,step_move->head_move);
    return (uint8_t)0;
}