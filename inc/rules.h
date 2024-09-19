// ======================================================================
// Turing Machine Simulator
// Chandler Klüser, 2024
// ======================================================================

#ifndef RULES_H
#define RULES_H

#include <stdint.h>
#include <string.h>

#define MOVE_LEFT     (uint8_t) 0
#define MOVE_RIGHT    (uint8_t) 1
#define MOVE_WAIT     (uint8_t) 2

#define STATE_INITIAL (uint8_t) 0
#define STATE_NORMAL  (uint8_t) 1
#define STATE_ACCEPT  (uint8_t) 2

#define STATUS_SGMOVE (uint8_t) 0
#define STATUS_ACCEPT (uint8_t) 1
#define STATUS_NOMOVE (uint8_t) 2
#define STATUS_MMOVES (uint8_t) 3

/// @brief Turing Machine Automaton State
typedef struct {
    /// @brief State name
    uint8_t* name;
    /// @brief whether if initial state, normal state or accept state...
    uint8_t type;
} State_t;

/// @brief TM Move Type
typedef struct {
    /// @brief State Origin Pointer
    State_t* current_state;
    /// @brief Read Symbol
    uint8_t read_symbol;
    /// @brief State Destination Pointer
    State_t* new_state;
    /// @brief Write Symbol
    uint8_t write_symbol;
    /// @brief Move of the Head of Turing Machine (Left, Right or Stick in Position)
    uint8_t head_move;
} Move_t;

/// @brief Turing Machine Simulation Type
typedef struct {
    /// @brief Tape String
    uint8_t* tape;
    /// @brief Tape Head Position
    uint8_t head;
    /// @brief Moves Pointer
    Move_t* moves;
    /// @brief Moves Array Length
    uint8_t moves_size;
    /// @brief States Pointer
    State_t* states;
    /// @brief States Array Length
    uint8_t states_size;
} TM_t;

/// @brief Valid Moves Found for NDTMs search
typedef struct {
    /// @brief Move_t* Base Pointer of Found Valid Moves
    /// NULL if there is no Valid Move
    Move_t** base;
    /// @brief Move_t Pointers Array Length
    /// zero if there is no Valid Move
    uint8_t length;
} ValidMoves_t;

/// @brief Status from a NDTM Step
typedef struct {
    /// @brief Valid Moves object with 
    ValidMoves_t valid_moves;
    /// @brief uint8_t status code of actual status of NDTM.
    /// Similar of uint8_t return of runStepTM:
    /// - 0, if NDTM is in a Single deterministic valid state transition for NDTM
    /// - 1, if NDTM is in a Valid Accept State
    /// - 2, if NDTM stops
    /// - 3, if NDTM finds two or more Valid Moves. If so, valid_moves have these moves
    uint8_t status;
} StatusNDTM_t;

void moveHead(uint8_t* head,uint8_t head_move);
Move_t* findValidMove(uint8_t* string, uint8_t TM_str_head, Move_t* moves,uint8_t moves_size, State_t* current_state);
void appendTMTapeLeft(uint8_t** TM_str, uint8_t** TM_str_head, uint8_t* TM_str_size);
void appendTMTapeRight(uint8_t** TM_str, uint8_t** TM_str_head, uint8_t* TM_str_size);
uint8_t runStepTM(uint8_t** TM_str, uint8_t* TM_str_head, uint8_t* TM_str_size, State_t* TM_states, State_t** state_head, uint8_t states_size, Move_t* moves,uint8_t moves_size);

#endif