#ifndef OBJECTS_H_INCLUDED
#define OBJECTS_H_INCLUDED

#include "debugmalloc.h"

// Megnézi, hogy egy objektum elhagyta-e a képernyőt
bool check_if_outside(_2d_object obj);

// Generál egy új random pozíciót a képernyőn kívül, illetve a hozzá tartozó mozgásirányt
void generate_random_pos_outside_boundaries(Point* pos, double* rotation, int radius);

// Generál egy új aszteroidát, majd hozzáadja egy objetkum láncolt listához
Ll_2d_object* spawn_asteroid(Ll_2d_object* list, enum object_type object_type, SDL_Texture* texture, Point starting_point, bool strating_point_is_random);

// Megnézi, hogy a játékos a képernyőn kívülre került-e,
// és áthelyezi a képernyő másik oldalára, amennyiben igen
bool check_if_outside_player(_2d_object* player);

// Generál egy lézert, majd hozzáadja egy objektum láncolt listához
Ll_2d_object* generate_laser(Ll_2d_object* list, _2d_object* player, SDL_Texture* texture);

#endif // OBJECTS_H_INCLUDED
