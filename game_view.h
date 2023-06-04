#ifndef GAME_VIEW_H
#define GAME_VIEW_H

#include "debugmalloc.h"

// A játékot tartalmazó függvény, visszatérési értéke a megszerzett pontszám
int GameView(SDL_Renderer* renderer, enum difficulty diff, Submenu background_sub);

#endif