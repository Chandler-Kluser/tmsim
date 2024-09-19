// ======================================================================
// Turing Machine Simulator
// Chandler Klüser, 2024
// ======================================================================

#include <string.h>
#include <ctype.h>
#include <interpreter.h>

const char *STR_TAPE_DEFINITION         = "tape=";
const char *STR_HEAD_DEFINITION         = "head=";
const char *STR_INITIAL_STATE           = "initial_state=";
const char *STR_ACCEPT_STATES           = "accept_states=";
const char *STR_COMMENT_MARK            = "//";
const char *INTERPRETER_ERROR_MESSAGE   = "Invalid Command in Line %i.\n";

/// @brief Reads a file in disk and parses what is written to a Parser_t object
/// in order to generate Turing Machines Simulations in later phases
/// Exits Program if it finds invalid syntax
/// @param filename Text File Path in Disk
/// @return Parser_t file with Turing Machine Variables to Assemble TMs
Parser_t parseFile(const int8_t *filename) {
    
    // reads text file
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file %s\n", filename);
        exit(1);
    }

    Parser_t parser;

    uint32_t line_number = 1;
    // line buffer with whitespaces
    char line[MAX_LINE_LENGTH];
    // line buffer without whitespaces
    char line_buffer[MAX_LINE_LENGTH];

    // defining Move Parser Array
    MoveParser_t* mparser;
    uint8_t mparser_size=0;
    uint8_t *tape_heads,tape_heads_index=0;
    uint8_t *initial_state_name,tape_string_name_index=0,**tape_string_names,hmove;
    // Parser pointers
    uint8_t *cstate_name,*nstate_name,rchar[1],wchar[1];
    uint8_t **accept_states_names;
    uint8_t accept_states_size=0;

    uint8_t initial_state_defined = 0,accept_states_defined = 0,tape_head_defined = 0,tape_defined = 0;

    // general purpose counters
    uint8_t i = 0, j = 0;

    // iterate over textfile lines
    while (fgets(line, sizeof(line), file) != NULL) {
        // Process each line here

        // Wipe Off Comments
        wipeOffSubstring(line, line, STR_COMMENT_MARK);
        
        // Ignore whitespaces
        // Every non-whitespace character will be copied
        cleanBuffer(line,line_buffer);

        // Checks for Empty Lines or Lines with whitespaces only
        if (line_buffer[0]==0) {
            line_number++; continue;
        }

        // Check Commands greater than 255 characters
        // not working
        i = 0;
        j = 0;
        for (uint8_t k = 0;k<255;k++) if(line_buffer[k]==0) j=1;
        if (j==0) {
            printf("Error in Line %i\n",line_number);
            printf("Command must not have more than 256 valid characters.\n");
            exit(1);
        }

        // Check for initial state pattern
        if (strncmp(line_buffer, STR_INITIAL_STATE,strlen(STR_INITIAL_STATE)) == 0) {
            if (strlen(line_buffer)==strlen(STR_INITIAL_STATE)) {
                printf(INTERPRETER_ERROR_MESSAGE,line_number);
                printf("Initial State must not be empty.\n");
                exit(1);
            }
            if (initial_state_defined) {
                printf("Initial State in Line %i redefinition.\n",line_number);
                free(initial_state_name);
            }
            initial_state_name = malloc((strlen(line_buffer)-strlen(STR_INITIAL_STATE)) * sizeof(uint8_t));
            strcpy(initial_state_name, line_buffer + strlen(STR_INITIAL_STATE));
            line_number++;
            initial_state_defined=1;
            continue;
        }

        // Check for accept states pattern
        if (strncmp(line_buffer, STR_ACCEPT_STATES,strlen(STR_ACCEPT_STATES)) == 0) {
            if (strlen(line_buffer)==strlen(STR_ACCEPT_STATES)) {
                printf(INTERPRETER_ERROR_MESSAGE,line_number);
                printf("Accept States Definition must not be empty.\n");
                exit(1);
            }
            
            // drop all accept states defined before
            if (accept_states_defined) {
                printf("Accept States in Line %i redefinition.\n",line_number);
                free(accept_states_names);
                accept_states_size=0;
            }

            // allocate memory
            accept_states_names = malloc((accept_states_size+1)*sizeof(uint8_t*));

            // sweep string right to left to find commas and parse accept states
            i = strlen(line_buffer)-strlen(STR_ACCEPT_STATES);
            while (i!=0) {
                if (line_buffer[i+strlen(STR_ACCEPT_STATES)-1] == ',') {
                    if (strlen(&line_buffer[i+strlen(STR_ACCEPT_STATES)])==0) {
                        printf(INTERPRETER_ERROR_MESSAGE,line_number);
                        printf("Accept States Definition must not be empty.\n");
                        exit(1);
                    }
                    accept_states_names[accept_states_size] = malloc(strlen(&line_buffer[i+strlen(STR_ACCEPT_STATES)]) + 1);
                    strcpy(accept_states_names[accept_states_size],&line_buffer[i+strlen(STR_ACCEPT_STATES)]);
                    accept_states_size++;
                    accept_states_names = realloc(accept_states_names,accept_states_size*sizeof(uint8_t*));
                    // Write 0 in comma mark to make string smaller
                    line_buffer[i+strlen(STR_ACCEPT_STATES)-1]=0;
                }
                i--;
            }
            if (strlen(&line_buffer[strlen(STR_ACCEPT_STATES)])==0) {
                printf(INTERPRETER_ERROR_MESSAGE,line_number);
                printf("Accept States Definition must not be empty.\n");
                exit(1);
            }
            accept_states_names[accept_states_size] = malloc(strlen(&line_buffer[strlen(STR_ACCEPT_STATES)]) + 1);
            strcpy(accept_states_names[accept_states_size],&line_buffer[strlen(STR_ACCEPT_STATES)]);
            accept_states_size++;
            line_number++;
            accept_states_defined=1;
            continue;
        }

        // Check for tape definition pattern
        if (strncmp(line_buffer, STR_TAPE_DEFINITION,strlen(STR_TAPE_DEFINITION)) == 0) {
            if (strlen(line_buffer)==strlen(STR_TAPE_DEFINITION)) {
                printf(INTERPRETER_ERROR_MESSAGE,line_number);
                printf("Tape String must not be empty.\n");
                exit(1);
            }
            if (tape_defined) {
                // printf("New Tape Definition in Line %i.\n",line_number);
                tape_string_names = realloc(tape_string_names,(++tape_string_name_index)*sizeof(uint8_t*));
                // free(tape_string_name);
            } else {
                tape_string_names = malloc((++tape_string_name_index)*sizeof(uint8_t*));
            }
            tape_string_names[tape_string_name_index-1] = malloc((strlen(line_buffer)-strlen(STR_TAPE_DEFINITION))*sizeof(uint8_t));
            strcpy(tape_string_names[tape_string_name_index-1], line_buffer + strlen(STR_TAPE_DEFINITION));
            line_number++;
            tape_defined=1;
            continue;
        }

        // Check for tape head definition pattern
        if (strncmp(line_buffer, STR_HEAD_DEFINITION,strlen(STR_HEAD_DEFINITION)) == 0) {
            uint8_t* hd_ptr = line_buffer + strlen(STR_HEAD_DEFINITION);
            if (*hd_ptr==0) {
                printf(INTERPRETER_ERROR_MESSAGE,line_number);
                printf("Tape Head Index invalid.\n");
                exit(1);
            }
            if (atoi(hd_ptr)>=strlen(tape_string_names[0])) {
                printf(INTERPRETER_ERROR_MESSAGE,line_number);
                printf("Tape Head Index must be lesser than Tape length.\n");
                printf("Take note that tape index is zero-based.\n");
                exit(1);
            }
            if (tape_head_defined) {
                // printf("New Tape Head Definition in Line %i.\n",line_number);
                tape_heads = realloc(tape_heads,(++tape_heads_index)*sizeof(uint8_t));
            } else {
                tape_heads = malloc((++tape_heads_index)*sizeof(uint8_t));
            }
            tape_heads[tape_heads_index-1]=atoi(hd_ptr);
            tape_head_defined=1;
            line_number++;
            continue;
        }

        // Initialize comma count for iteration line
        uint8_t comma_count;
        // Initialize string comma positions
        uint8_t comma_position[6];

        // count commas in buffer
        countCommas(line_buffer,comma_position,&comma_count);

        if (comma_count!=4) {
            errorLineMessage(line_number);
            exit(1);
        }

        // Command Lines here
        if (comma_position[3]-comma_position[2]!=2) {
            errorLineMessage(line_number);
            printf("Head movement must have a single character length.\n");
            exit(1);
        }
        uint8_t move_char = (uint8_t)line_buffer[comma_position[2]+1];
        if ((move_char!='<')&&(move_char!='>')&&(move_char!='-')) {
            errorLineMessage(line_number);
            printf("Head movement must be < (left), > (right) or - (no move).\n");
            exit(1);
        }

        // Valid Command Lines here
        // Parsing current state name string
        uint8_t nstate_name_size;
        cstate_name = malloc((comma_position[0]+1)*sizeof(uint8_t));
        for (uint8_t i = 0; i<comma_position[0];i++) cstate_name[i] = line_buffer[i];
        cstate_name[comma_position[0]]='\0';
        
        for (uint8_t aux = comma_position[3]+1; line_buffer[aux]!='\0'; aux++) nstate_name_size=aux-comma_position[3];
        // Parsing new state name string
        nstate_name = malloc((nstate_name_size+1)*sizeof(uint8_t));
        j=0;
        for (uint8_t i = comma_position[3]+1; line_buffer[i]!='\0';i++) {
            nstate_name[j] = line_buffer[i];
            j++;
        }
        nstate_name[nstate_name_size]='\0';

        // Parsing read character
        if (comma_position[1]-comma_position[0]!=2) rchar[0] = ' '; else rchar[0] = line_buffer[comma_position[0]+1];
        if (comma_position[2]-comma_position[1]!=2) wchar[0] = ' '; else wchar[0] = line_buffer[comma_position[1]+1];
        if ((uint8_t)line_buffer[comma_position[2]+1]=='<') hmove=MOVE_LEFT;  else
        if ((uint8_t)line_buffer[comma_position[2]+1]=='>') hmove=MOVE_RIGHT; else 
        hmove=MOVE_WAIT;

        if (mparser_size==0) {
            mparser = malloc((++mparser_size)*sizeof(Parser_t));
        } else {
            mparser = realloc(mparser,(++mparser_size)*sizeof(Parser_t));
        }
        mparser[mparser_size-1].current_state_name=cstate_name;
        mparser[mparser_size-1].new_state_name=nstate_name;
        mparser[mparser_size-1].read_symbol=rchar[0];
        mparser[mparser_size-1].write_symbol=wchar[0];
        mparser[mparser_size-1].head_move=hmove;

        line_number++;
    }
    fclose(file);
    parser.tapes=tape_string_names;
    parser.tapes_size=tape_string_name_index-1;
    parser.heads=tape_heads;
    parser.head_size=tape_heads_index-1;
    parser.initial_state =initial_state_name;
    parser.accept_states=accept_states_names;
    parser.accept_states_size=accept_states_size;
    parser.move_parser=mparser;
    parser.mparser_size=mparser_size;
    return parser;
}

