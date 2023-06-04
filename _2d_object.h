#ifndef _2D_OBJECT_H
#define _2D_OBJECT_H

#include <SDL.h>
#include "basic_constructs.h"
#include "debugmalloc.h"

// Az objektumok típuskódjait tartalmazó felsorolás
enum object_type {
    obj_player,
    obj_laser,
    obj_asteroid_big,
    obj_asteroid_medium,
    obj_asteroid_small
};

// Egy ütközés érzékelő kör struktúrája
typedef struct ColliderCircle{
    double offset_abs;                              // Középpontjának abszolút távolsága az őt tartalmazó _2d_object középpontjától
    double offset_rotation;                         // Melyik irányba van értelmezve az offset_abs
    Point current_pos;                              // A ColliderCircle pillanatnyi pozíciója (ezt az értéket általában függvények kezelik)
    int radius;                                     // A ColliderCircle sugara
} ColliderCircle;

// Egy ütközés érzékelő köröket tartalmazó dinamikus tömböt tartalmazó struktúra
typedef struct Array_ColliderCircle{
    ColliderCircle* array;                          // Dinamikus tömb, melynek minden tagja egy ColliderCircle
    int length;                                     // A dinamikus tömb hossza
} Array_ColliderCircle;

// Egy 2d objektum struktúrája
typedef struct _2d_object{
    int type;                                       // Az objektum típusa (enum object_type)
    Texture texture;                                // Az objektum textúrája
    Point pos;                                      // Az objektum pozíciója (középpont)
    Size size;                                      // Az objektum mérete
    double speed_abs;                               // Az objektum abszolút sebessége
    double speed_ang;                               // Az objektum forgásának szögsebessége
    double rotation;                                // Az objektum pillanatnyi elforgatása
    Array_ColliderCircle collider_circles;          // Az objektumhoz rendelt ütközésérzékelő köröket tartalmazó tömb
} _2d_object;

// Egy _2d_objecteket tartalmazó láncolt lista nodeja
typedef struct Ll_2d_object{
    _2d_object obj;                                 // Maga az objektum
    struct Ll_2d_object* next;                      // A listában a következő objektumra mutató pointer
}Ll_2d_object;

// Átad egy objektumot a renderernek
void rajzol_2d_object(SDL_Renderer* renderer, _2d_object* object);

// Felszabadít egy elemet egy láncolt listában
Ll_2d_object* dispose_Ll_2d_object_node(Ll_2d_object* node);

// Töröl egy elemet egy láncolt listából, melyet fel is szabadít
Ll_2d_object* delete_item_from_Ll(Ll_2d_object* list, Ll_2d_object* node_to_delete);

// Visszatér egy láncolt lista utolsó elemére mutató pointerrel
Ll_2d_object* find_last_node(Ll_2d_object* list);

// Hozzáad egy új listaelemet egy láncolt lista végére
Ll_2d_object* append_2d_object_to_Ll(Ll_2d_object* list, _2d_object new_obj);

// Felszabadítja az összes elemet egy láncolt listában
void destroy_Ll_2d_object(Ll_2d_object* list);

// Kirajzolja a képernyőre a játékos fennmaradó életeinek számát
void render_remaining_lives(SDL_Renderer* render, _2d_object player, int max_lives, int current_lives);

// Beállítja a collision boxok abzolút helyzetét
void adjust_collision_boxes(_2d_object* object);

// Két objektum között ellenőrzi az ütközést (true, ha ütköznek; false, ha nem)
bool check_collision_between_2_obj(_2d_object* obj_a, _2d_object* obj_b);

// Inicializál egy új dinamikusan foglalt tömböt, melybe ütközésérzékelő körök kerülnek
Array_ColliderCircle init_Array_ColliderCircle();

// Egy CollisionCircle-ből álló tömbhöz hozzáad egy új elemet
void append_collider_to_array(Array_ColliderCircle* array, ColliderCircle circle);

#endif
