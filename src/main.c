// ======================================================================
// Turing Machine Simulator
// Chandler Klüser, 2024
// ======================================================================

#include <rules.h>
#include <interpreter.h>
#include <main.h>
#include <io.h>
#ifdef OPENMP
#include <omp.h>
uint8_t jobs = 2;
#endif

int8_t main(int argc, char *argv[]) {
#ifdef MINGW
    SetConsoleOutputCP(CP_UTF8);
    setlocale(LC_ALL, "");
#endif
    TM_t* t;
    uint32_t t_number=0;
    parseArgs(argc, argv);

    // if there is a TM define request, checks TM mode
    // raises error elsewhere
    if (TM_defined) {
        if (DTM_mode && NDTM_mode) {
            printf("You can't use both DTM and NDTM modes.\n");
            exit(1);
        }
        if (!DTM_mode && !NDTM_mode) {
            printf("You must define a Turing Machine Simulation mode.\n");
            exit(1);
        }
        if (DTM_mode) {
            AutomataParser_t a;
            HeadParser_t hp;
            a = parserToAutomata(p);
            hp = parserToHeadParser(p);
            t = malloc((t_number+1)*sizeof(TM_t));
            for (uint8_t i = 0; i < hp.size; i++) {
                t[i] = DTM(hp.tapes[i],hp.heads[i],a);
                (t_number)++;
                t = realloc(t,(t_number+1)*sizeof(TM_t));
            }
        }
        if (NDTM_mode) {
            AutomataParser_t a;
            HeadParser_t hp;
            a = parserToAutomata(p);
            hp = parserToHeadParser(p);
            if (hp.size>1) {
                printf("You can only run a single NDTM per file.\n");
                exit(1);
            }
            t = malloc(1*sizeof(TM_t));
            t[0] = DTM(hp.tapes[0],hp.heads[0],a);
        }
    }
    
    if (NDTM_mode) {
        uint8_t stop = 0;
        uint32_t* flagStatus = malloc(1*sizeof(uint32_t));
        flagStatus[0]=STATUS_SGMOVE;
        State_t* current_state = malloc(1*sizeof(State_t));
        uint8_t* tmlen = malloc(1*sizeof(uint8_t));
        uint8_t** tm_string_length = malloc(1*sizeof(uint8_t*));
        uint32_t i = 0;
        ValidMoves_t* valid_moves = malloc(1*sizeof(ValidMoves_t));
        // Initializes Valid_moves on first time, since its base is going to be free every iteration
        valid_moves[0].length=0;
        valid_moves[0].base = malloc(1*sizeof(Move_t*));
        StatusNDTM_t status_ndtm;
        // finds initial state for original TM
        for (uint8_t k = 0; k < t[0].states_size; k++) {
            if (t[0].states[k].type==STATE_INITIAL) {current_state[0]=t[0].states[k];break;}
        }
        // defines tape for original TM
        tmlen[0]=strlen(t[0].tape);
        while (!stop) {
            // find Valid Moves on Non Deterministic Turing Machine
            // this is different from a Valid Move search on a DTM
            
            // skips stopped TMs
            if (flagStatus[i]==STATUS_NOMOVE) {
                // update while index
                i++;
                // resets index on t_number overflow
                if (i>t_number) i=0;
                continue;
            }

            // Returns 1 if NDTM is in an Accept State
            if (current_state[i].type==STATE_ACCEPT) {
                flagStatus[i]=STATUS_ACCEPT;
                status_ndtm.status=STATUS_ACCEPT;
                status_ndtm.valid_moves.length=0;
                status_ndtm.valid_moves.base=NULL;
                stop=1;
                if (isVerbose) {
                    // print tape and status
                    printTapeNum(t[i].tape,t[i].head,i);
                    printTMStatusNum(flagStatus[i],i);
                }
                break;
            } else {
                // Reinitializes input valid_moves given object
                if (valid_moves[i].length!=0) valid_moves[i].length=0;

                // Find valid moves in current turing machine
                for (uint8_t j = 0;j<t[i].moves_size;j++) {
                    if (t[i].moves[j].current_state->name==current_state[i].name) {
                        if (t[i].moves[j].read_symbol==t[i].tape[t[i].head]) {
                            valid_moves[i].base[valid_moves[i].length] = &(t[i].moves[j]);
                            valid_moves[i].length++;
                            valid_moves[i].base = realloc(valid_moves[i].base,(valid_moves[i].length+1)*sizeof(Move_t*));
                        }
                    }
                }
                // If no move valid, returns 2.
                if (valid_moves[i].length==0) {
                    flagStatus[i]=STATUS_NOMOVE;
                    status_ndtm.status=STATUS_NOMOVE;
                    status_ndtm.valid_moves.length=0;
                    status_ndtm.valid_moves.base=NULL;
                } else {
                    // Checks if there is only 1 Valid Move, if so STATUS=0, 3 elsewhere
                    if (valid_moves[i].length==1) status_ndtm.status=STATUS_SGMOVE; else status_ndtm.status=STATUS_MMOVES;
                    status_ndtm.valid_moves.length=valid_moves[i].length;
                    status_ndtm.valid_moves.base=valid_moves[i].base;
                }
            }
            
            if (status_ndtm.status==STATUS_MMOVES) {
                t                = realloc(t,(t_number+status_ndtm.valid_moves.length+1)*sizeof(TM_t));
                flagStatus       = realloc(flagStatus,(t_number+status_ndtm.valid_moves.length+1)*sizeof(uint32_t));
                tmlen            = realloc(tmlen,(t_number+status_ndtm.valid_moves.length+1)*sizeof(uint8_t));
                tm_string_length = realloc(tm_string_length,(t_number+status_ndtm.valid_moves.length+1)*sizeof(uint8_t*));
                valid_moves      = realloc(valid_moves,(t_number+status_ndtm.valid_moves.length+1)*sizeof(ValidMoves_t));
                current_state    = realloc(current_state,(t_number+status_ndtm.valid_moves.length+1)*sizeof(State_t));
                // counter does not starts at zero because the first non-deterministic move found is already updated in
                // extending arrays to add children NDTM instances (zero is skipped since has been overwriten in i-index)
                for (uint8_t j = 1; j<status_ndtm.valid_moves.length; j++) {
                    valid_moves[t_number+j].length = 0;
                    valid_moves[t_number+j].base = malloc(1*sizeof(Move_t*));
                    flagStatus[t_number+j] = STATUS_SGMOVE;
                    t[t_number+j].tape = malloc(strlen(t[i].tape)*sizeof(uint8_t));
                    strcpy(t[t_number+j].tape,t[i].tape);
                    tmlen[t_number+j] = strlen(t[t_number+j].tape);
                    tm_string_length[t_number+j] = &tmlen[t_number+j];
                    t[t_number+j].head = t[i].head;
                    t[t_number+j].moves = t[i].moves;
                    t[t_number+j].moves_size = t[i].moves_size;
                    t[t_number+j].states = t[i].states;
                    t[t_number+j].states_size = t[i].states_size;
                    // moving NDTM instance (t_number+1 to t_number+valid_moves_length+1)
                    // writing character to tape
                    t[t_number+j].tape[t[t_number+j].head] = status_ndtm.valid_moves.base[j]->write_symbol;
                    // moving head (left or right)
                    // Checking if it is necessary to append a character leftside
                    if (status_ndtm.valid_moves.base[j]->head_move==MOVE_LEFT && t[t_number+j].head==0) {
                        // Append Left
                        t[t_number+j].tape = realloc(t[t_number+j].tape, (tmlen[t_number+j]+1)*sizeof(uint8_t));
                        // shift every character one position right
                        for (uint16_t aux = tmlen[t_number+j]-1;aux>=0;aux--) t[t_number+j].tape[aux+1]=t[t_number+j].tape[aux];
                        // add blank space to first element
                        t[t_number+j].tape[0] = ' ';
                        // finish tape string with 0
                        t[t_number+j].tape[tmlen[t_number+j]+1] = (uint8_t)0;
                        // shift head to the right
                        t[t_number+j].head++;
                        // Update Tape Size
                        tmlen[t_number+j]++;
                    }
                    // Checking if it is necessary to append a character rightside
                    if (status_ndtm.valid_moves.base[j]->head_move==MOVE_RIGHT && t[t_number+j].head==tmlen[t_number+j]-1) {
                        // Append Right
                        t[t_number+j].tape = realloc(t[t_number+j].tape, (tmlen[t_number+j]+1)*sizeof(uint8_t));
                        // add blank space to last element
                        t[t_number+j].tape[tmlen[t_number+j]] = ' ';
                        // finish tape string with 0
                        t[t_number+j].tape[tmlen[t_number+j]+1] = (uint8_t)0;
                        // Update Tape Size
                        tmlen[t_number+j]++;
                        // Not necessary to move string head to the left
                    }
                    // move head
                    moveHead(&t[t_number+j].head,status_ndtm.valid_moves.base[j]->head_move);
                    // update current_state for new instances
                    current_state[t_number+j] = *status_ndtm.valid_moves.base[j]->new_state;
                }
                // moving original instance with the first valid move in non-deterministic state found
                t[i].tape[t[i].head] = status_ndtm.valid_moves.base[0]->write_symbol;
                // moving head (left or right)

                // Checking if it is necessary to append a character leftside
                if (status_ndtm.valid_moves.base[0]->head_move==MOVE_LEFT && t[i].head==0) {
                    // Append Left
                    t[i].tape = realloc(t[i].tape, (tmlen[i]+1)*sizeof(uint8_t));
                    // shift every character one position right
                    for (uint16_t aux = tmlen[i]-1;aux>=0;aux--) t[i].tape[aux+1]=t[i].tape[aux];
                    // add blank space to first element
                    t[i].tape[0] = ' ';
                    // finish tape string with 0
                    t[i].tape[tmlen[i]+1] = (uint8_t)0;
                    // shift head to the right
                    t[i].head++;
                    // Update Tape Size
                    tmlen[i]++;
                }
                // Checking if it is necessary to append a character rightside
                if (status_ndtm.valid_moves.base[0]->head_move==MOVE_RIGHT && t[i].head==tmlen[i]-1) {
                    // Append Right
                    t[i].tape = realloc(t[i].tape, (tmlen[i]+1)*sizeof(uint8_t));
                    // add blank space to last element
                    t[i].tape[tmlen[i]] = ' ';
                    // finish tape string with 0
                    t[i].tape[tmlen[i]+1] = (uint8_t)0;
                    // Update Tape Size
                    tmlen[i]++;
                    // Not necessary to move string head to the left
                }
                // move head
                moveHead(&t[i].head,status_ndtm.valid_moves.base[0]->head_move);
                // update current_state
                current_state[i] = *status_ndtm.valid_moves.base[0]->new_state;
                // update t_number
                t_number=t_number+status_ndtm.valid_moves.length-1;
            }
            if (status_ndtm.status==STATUS_SGMOVE) {
                // moving original instance with the first valid move in non-deterministic state found
                t[i].tape[t[i].head] = status_ndtm.valid_moves.base[0]->write_symbol;
                // moving head (left or right)

                // Checking if it is necessary to append a character leftside
                if (status_ndtm.valid_moves.base[0]->head_move==MOVE_LEFT && t[i].head==0) {
                    // Append Left
                    t[i].tape = realloc(t[i].tape, (tmlen[i]+1)*sizeof(uint8_t));
                    // shift every character one position right
                    for (uint16_t aux = tmlen[i]-1;aux>=0;aux--) t[i].tape[aux+1]=t[i].tape[aux];
                    // add blank space to first element
                    t[i].tape[0] = ' ';
                    // finish tape string with 0
                    t[i].tape[tmlen[i]+1] = (uint8_t)0;
                    // shift head to the right
                    t[i].head++;
                    // Update Tape Size
                    tmlen[i]++;
                }
                // Checking if it is necessary to append a character rightside
                if (status_ndtm.valid_moves.base[0]->head_move==MOVE_RIGHT && t[i].head==tmlen[i]-1) {
                    // Append Right
                    t[i].tape = realloc(t[i].tape, (tmlen[i]+1)*sizeof(uint8_t));
                    // add blank space to last element
                    t[i].tape[tmlen[i]] = ' ';
                    // finish tape string with 0
                    t[i].tape[tmlen[i]+1] = (uint8_t)0;
                    // Update Tape Size
                    tmlen[i]++;
                    // Not necessary to move string head to the left
                }
                // move head
                moveHead(&t[i].head,status_ndtm.valid_moves.base[0]->head_move);
                // update current_state
                current_state[i] = *status_ndtm.valid_moves.base[0]->new_state;
            }

            if (isVerbose) {
                // print tape and status
                printTapeNum(t[i].tape,t[i].head,i);
                printTMStatusNum(flagStatus[i],i);
            }

            // update while index
            i++;
            // resets index on t_number overflow
            if (i>t_number) i=0;
        }
        return (int8_t) 0;
    }
#ifdef OPENMP
    omp_set_num_threads(jobs);
    uint32_t stepStatus[t_number];
    uint8_t tmlen[t_number];
    uint8_t* tm_string_length[t_number];
    State_t* initial_state[t_number];
    State_t** initial_state_ptr[t_number];
    if (firstAccept) {
        uint8_t stop = 0;
        #pragma omp parallel for shared(stop)
        for (uint8_t tm_num = 0; tm_num<t_number; tm_num++) {
            if (stop) continue;
            stepStatus[tm_num] = 0;
            tmlen[tm_num] = strlen(t[tm_num].tape);
            tm_string_length[tm_num] = &tmlen[tm_num];
            for (uint8_t i = 0; i < t[tm_num].states_size; i++) {
                if (t[tm_num].states[i].type==STATE_INITIAL) {initial_state[tm_num]=&(t[tm_num].states[i]);break;}
            }
            initial_state_ptr[tm_num]=&initial_state[tm_num];
            if (isVerbose) {
                printTapeNum(t[tm_num].tape,t[tm_num].head,tm_num);
                while (stepStatus[tm_num]==0) {
                    stepStatus[tm_num] = runStepTM(&t[tm_num].tape,&t[tm_num].head,tm_string_length[tm_num],t[tm_num].states,initial_state_ptr[tm_num],t[tm_num].states_size,t[tm_num].moves,t[tm_num].moves_size);
                    printTapeNum(t[tm_num].tape,t[tm_num].head,tm_num);
                }
            } else {
                while (stepStatus[tm_num]==0) stepStatus[tm_num] = runStepTM(&t[tm_num].tape,&t[tm_num].head,tm_string_length[tm_num],t[tm_num].states,initial_state_ptr[tm_num],t[tm_num].states_size,t[tm_num].moves,t[tm_num].moves_size);
            }
            printTMStatusNum(stepStatus[tm_num],tm_num);
            if (stepStatus[tm_num]==(uint8_t)1) {
                #pragma omp atomic write
                stop=1;
            }
        }
    } else {
        #pragma omp parallel for
        for (uint8_t tm_num = 0; tm_num<t_number; tm_num++) {
            stepStatus[tm_num] = 0;
            tmlen[tm_num] = strlen(t[tm_num].tape);
            tm_string_length[tm_num] = &tmlen[tm_num];
            for (uint8_t i = 0; i < t[tm_num].states_size; i++) {
                if (t[tm_num].states[i].type==STATE_INITIAL) {initial_state[tm_num]=&(t[tm_num].states[i]);break;}
            }
            initial_state_ptr[tm_num]=&initial_state[tm_num];
            if (isVerbose) {
                printTapeNum(t[tm_num].tape,t[tm_num].head,tm_num);
                while (stepStatus[tm_num]==0) {
                    stepStatus[tm_num] = runStepTM(&t[tm_num].tape,&t[tm_num].head,tm_string_length[tm_num],t[tm_num].states,initial_state_ptr[tm_num],t[tm_num].states_size,t[tm_num].moves,t[tm_num].moves_size);
                    printTapeNum(t[tm_num].tape,t[tm_num].head,tm_num);
                }
            } else {
                while (stepStatus[tm_num]==0) stepStatus[tm_num] = runStepTM(&t[tm_num].tape,&t[tm_num].head,tm_string_length[tm_num],t[tm_num].states,initial_state_ptr[tm_num],t[tm_num].states_size,t[tm_num].moves,t[tm_num].moves_size);
            }
            printTMStatusNum(stepStatus[tm_num],tm_num);
        }
    }
    return (int8_t)0;
#else
    uint8_t stepStatus[t_number];
    if (firstAccept) {
        uint8_t stop = 0;
        for (uint8_t tm_num = 0; tm_num<t_number; tm_num++) {
            if (stop) continue;
            stepStatus[tm_num] = 0;
            uint8_t tmlen = strlen(t[tm_num].tape);
            uint8_t* tm_string_length = &tmlen;
            State_t* initial_state;
            State_t** initial_state_ptr;
            for (uint8_t i = 0; i < t[tm_num].states_size; i++) {
                if (t[tm_num].states[i].type==STATE_INITIAL) {initial_state=&(t[tm_num].states[i]);break;}
            }
            initial_state_ptr=&initial_state;
            if (isVerbose) {
                printTapeNum(t[tm_num].tape,t[tm_num].head,tm_num);
                while (stepStatus[tm_num]==0) {
                    stepStatus[tm_num] = runStepTM(&t[tm_num].tape,&t[tm_num].head,tm_string_length,t[tm_num].states,initial_state_ptr,t[tm_num].states_size,t[tm_num].moves,t[tm_num].moves_size);
                    printTapeNum(t[tm_num].tape,t[tm_num].head,tm_num);
                }
            } else {
                while (stepStatus[tm_num]==0) stepStatus[tm_num] = runStepTM(&t[tm_num].tape,&t[tm_num].head,tm_string_length,t[tm_num].states,initial_state_ptr,t[tm_num].states_size,t[tm_num].moves,t[tm_num].moves_size);
            }
            printTMStatusNum(stepStatus[tm_num],tm_num);
            if (stepStatus[tm_num]==(uint8_t)1) stop=1;
        }
    } else {
        for (uint8_t tm_num = 0; tm_num<t_number; tm_num++) {
            stepStatus[tm_num] = 0;
            uint8_t tmlen = strlen(t[tm_num].tape);
            uint8_t* tm_string_length = &tmlen;
            State_t* initial_state;
            State_t** initial_state_ptr;
            for (uint8_t i = 0; i < t[tm_num].states_size; i++) {
                if (t[tm_num].states[i].type==STATE_INITIAL) {initial_state=&(t[tm_num].states[i]);break;}
            }
            initial_state_ptr=&initial_state;
            if (isVerbose) {
                printTapeNum(t[tm_num].tape,t[tm_num].head,tm_num);
                while (stepStatus[tm_num]==0) {
                    stepStatus[tm_num] = runStepTM(&t[tm_num].tape,&t[tm_num].head,tm_string_length,t[tm_num].states,initial_state_ptr,t[tm_num].states_size,t[tm_num].moves,t[tm_num].moves_size);
                    printTapeNum(t[tm_num].tape,t[tm_num].head,tm_num);
                }
            } else {
                while (stepStatus[tm_num]==0) stepStatus[tm_num] = runStepTM(&t[tm_num].tape,&t[tm_num].head,tm_string_length,t[tm_num].states,initial_state_ptr,t[tm_num].states_size,t[tm_num].moves,t[tm_num].moves_size);
            }
            printTMStatusNum(stepStatus[tm_num],tm_num);
        }
    }
    return (int8_t)0;
#endif
}