/// @brief Checks Automata Definition from Parser_t given
/// and Allocates Memory to AutomataParser_t output, if valid
/// Parser_t p (input) still has memory allocated after function call!!!
/// @param p Parser_t input to be checked
/// @return AutomataParser_t object with Memory Allocated
AutomataParser_t parserToAutomata(Parser_t p) {
    // States
    uint8_t is_unique,states_size=0;
    uint8_t **unique_states;
    unique_states = malloc(1*sizeof(uint8_t*));
    // get all unique values of uint8_t** accept_states array
    for (uint8_t i = 0; i < p.mparser_size; i++) {
        // iterate over current state name
        is_unique=1;
        for (uint8_t j = 0; j < states_size; j++) {
            if (!strcmp(unique_states[j],p.move_parser[i].current_state_name)) {is_unique=0; break;}
        }
        if (is_unique) {
            unique_states=realloc(unique_states,(states_size+1)*sizeof(uint8_t*));
            unique_states[states_size]=malloc(strlen(p.move_parser[i].current_state_name+1)*sizeof(uint8_t));
            strcpy(unique_states[states_size],p.move_parser[i].current_state_name);
            states_size++;
        }
        // iterate over new state name
        is_unique=1;
        for (uint8_t j = 0; j < states_size; j++) {
            if (!strcmp(unique_states[j],p.move_parser[i].new_state_name)) {is_unique=0; break;}
        }
        if (is_unique) {
            unique_states=realloc(unique_states,(states_size+1)*sizeof(uint8_t*));
            unique_states[states_size]=malloc(strlen(p.move_parser[i].new_state_name+1)*sizeof(uint8_t));
            strcpy(unique_states[states_size],p.move_parser[i].new_state_name);
            states_size++;
        }
    }
    // Once we have all unique states from moves definition, we need to allocate memory
    // for each unique state object to assemble Automata_t object
    State_t* states;
    states = malloc((states_size+1)*sizeof(State_t));
    for (uint8_t i = 0; i < states_size; i++) {
        states[i].name=malloc(strlen(unique_states[i]+1)*sizeof(uint8_t));
        strcpy(states[i].name,unique_states[i]);
        states[i].type = STATE_NORMAL;
        free(unique_states[i]);
    }
    // Defining accept states to assemble Automata_t object
    for (uint8_t i = 0; i < p.accept_states_size ; i++) {
        for (uint8_t j = 0; j < states_size ; j++) {
            if (!strcmp(states[j].name,p.accept_states[i])) {
                states[j].type=STATE_ACCEPT;
                break;
            }
        }
    }
    // Checking if there were any accept state defined to validate Automata_t object
    uint8_t thereIsValidState = 0;
    for (uint8_t j = 0; j < states_size ; j++) {
        if (states[j].type==STATE_ACCEPT) {thereIsValidState=1; break;}
    }
    if (!thereIsValidState) {
        printf("No valid Accept State defined.\n");
        exit(1);
    }
    // Defining initial state to assemble Automata_t object
    // And checks if there is an initial state defined to validate Automata_t object
    thereIsValidState = 0;
    for (uint8_t j = 0; j < states_size ; j++) {
        if (!strcmp(states[j].name,p.initial_state)) {
            if (states[j].type!=STATE_ACCEPT) {
                states[j].type=STATE_INITIAL;
                thereIsValidState=1;
            } else {
                printf("Accept States cannot be Initial State.\n");
                exit(1);
            }
            break;
        }
    }
    if (!thereIsValidState) {
        printf("No valid Initial State defined.\n");
        exit(1);
    }

    // Defining Moves from Parser_t object and defined States_t with their memory already allocated
    // it does not perform any DTM or NDTM checks in this execution
    uint8_t moves_size=0;
    Move_t* moves=malloc((moves_size+1)*sizeof(Move_t));
    for (uint8_t i = 0; i < p.mparser_size ; i++) {
        moves=realloc(moves,(moves_size+1)*sizeof(Move_t));
        for (uint8_t j = 0; j < states_size ; j++) {
            if (!strcmp(states[j].name,p.move_parser[i].current_state_name)) {
                moves[moves_size].current_state = malloc(1*sizeof(State_t));
                moves[moves_size].current_state->name = malloc((strlen(p.move_parser[i].current_state_name)+1)*sizeof(uint8_t));
                strcpy(moves[moves_size].current_state->name,states[j].name);
                moves[moves_size].current_state->type=states[j].type;
                
            }
            if (!strcmp(states[j].name,p.move_parser[i].new_state_name)) {
                moves[moves_size].new_state = malloc(1*sizeof(State_t));
                moves[moves_size].new_state->name = malloc((strlen(p.move_parser[i].new_state_name)+1)*sizeof(uint8_t));
                strcpy(moves[moves_size].new_state->name,states[j].name);
                moves[moves_size].new_state->type=states[j].type;
            }
        }
        moves[moves_size].head_move=p.move_parser[i].head_move;
        moves[moves_size].read_symbol=p.move_parser[i].read_symbol;
        moves[moves_size].write_symbol=p.move_parser[i].write_symbol;
        moves_size++;
    }
    AutomataParser_t a;
    a.moves = moves;
    a.moves_size = moves_size;
    a.states = states;
    a.states_size = states_size;
    return a;
}

