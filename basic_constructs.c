#include "basic_constructs.h"
#include "menu_items.h"
#include <math.h>
#include <SDL_image.h>


bool isInRange(int num, int range_lower, int range_upper) {
    return num < range_upper && num >= range_lower;
}

double point_dist(Point a, Point b) {
    double tav_x = a.x - b.x;
    double tav_y = a.y - b.y;
    return (sqrt((tav_x * tav_x) + (tav_y * tav_y)));
}

// Egy új usereventet ad hozzá az event queue-hoz
void send_user_event(enum event_codes code) {
    SDL_Event ev;
    ev.type = SDL_USEREVENT;
    ev.user.code = code;
    SDL_FlushEvent(SDL_USEREVENT);
    SDL_PushEvent(&ev);
}

SDL_Texture_Array init_SDL_Texture_Array(){
    return (SDL_Texture_Array){NULL, 0};
}

void append_item_to_SDL_Texture_Array(SDL_Texture_Array* array, SDL_Texture* item){
    array->items = realloc(array->items, sizeof(SDL_Texture*) * (array->length + 1));
    array->items[array->length] = item;
    array->length += 1;
}

void free_items_in_SDL_Texture_Array(SDL_Texture_Array* array){
    for(int i = 0; i < array->length; i++){
        SDL_DestroyTexture(array->items[i]);
    }
    free(array->items);
}

SDL_Texture* load_texture(SDL_Renderer* renderer, char* file, SDL_Texture_Array* texture_array){
    SDL_Texture *text = IMG_LoadTexture(renderer, file);
    if (text == NULL) {
        SDL_Log("Nem nyithato meg a kepfajl: %s", IMG_GetError());
        exit(1);
    }
    append_item_to_SDL_Texture_Array(texture_array, text);
    return text;
}

void convert_score_to_string(int score, char* dest){
    if(score > 999999) score = 999999;
    if(score < 0) score = 0;

    for(int i = 0; i < 6; i++){
        int curr = (int)pow(10., (double)(6 - (i + 1)));
        dest[i] = '0' + (score / curr);
        score %= curr;
    }
    dest[6] = 0;
}
