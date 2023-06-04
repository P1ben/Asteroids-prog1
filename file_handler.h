#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H
#define PLAYER_NAME_MAX_LENGTH 16

#include "debugmalloc.h"

// Egy nevet, illetve a névhez tartozó pontszámot tartalmazó struktúra
typedef struct file_entry{
    char name[PLAYER_NAME_MAX_LENGTH + 1];                      // Név
    int score;                                                  // Pontszám
} file_entry;

// Egy file_entrykből álló dinamikus tömböt, illetve annak hosszát tartalmazó struktúra.
typedef struct Array_file_entry{                                
    struct file_entry* obj;                                     // A dinamikus tömb
    int length;                                                 // A tömb hossza
}Array_file_entry;

// Megnyit egy fájlt, majd beleírja a az összes file_entryt egy megadott Array_file_entry-ből (A fájl eredeti tartalma törlődik)
void save_scores_in_file(char* filename, Array_file_entry* array);

// Felaszabadít egy Array_file_entryt, amennyiben a struktúrában található tömbre mutató pointer nem NULL
void destroy_Array_file_entry(Array_file_entry* array);

// Lértehoz egy új Array_file_entryt, majd beolvassa a megadott fájlban eltárolt adatokat.
Array_file_entry read_file_contents(char* filename);

// Pontszám szerinti csökkenő sorrendbe rendezi egy megadott Array_file_entry tartalmát
void sort_Array_file_entry(Array_file_entry* array);

// Egy Array_file_entry tömbjén belül két objektum helyét megcseréli
void file_entry_cserel(Array_file_entry* array, int index0, int index1);

// Hozzáad egy új elemet egy Array_file_entryhez, majd pontszám szerinti csökkenő sorrendbe rendezi a tömböt. Ezután a tömb hosszát limitálja 10 file_entryre, levágva a maradékot.
void beszur_elem_majd_rendez(Array_file_entry* array, file_entry entry);

// Hozzáad egy új file_entryt egy Array_file_entry tömbjéhez
void append_file_entry_to_Array(Array_file_entry* array, file_entry item);

// Egy Array_file_entry tömb hosszát limitálja 10 elemre, a maradékot pedig levágja
void Array_file_entry_top10(Array_file_entry* array);
#endif