/// @brief Checks Multiple DTMs Tapes and Heads Definition from Parser_t given
/// and Allocates Memory to HeadParser_t output, if valid
/// Parser_t p (input) has not its memory deallocated
/// @param p Parser_t input to be checked
/// @return HeadParser_t object with Memory Allocated
HeadParser_t parserToHeadParser(Parser_t p) {
    HeadParser_t hp;

    // Double Check Tape and Heads
    if (p.head_size!=p.tapes_size) {
        printf("Number of Tape Definitions and Head Definitions must match.\n");
        exit(1);
    } else hp.size = p.head_size+1;

    // Copying to objects in array
    hp.tapes = malloc((hp.size+1)*sizeof(uint8_t*));
    for (uint8_t aux = 0; aux<hp.size; aux++) {
        // Tapes
        hp.tapes[aux] = malloc(strlen(p.tapes[aux]+1)*sizeof(uint8_t));
        strcpy(hp.tapes[aux],p.tapes[aux]);
        // free(p.tapes[aux]);
    }
    // Heads
    hp.heads = malloc((hp.size+1)*sizeof(uint8_t));
    for (uint8_t i=0;i<hp.size;i++) hp.heads[i]=p.heads[i];
    return hp;
}

/// @brief Generates a Deterministic Turing Machine (DTM) Object
/// TO DO: Rejects TM_t when there is more than one valid move, this verification is not made here, yet!
/// @param tape uint8_t* tape string
/// @param head uint8_t 0-based number INDEX (not pointer) pointing to tape head string
/// @param a AutomataParser_t Automata Object
/// @return TM_t Deterministic Turing Machine
TM_t DTM(uint8_t* tape, uint8_t head,AutomataParser_t a) {
    TM_t t;
    // Memory Allocating TM Tape
    t.tape = malloc((strlen(tape)+1)*sizeof(uint8_t));
    strcpy(t.tape,tape);
    // Memory Allocating TM Head
    t.head = head;
    // Memory Allocating TM States
    // TO DO: This is not necessary and can lead to memory overload
    // when running multiple Turing Machines, in later code revisions replace that to
    // states that are already defined and allocated to Automata_t a object
    t.states = malloc((a.states_size)*sizeof(State_t));
    t.states_size = a.states_size;
    for (uint8_t i = 0; i < a.states_size; i++) {
        t.states[i].name = malloc((strlen(a.states[i].name)+1)*sizeof(uint8_t));
        strcpy(t.states[i].name,a.states[i].name);
        t.states[i].type=a.states[i].type;
    }
    // Memory Allocating TM Moves
    t.moves = malloc((a.moves_size)*sizeof(Move_t));
    t.moves_size = a.moves_size;
    for (uint8_t i = 0; i < a.moves_size; i++) {
        // find the pointer that match the actual state pointer instead of memory allocate a state
        uint8_t foundCurrentState=0,foundNewState=0;
        for (uint8_t j = 0; j < t.states_size; j++) {
            if (!foundCurrentState) {
                if (!strcmp(a.moves[i].current_state->name,t.states[j].name)) {
                    foundCurrentState = 1;
                    t.moves[i].current_state=&t.states[j];
                }
            }
            if (!foundNewState) {
                if (!strcmp(a.moves[i].new_state->name,t.states[j].name)) {
                    foundNewState = 1;
                    t.moves[i].new_state=&t.states[j];
                }
            }
        }
        t.moves[i].read_symbol = a.moves[i].read_symbol;
        t.moves[i].write_symbol = a.moves[i].write_symbol;
        t.moves[i].head_move = a.moves[i].head_move;
    }    
    return t;
}

