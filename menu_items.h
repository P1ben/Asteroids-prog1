#ifndef MENU_ITEMS_H
#define MENU_ITEMS_H

#include <SDL_ttf.h>
#include "basic_constructs.h"
#include "debugmalloc.h"

// A gombok állapotait tároló felsorolás
enum button_states{
    button_normal,
    button_hovered,
    button_pressed
};

// Az effektek irányát meghatárózó állapotok
enum direction{
    dir_up,
    dir_down,
    dir_left,
    dir_right,
    dir_in,
    dir_out
};

// Effekt típusok
enum effect_type{
    no_effect,
    move_vertical,
    move_horizontal,
    blink
};

struct menu_item;

// Egy menu_itemhez tartozó effektet tartalmazó struktúra
typedef struct Effect{
    enum effect_type effect_type;                                       // Az effekt típusa
    enum direction direction;                                           // Az effekt iránya
    int param0;                                                         // 1. paraméter
    int param1;                                                         // 2. paraméter
    int param2;                                                         // 3. paraméter
    int param3;                                                         // 4. paraméter
    void (*setNextEffect)(struct menu_item*);                           // Az effekt végetérte után végrehajtandó függvényre mutató pointer
} Effect;

struct Submenu;

// Egy menüben található objektumot (pl. gomb) definiáló struktúra
typedef struct menu_item{
    Texture texture;                                                    // Alapállapotban használandó textúra
    Texture texture_hover;                                              // A kurzor hover esetén használandó textúra
    Texture texture_pressed;                                            // A menu_itemre kattintás közben használandó textúra
    Point pos;                                                          // A menu_item pozíciója (középpont)
    Size size;                                                          // A menu_item mérete
    bool clickable;                                                     // Kattintható? (true -> igen, false -> nem)
    void (*onClickEvent)(struct menu_item*);                            // Kattintás esetén végrehajtandó függvényre mutató pointer
    void (*onHoverEvent)(struct menu_item*);                            // Hover esetén végrehajtandó függvényre mutató pointer
    struct Submenu* submenu;                                            // Melyik submenu-höz tartozik az adott menu_item
    Effect effect;                                                      // A menu_itemhez rendelt effekt
} menu_item;

// Egy szöveget, illetve annak attribútumait tartalmazó struktúra
typedef struct menu_text{
    char* content;                                                      // A menu_text által tárolt szöveg
    SDL_Color color;                                                    // A szöveg színe
    TTF_Font* font;                                                     // A szöveg betűtípusa
    int alpha;                                                          // A szöveg áttetszősége (0 - 255)
    Point pos;                                                          // A szöveg pozíciója
    Size size;                                                          // A szöveg mérete
} menu_text;

// Egy menu_itemekre mutató pointereket tartalmazó dinamikus tömb (Almenü)
typedef struct Submenu{
    menu_item **items;                                                  // A menu_item pointereket tartalmazó dinamikus tömb
    int length;                                                         // A dinamikus tömb hossza
} Submenu;

// Létrehoz egy új almenüt
Submenu init_Submenu();

// Hozzáad egy menu_itemet egy almenühöz
void append_menu_item_to_Submenu(Submenu* submenu, menu_item* menu_item);

// Egy almenü összes elemére alkalmazza a rajtuk lévő effektet, majd továbbadja őket a renderernek.
void render_Submenu(SDL_Renderer* renderer, Submenu* submenu);

// Megnézi, hogy a kurzor egy adott menu_item-en helyezkedik-e el
bool is_hovered(menu_item* item, Point cursor_pos);

// Átad egy menu_item-et a renderernek
void menu_item_rajzol(SDL_Renderer* renderer, menu_item* item, enum button_states button_state);

// Alkalmazza ehy menu_itemre a hozzárendelt effektet
void execute_effect_on_menu_item(menu_item* item);

// Hozzárendel egy effektet egy almenü összes objektumához
void apply_Effect_to_Submenu(Submenu* submenu, Effect effect);

// Egy megadott menu_itemhez hozzárendel egy villogás effektet
void set_title_effect_to_blink(menu_item* item);

// Leveszi egy menu_itemről a rajta lévő effektet, majd elindítja a játékot
void start_game(menu_item* item);

// A menüt visszaviszi a nehézségválasztó képernyőről a főmenübe
void back_from_diff_screen(menu_item* item);

// Kirajzol egy szöveget
void render_text(SDL_Renderer* renderer, menu_text* item);

#endif
