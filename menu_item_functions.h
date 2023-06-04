#ifndef MENU_ITEM_FUNCTIONS_H
#define MENU_ITEM_FUNCTIONS_H

#include "basic_constructs.h"
#include "debugmalloc.h"

// Megnézi egy gomb állapotát, majd ez alapján kirajzolja, illetve végrehajtja az állapothoz hozzárendelt függvényt
void check_button_state(SDL_Renderer* renderer, menu_item* item);

// A play_button onClick eventje
void play_button_onClick(menu_item*);

// A diff_back_button onClick eventje
void diff_back_button_onClick(menu_item*);

// A exit_button onClick eventje
void exit_button_onClick(menu_item*);

// A leaderboard_button onClick eventje
void leaderboard_button_onClick(menu_item*);

// A leaderboard_back_button onClick eventje
void leaderboard_back_button_onClick(menu_item*);

// A diff_select_easy_button onClick eventje
void diff_select_easy_button_onClick(menu_item*);

// A diff_select_normal_button onClick eventje
void diff_select_normal_button_onClick(menu_item*);

// A diff_select_hard_button onClick eventje
void diff_select_hard_button_onClick(menu_item*);
#endif