/// @brief Inputs a line buffer with whitespaces strings
/// Outputs a line buffer with no whitespaces strings
/// Every non-whitespace character will be copied
/// to a new line buffer with no whitespaces
/// @param buffer input buffer (char*)
/// @param clean_buffer output buffer (char*)
void cleanBuffer(char* buffer, char* clean_buffer) {
    uint8_t i = 0;
    uint8_t j = 0;
    while (buffer[i]!='\n' || buffer[i]!='\0' || buffer[i] != EOF) {
        if (!isspace((char)buffer[i])) {
            clean_buffer[j]=(char)buffer[i];
            j++;
        }
        // stop at MAX_LINE_LENGTH
        // prevents infinite loop
        if (i==MAX_LINE_LENGTH) break; else i++;
    }
    clean_buffer[j]='\0';
}

/// @brief Find the position of the substring in the input string
/// @param input input string (char*)
/// @param output output pointer (char*) where substring starts
/// @param substring const char* substring to be defined to look in string
void wipeOffSubstring(char* input, char* output, const char* substring) {
    char* pos = strstr(input, substring);
    if (pos) {
        // Copy everything up to the found substring into the output buffer
        size_t length = pos - input;
        strncpy(output, input, length);
        output[length] = 0;  // Null-terminate the output string
    } else {
        // If the substring is not found, copy the entire input to output
        strcpy(output, input);
    }
}

/// @brief This function writes a positions array with indexes with ',' character
/// @param line_buffer input buffer to seek ',' character
/// @param positions output array with 5 non-zero elements with ',' indexes, ends with \0
/// @param count number of commas in buffer
void countCommas(char* line_buffer,uint8_t* positions,uint8_t* count) {
    uint8_t i = 0, j = 0;
    // initialize all elements with 255
    for (uint8_t aux = 0; aux<5; aux++) positions[aux]=255;
    // set end of array with 0 to avoid pointer problems
    positions[5]=0;
    (*count)=0;
    while (line_buffer[i]!='\0') {
        if (line_buffer[i]==',') {
            positions[j]=i;
            j++;
            (*count)++;
        }
        i++;
    }
    positions[j]=0;
}

/// @brief Print Error Message with line number input
/// @param line_number uint32_t line number where interpreter found an error
void errorLineMessage(uint32_t line_number) {
    printf(INTERPRETER_ERROR_MESSAGE,line_number);
}