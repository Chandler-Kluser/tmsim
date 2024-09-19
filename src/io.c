// ======================================================================
// Turing Machine Simulator
// Chandler Klüser, 2024
// ======================================================================

#include <io.h>
#include <string.h>
#ifdef MINGW
#include <windows.h>
#include <locale.h>
#include <wchar.h>
#endif

/// @brief Print Tape String and its Head in Console
/// @param TM_string    Tape String
/// @param TM_head      Tape Head
void printTape(uint8_t* TM_string,uint8_t TM_head) {
    uint8_t* str = TM_string;
    uint8_t TM_string_size = strlen(TM_string);
    for (uint8_t a = 0; a<TM_string_size; a++){
#ifdef MINGW
    printf("%c",TM_string[a]);
#else
    if (a==TM_head) printf("\e[1;31m%c\e[0m",TM_string[a]); else printf("%c",TM_string[a]);
#endif
        str++;
    }
    printf("\n");
    str = TM_string;
    for (uint8_t a = 0; a<TM_string_size; a++){
#ifdef MINGW
        if (a==TM_head) printf("^"); else printf(" ");
#else
        if (a==TM_head) printf("\e[1;31m^\e[0m"); else printf(" ");
#endif
        str++;
    }
    printf("\n");
}

/// @brief Print DTM Current Status, its Tape String and its Head in Console
/// @param TM_string    DTM Tape String
/// @param TM_head      DTM Tape Head
/// @param TM_num       DTM Number in a Greater List (given) of running DTMs
void printTapeNum(uint8_t* TM_string,uint8_t TM_head,uint8_t TM_num) {
    uint8_t* str = TM_string;
    uint8_t TM_string_size = strlen(TM_string);
    printf("Turing Machine %i Running...\n",TM_num);
    for (uint8_t a = 0; a<TM_string_size; a++){
#ifdef MINGW
        printf("%c",TM_string[a]);
#else
        if (a==TM_head) printf("\e[1;31m%c\e[0m",TM_string[a]); else printf("%c",TM_string[a]);
#endif
        str++;
    }
    printf("\n");
    str = TM_string;
    for (uint8_t a = 0; a<TM_string_size; a++){
#ifdef MINGW
        if (a==TM_head) printf("^"); else printf(" ");
#else
        if (a==TM_head) printf("\e[1;31m^\e[0m"); else printf(" ");
#endif
        str++;
    }
    printf("\n");
}

/// @brief Print DTM Current Status
/// @param stepStatus DTM Status Number
void printTMStatus(uint8_t stepStatus) {
    switch (stepStatus)
    {
    case 0:
        printf("Turing Machine Running...\n");
        break;
    case 1:
#ifdef MINGW
        printf("Turing Machine in Accept State!\n");
#else
        printf("\e[1;32mTuring Machine in Accept State!\e[0m\n");
#endif
        break;
    case 2:
#ifdef MINGW
        printf("Turing Machine Stopped!\n");
#else
        printf("\e[1;31m\e[1mTuring Machine Stopped!\e[0m\n");
#endif
        break;
    
    default:
        break;
    }
}

/// @brief Print DTM Current Status with its number in a greater list of running DTMs
/// @param stepStatus   DTM Status Number
/// @param TM_num       DTM Number in List
void printTMStatusNum(uint8_t stepStatus,uint8_t TM_num) {
    switch (stepStatus)
    {
    case 0:
        // printf("Turing Machine Running...\n");
        break;
    case 1:
#ifdef MINGW
        printf("Turing Machine %i in Accept State!\n",TM_num);
#else
        printf("\e[1;32mTuring Machine %i in Accept State!\e[0m\n",TM_num);
#endif
        break;
    case 2:
#ifdef MINGW
        printf("Turing Machine %i Stopped!\n",TM_num);
#else
        printf("\e[1;31m\e[1mTuring Machine %i Stopped!\e[0m\n",TM_num);
#endif
        break;
    
    default:
        break;
    }
}

// TO DO: Add print Turing Machine Current Stante void Function