/// @brief Parse Main Function Arguments (flags) and outputs an Array of DTMs
/// @brief it also displays help and options when --help is given
/// @param argc Main Function argc integer
/// @param argv Main Function argv char* pointer
/// @param TM TM_t** pointer with DTMs
/// @param TM_number length of TM pointer with TM quantity
void parseArgs(int argc, char *argv[]) {
    for (uint16_t i=0; i<argc; i++) {
        if (strcmp(argv[i], "-V" ) == 0 || strcmp(argv[i], "--version") == 0) {
#ifdef MINGW
            // Get handle to the console output
            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            // Write the string to the console
            WriteConsoleW(hConsole, PROGRAM_NAME, wcslen(PROGRAM_NAME), NULL, NULL);
            WriteConsoleW(hConsole, AUTHOR_NAME, wcslen(AUTHOR_NAME), NULL, NULL);
#else
            printf(PROGRAM_NAME);
            printf(AUTHOR_NAME);
#endif
#ifdef OPENMP
            printf("\n");
            printf(OPENMP_SUPPORT_MESSAGE);
#endif
            printf(VERSION);
            exit(0);
        }
        if (strcmp(argv[i], "-H") == 0 || strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
#ifdef MINGW
            // Get handle to the console output
            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            // Write the string to the console
            WriteConsoleW(hConsole, PROGRAM_NAME, wcslen(PROGRAM_NAME), NULL, NULL);
            WriteConsoleW(hConsole, AUTHOR_NAME, wcslen(AUTHOR_NAME), NULL, NULL);
#else
            printf(PROGRAM_NAME);
            printf(AUTHOR_NAME);
#endif
#ifdef CLANG
            printf("Built with clang\n");
#else
            printf("Built with gcc\n");
#endif
#ifdef OPENMP
            printf("\n");
            printf(OPENMP_SUPPORT_MESSAGE);
#endif
            printf("\n");
            printf(" Option    Long Option              Argument                     Meaning\n");
            printf("   -V      --version                                Display Simulator Version\n");
            printf(" -h, -H    --help                                   Display Help and Commands\n");
            printf("   -r      --read_file             <filename>       Start Turing Machine from a Script File\n");
            printf("   -v      --verbose                                Print every Turing Machine step\n");
            printf("   -f      --first_accept                           When running Multiple Turing Machines, stops new threads if any Turing Machine is in an Accept State\n");
#ifdef OPENMP
            printf("   -j      --jobs                 <jobs_number>     Triggers Multiple Threads mode for Parallel Simulations (only for OpenMP support)\n");
#endif
            printf("  -DTM     --deterministic                          Deterministic Single Tape Turing Machine Mode\n");
            printf(" -NDTM     --non_deterministic                      Non Deterministic Single Tape Turing Machine Mode\n");
            // printf("  -i      --interactive                   Run Turing Machine in REPL mode (WIP)\n");
            exit(0);
        }
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) isVerbose=1;
#ifdef OPENMP
        if (strcmp(argv[i], "-j") == 0 || strcmp(argv[i], "--jobs") == 0) {
            char *end_ptr;
            long val = strtol(argv[i+1],&end_ptr,10);
            if (end_ptr == argv[i+1]) {
                printf("Error: No digits were found in the input string.\n");
                exit(1);
            }
            // The number is out of range for uint8_t
            if ((val < 0 || val > UINT8_MAX)) {
                printf("Number must be between 1 and 255.\n");
                exit(1);
            }
            jobs=val;
        }
#endif
        if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--read_file") == 0) {
            const char *filename = argv[i+1];
            p = parseFile(filename);
            TM_defined=1;
            // TO DO: make a free parser function to deallocate memory in Parser_t object
            
#ifdef MINGW
            // Get handle to the console output
            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            // Write the string to the console
            WriteConsoleW(hConsole, PROGRAM_NAME, wcslen(PROGRAM_NAME), NULL, NULL);
            WriteConsoleW(hConsole, AUTHOR_NAME, wcslen(AUTHOR_NAME), NULL, NULL);
#else
            printf(PROGRAM_NAME);
            printf(AUTHOR_NAME);
#endif
            printf("\n");
        }
        if (strcmp(argv[i], "-DTM") == 0 || strcmp(argv[i], "--deterministic") == 0) DTM_mode=1;
        if (strcmp(argv[i], "-NDTM") == 0 || strcmp(argv[i], "--non_deterministic") == 0) NDTM_mode=1;
        if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--first_acceptc") == 0) firstAccept=1;
    }
};