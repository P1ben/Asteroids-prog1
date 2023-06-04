#include "file_handler.h"
#include <stdio.h>

void save_scores_in_file(char* filename, Array_file_entry* array){
    FILE *file;
    if((file = fopen(filename,"w")) != NULL)
        {
            for(int i = 0; i < array->length; i++){
                fprintf(file, "%s\n%d\n", array->obj[i].name, array->obj[i].score);
            }
            fclose(file);
        }
    else
        {
            printf("Hiba tortent a file megnyitasakor.");
        }
}
void destroy_Array_file_entry(Array_file_entry* array){
    if(array->obj != NULL){
        free(array->obj);
        array->obj = NULL;
        array->length = 0;
    }
}

Array_file_entry read_file_contents(char* filename){
    Array_file_entry array = {NULL, 0};
    file_entry a;
    FILE *file;
    if((file = fopen(filename,"r")) != NULL)
        {
            while(fscanf(file, "%16s\n%d\n", &a.name, &a.score) == 2){
                append_file_entry_to_Array(&array, a);
            }
            fclose(file);
            return array;
        }
    else
        {
            printf("Hiba tortent a file megnyitasakor.");
            return array;
        }

}
void file_entry_cserel(Array_file_entry* array, int index0, int index1){
    file_entry x = array->obj[index0];
    array->obj[index0] = array->obj[index1];
    array->obj[index1] = x;
}

void sort_Array_file_entry(Array_file_entry* array){
    int i = 1;
    while(i > 0){
        i = 0;
        for(int j = 0; j < array->length - 1; j++){
            if(array->obj[j].score < array->obj[j + 1].score){
                file_entry_cserel(array, j, j + 1);
                i++;
            }
        }
    }
}

void Array_file_entry_top10(Array_file_entry* array){
    if(array->length > 10){
        array->obj = realloc(array->obj, 10*sizeof(file_entry));
        array->length = 10;
    }
}

void beszur_elem_majd_rendez(Array_file_entry* array, file_entry entry){
    append_file_entry_to_Array(array, entry);
    sort_Array_file_entry(array);
    Array_file_entry_top10(array);
}

void append_file_entry_to_Array(Array_file_entry* array, file_entry item){
    array->obj = realloc(array->obj, sizeof(file_entry) * (array->length + 1));
    array->obj[array->length] = item;
    array->length += 1;
}
