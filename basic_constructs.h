#ifndef BASIC_CONSTRUCTS_H
#define BASIC_CONSTRUCTS_H

#include <SDL.h>
#include <stdbool.h>
#include "debugmalloc.h"

// Egy méretet ad meg, szélesség és magasság.
typedef struct Size{
    int w;
    int h;
} Size;

// Az event kezelésért felelős állapotgép állapotkódjai.
enum event_codes{
    event_game_tick,
    event_quit,
    event_main_to_diff_select,
    event_diff_select_to_main,
    event_main_to_leaderboard,
    event_leaderboard_to_main,
    event_diff_select_easy,
    event_diff_select_normal,
    event_diff_select_hard,
    event_start_game,
    event_name_input_to_main
};

// A nehézségi szintek kódjai.
enum difficulty{
    easy,
    normal,
    hard
};

// Egy pontot ad meg a képrenyőn (x, y lebegőpontos valós).
typedef struct Point{
    double x;
    double y;
} Point;

// Egy textúra eltárolására képes.
typedef struct Texture{
    SDL_Texture *texture;                       // Betöltött textúra
    Point position;                             // A betöltött textúrán a keresett kextúra kezdőpozíciója (bal felső sarok) {Ha valamelyik érték {-1}, az egész képet tölti be a program.}
    Size size;                                  // A kivágandó részlet mérete
    double rotation;                            // A textúra forgatása (fok -> 0 = felfele)
} Texture;

// Egy betöltött textúrákat tároló tömb [A program végén megkönnyíti a betöltött textúrák felszabadítását].
typedef struct SDL_Texture_Array{
    SDL_Texture** items;                        // Betöltött textúrákra mutató pointereket tartalmazó dinamikus tömb.
    int length;                                 // A dinamikus tömb hossza.
} SDL_Texture_Array;

// Inicializál egy új SDL_Texture_Array-t
SDL_Texture_Array init_SDL_Texture_Array();

// Hozzáad egy textúrát egy SDL_Texture_Array-hez
void append_item_to_SDL_Texture_Array(SDL_Texture_Array* texture_array, SDL_Texture* texture);

// Megnézi, hogy egy szám bent van-e egy megadott intervallumban
bool isInRange(int number, int lower_boundary_excluded, int upper_boundary_included);

// Felszabadítja az összes elemet egy SDL_Texture_Array-ben
void free_items_in_SDL_Texture_Array(SDL_Texture_Array* texture_array);

// Visszaadja 2 pont egymástól való abszolút távolságát
double point_dist(Point a, Point b);

// Betölt egy textúrát egy fájlból, majd a kapott pointert hozzáadja egy SDL_Texture_Array-hez
SDL_Texture* load_texture(SDL_Renderer* renderer, char* file_name, SDL_Texture_Array* texture_array);

// Törli az SDL Event-queue-ból az összes SDL_USEREVENT típusú eventet, majd hozzáad egy event_code által definiált eventet.
void send_user_event(enum event_codes event_code);

// Egy egész számot 6 karakteres sztringgé konvertál, az alábbi formátumban: [52 -> "000052"]
void convert_score_to_string(int score, char* output);
#endif
