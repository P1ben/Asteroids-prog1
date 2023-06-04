#include <stdio.h>
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_ttf.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL_image.h>
#include "basic_constructs.h"
#include "menu_items.h"
#include "menu_item_functions.h"
#include "file_handler.h"
#include "debugmalloc.h"

#define WINDOW_HEIGHT 720
#define PLAYER_NAME_MAX_LENGTH 16
#define WINDOW_WIDTH 1280
#define GAME_TICK_DELAY 10
#define CHECK printf("check");

/* ablak letrehozasa */
void sdl_init(char const *, int, int, SDL_Window **, SDL_Renderer **);

// Inicializál egy új ablakot
void sdl_init(const char *felirat, int szeles, int magas, SDL_Window **pwindow, SDL_Renderer **prenderer) {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        SDL_Log("Nem indithato az SDL: %s", SDL_GetError());
        exit(1);
    }
    SDL_Window *window = SDL_CreateWindow(felirat, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, szeles, magas, 0);
    if (window == NULL) {
        SDL_Log("Nem hozhato letre az ablak: %s", SDL_GetError());
        exit(1);
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (renderer == NULL) {
        SDL_Log("Nem hozhato letre a megjelenito: %s", SDL_GetError());
        exit(1);
    }
    SDL_RenderClear(renderer);

    *pwindow = window;
    *prenderer = renderer;
}
Uint32 idozit(Uint32 ms, void *param) {
    SDL_Event ev;
    ev.type = SDL_USEREVENT;
    ev.user.code = event_game_tick;
    SDL_PushEvent(&ev);
    return ms;   /* ujabb varakozas */
}

int main(int argc, char *argv[]) {
    /* ablak letrehozasa */
    debugmalloc_log_file("debugmaloc_log.txt");
    SDL_Window *window;
    SDL_Renderer *renderer;
    sdl_init("Asteroids", WINDOW_WIDTH, WINDOW_HEIGHT, &window, &renderer);
    int score = 0;
    int counter = 0;
    int player_name_curr_len = 0;
    char player_name[PLAYER_NAME_MAX_LENGTH + 1];
    TTF_Init();
    srand(time(NULL));

    // Textúrák betöltése

    SDL_Texture_Array menu_textures = init_SDL_Texture_Array();

    SDL_Texture *main_buttons = load_texture(renderer, "text0.png", &menu_textures);
    SDL_Texture *difficulty_buttons = load_texture(renderer, "text1.png", &menu_textures);
    SDL_Texture *logo_text = load_texture(renderer, "logo.png", &menu_textures);
    SDL_Texture *background = load_texture(renderer, "background.jpg", &menu_textures);

    TTF_Font *arcade_font = TTF_OpenFont("ARCADE_N.TTF", 100);
    if (arcade_font == NULL) {
        SDL_Log("Nem sikerult megnyitni a fontot! %s\n", TTF_GetError());
        exit(1);
    }

    SDL_TimerID id = SDL_AddTimer(GAME_TICK_DELAY, idozit, NULL);

    // Gombok, illetve szövegdobozok inicializálása

    menu_text score_text = {
        malloc((6 + 1) * sizeof(char)),
        (SDL_Color){255, 255, 255},
        arcade_font,
        100,
        (Point){WINDOW_WIDTH / 2, 150},
        (Size){480, 80}
    };
    menu_text enter_name_text = {
        "ENTER YOUR NAME:",
        (SDL_Color){255, 255, 255},
        arcade_font,
        100,
        (Point){WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2},
        (Size){650, 60}
    };
    menu_text leaderboard_not_available = {
        "NO RECORDS FOUND!",
        (SDL_Color){255, 255, 255},
        arcade_font,
        0,
        (Point){WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2},
        (Size){300, 60}
    };
    menu_text leaderboard_placement_container = {
        malloc((2 + 1) * sizeof(char)),
        (SDL_Color){255, 255, 255},
        arcade_font,
        0,
        (Point){140, 0},
        (Size){0, 35}
    };
    menu_text leaderboard_name_container = {
        malloc((PLAYER_NAME_MAX_LENGTH + 1) * sizeof(char)),
        (SDL_Color){255, 255, 255},
        arcade_font,
        0,
        (Point){WINDOW_WIDTH / 2 - 100, 0},
        (Size){0, 35}
    };
    menu_text leaderboard_score_container = {
        malloc((6 + 1) * sizeof(char)),
        (SDL_Color){255, 255, 255},
        arcade_font,
        0,
        (Point){WINDOW_WIDTH - 180, 0},
        (Size){300, 35}
    };

    menu_text press_enter_text = {
        "PRESS [ENTER] TO SAVE SCORE",
        (SDL_Color){255, 255, 255},
        arcade_font,
        100,
        (Point){WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 + 200},
        (Size){1000, 60}
    };
    menu_text leaderboard_score_text_label = {
        "SCORE",
        (SDL_Color){255, 255, 255},
        arcade_font,
        0,
        (Point){WINDOW_WIDTH - 180, 130},
        (Size){200, 35}
    };
    menu_text leaderboard_name_text_label = {
        "NAME",
        (SDL_Color){255, 255, 255},
        arcade_font,
        0,
        (Point){WINDOW_WIDTH / 2 - 100, 130},
        (Size){160, 35}
    };
    menu_text leaderboard_placement_text_label = {
        "RANK",
        (SDL_Color){255, 255, 255},
        arcade_font,
        0,
        (Point){140, 130},
        (Size){160, 35}
    };

    convert_score_to_string(counter, score_text.content);

    menu_text player_name_input_text = {
        malloc((PLAYER_NAME_MAX_LENGTH + 1) * sizeof(char)),
        (SDL_Color){255, 255, 255},
        arcade_font,
        200,
        (Point){WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 + 100},
        (Size){1100, 100}
    };

    for(int i = 0; i < PLAYER_NAME_MAX_LENGTH; i++){
        player_name_input_text.content[i] = '_';
    }
    player_name_input_text.content[PLAYER_NAME_MAX_LENGTH] = '\0';
    // Background
    menu_item background_item = {
        (Texture){background, {-1, -1}, {0, 0}},
        (Texture){background, {-1, -1}, {0, 0}},
        (Texture){background, {-1, -1}, {0, 0}},
        {WINDOW_WIDTH, WINDOW_HEIGHT},
        {WINDOW_WIDTH*2, WINDOW_HEIGHT*2},
        false,
        NULL,
        NULL,
        NULL,
        (Effect){no_effect, 0, 0, 0, 0}
    };

    menu_item final_score_text = {
        (Texture){main_buttons, {0, 1800}, {3231, 300}},
        (Texture){main_buttons, {0, 1800}, {3231, 300}},
        (Texture){main_buttons, {0, 1800}, {3231, 300}},
        {WINDOW_WIDTH/2, 50},
        {619, 50},
        false,
        NULL,
        NULL,
        NULL,
        (Effect){blink, dir_out, 50, 255, 20, 255}
    };

    menu_item logo = {
        (Texture){logo_text, {-1, -1}, {0, 0}},
        (Texture){logo_text, {-1, -1}, {0, 0}},
        (Texture){logo_text, {-1, -1}, {0, 0}},
        {WINDOW_WIDTH/2, WINDOW_HEIGHT/2 - 200},
        {879, 154},
        false,
        NULL,
        NULL,
        NULL,
        (Effect){blink, dir_out, 50, 255, 2, 255}
    };
    // Play button:
    menu_item play_button = {
        (Texture){main_buttons, {0, 0}, {1175, 300}},
        (Texture){main_buttons, {0, 300}, {1175, 300}},
        (Texture){main_buttons, {0, 300}, {1175, 300}},
        {WINDOW_WIDTH/2, WINDOW_HEIGHT/2},
        {200, 50},
        true,
        play_button_onClick,
        NULL,
        NULL,
        (Effect){no_effect}
    };

    // Leaderboard button

    menu_item leaderboard_button = {
        (Texture){main_buttons, {0, 600}, {3231, 300}},
        (Texture){main_buttons, {0, 900}, {3231, 300}},
        (Texture){main_buttons, {0, 900}, {3231, 300}},
        {WINDOW_WIDTH/2, WINDOW_HEIGHT/2 + 75},
        {619, 50},
        true,
        leaderboard_button_onClick,
        NULL,
        NULL,
        (Effect){no_effect}
    };
    menu_item leaderboard_title_text = {
        (Texture){main_buttons, {0, 600}, {3231, 300}},
        (Texture){main_buttons, {0, 600}, {3231, 300}},
        (Texture){main_buttons, {0, 600}, {3231, 300}},
        {WINDOW_WIDTH/2, WINDOW_HEIGHT + 50},
        {619, 50},
        false,
        NULL,
        NULL,
        NULL,
        (Effect){no_effect}
    };
    // Exit button:
    menu_item exit_button = {
        (Texture){main_buttons, {0, 1200}, {1175, 300}},
        (Texture){main_buttons, {0, 1500}, {1175, 300}},
        (Texture){main_buttons, {0, 1500}, {1175, 300}},
        {WINDOW_WIDTH/2, WINDOW_HEIGHT/2 + 150},
        {200, 50},
        true,
        exit_button_onClick,
        NULL,
        NULL,
        (Effect){no_effect}
    };

    // Easy button:

    menu_item easy_button = {
        (Texture){difficulty_buttons, {0, 0}, {1175, 300}},
        (Texture){difficulty_buttons, {0, 300}, {1175, 300}},
        (Texture){difficulty_buttons, {0, 300}, {1175, 300}},
        {WINDOW_WIDTH * 1.5, WINDOW_HEIGHT/2},
        {200, 50},
        true,
        diff_select_easy_button_onClick,
        NULL,
        NULL,
        (Effect){no_effect}
    };

    // Normal button

    menu_item normal_button = {
        (Texture){difficulty_buttons, {0, 600}, {1770, 300}},
        (Texture){difficulty_buttons, {0, 900}, {1770, 300}},
        (Texture){difficulty_buttons, {0, 900}, {1770, 300}},
        {WINDOW_WIDTH * 1.5, WINDOW_HEIGHT/2 + 75},
        {339, 50},
        true,
        diff_select_normal_button_onClick,
        NULL,
        NULL,
        (Effect){no_effect}
    };

    // Hard button:

    menu_item hard_button = {
        (Texture){difficulty_buttons, {0, 1200}, {1175, 300}},
        (Texture){difficulty_buttons, {0, 1500}, {1175, 300}},
        (Texture){difficulty_buttons, {0, 1500}, {1175, 300}},
        {WINDOW_WIDTH + WINDOW_WIDTH / 2, WINDOW_HEIGHT/2 + 150},
        {200, 50},
        true,
        diff_select_hard_button_onClick,
        NULL,
        NULL,
        (Effect){no_effect}
    };

    menu_item diff_back_button = {
        (Texture){difficulty_buttons, {0, 1800}, {1175, 300}},
        (Texture){difficulty_buttons, {0, 2100}, {1175, 300}},
        (Texture){difficulty_buttons, {0, 2100}, {1175, 300}},
        {WINDOW_WIDTH * 1.5, WINDOW_HEIGHT/2 + 250},
        {200, 50},
        true,
        diff_back_button_onClick,
        NULL,
        NULL,
        (Effect){no_effect}
    };

    menu_item leaderboard_back_button = {
        (Texture){difficulty_buttons, {0, 1800}, {1175, 300}},
        (Texture){difficulty_buttons, {0, 2100}, {1175, 300}},
        (Texture){difficulty_buttons, {0, 2100}, {1175, 300}},
        {WINDOW_WIDTH / 2, WINDOW_HEIGHT + WINDOW_HEIGHT - 80},
        {200, 50},
        false,
        leaderboard_back_button_onClick,
        NULL,
        NULL,
        (Effect){no_effect, 0, 0, 0, 0, 0, 0, NULL}
    };

    // Az inicializált gombok almenükbe rendezése

    Submenu background_sub = init_Submenu();
    append_menu_item_to_Submenu(&background_sub, &background_item);

    Submenu main = init_Submenu();
    append_menu_item_to_Submenu(&main, &logo);
    append_menu_item_to_Submenu(&main, &play_button);
    append_menu_item_to_Submenu(&main, &leaderboard_button);
    append_menu_item_to_Submenu(&main, &exit_button);

    Submenu difficulty_select = init_Submenu();
    append_menu_item_to_Submenu(&difficulty_select, &easy_button);
    append_menu_item_to_Submenu(&difficulty_select, &normal_button);
    append_menu_item_to_Submenu(&difficulty_select, &hard_button);
    append_menu_item_to_Submenu(&difficulty_select, &diff_back_button);

    Submenu leaderboard = init_Submenu();
    append_menu_item_to_Submenu(&leaderboard, &leaderboard_back_button);
    append_menu_item_to_Submenu(&leaderboard, &leaderboard_title_text);

    Array_file_entry leaderboard_entries = {NULL, 0};
    bool render_leaderboard = false;
    bool ask_for_score = false;
    //SDL_SetTextureAlphaMod(play_button_normal, 100);
    enum difficulty diff = normal;
    bool quit = false;
    while(!quit){
        SDL_Event event;
        SDL_WaitEvent(&event);
        switch (event.type)
        {
            case SDL_USEREVENT:
                switch (event.user.code)
                {
                    case event_quit:
                        quit = true;
                        break;
                    case event_diff_select_easy:
                        diff = easy;
                        apply_Effect_to_Submenu(&difficulty_select, (Effect){move_vertical, dir_up, WINDOW_HEIGHT, 24, 0, 0, start_game});
                        apply_Effect_to_Submenu(&background_sub, (Effect){move_vertical, dir_up, WINDOW_HEIGHT / 4, 6, 0, 0, NULL});
                        break;
                    case event_diff_select_normal:
                        diff = normal;
                        apply_Effect_to_Submenu(&difficulty_select, (Effect){move_vertical, dir_up, WINDOW_HEIGHT, 24, 0, 0, start_game});
                        apply_Effect_to_Submenu(&background_sub, (Effect){move_vertical, dir_up, WINDOW_HEIGHT / 4, 6, 0, 0, NULL});
                        break;
                    case event_diff_select_hard:
                        diff = hard;
                        apply_Effect_to_Submenu(&difficulty_select, (Effect){move_vertical, dir_up, WINDOW_HEIGHT, 24, 0, 0, start_game});
                        apply_Effect_to_Submenu(&background_sub, (Effect){move_vertical, dir_up, WINDOW_HEIGHT / 4, 6, 0, 0, NULL});
                        break;
                    case event_start_game:;
                        score = GameView(renderer, diff, background_sub);

                        if(score > 0) ask_for_score = true;

                        else if(score != -1) send_user_event(event_name_input_to_main);

                        break;
                    case event_name_input_to_main:;
                        apply_Effect_to_Submenu(&difficulty_select, (Effect){move_vertical, dir_down, WINDOW_HEIGHT, 24, 0, 0, back_from_diff_screen});
                        apply_Effect_to_Submenu(&background_sub, (Effect){move_vertical, dir_down, WINDOW_HEIGHT / 4, 6, 0, 0, NULL});

                        // Save score and player name
                        player_name_curr_len = 0;
                        for(int i = 0; i < PLAYER_NAME_MAX_LENGTH; i++){
                            player_name_input_text.content[i] = '_';
                        }
                        player_name_input_text.content[PLAYER_NAME_MAX_LENGTH] = '\0';
                        strcpy(player_name, "");
                        counter = 0;
                        break;
                    case event_leaderboard_to_main:
                        destroy_Array_file_entry(&leaderboard_entries);
                        render_leaderboard = false;
                        leaderboard_name_container.alpha = 0;
                        leaderboard_score_container.alpha = 0;
                        leaderboard_placement_container.alpha = 0;
                        leaderboard_name_text_label.alpha = 0;
                        leaderboard_score_text_label.alpha = 0;
                        leaderboard_placement_text_label.alpha = 0;
                        leaderboard_not_available.alpha = 0;
                        apply_Effect_to_Submenu(&main, (Effect){move_vertical, dir_down, WINDOW_HEIGHT, 24, 0, 0, NULL});
                        logo.effect = (Effect){move_vertical, dir_down, WINDOW_HEIGHT, 24, 0, 0, set_title_effect_to_blink};
                        apply_Effect_to_Submenu(&leaderboard, (Effect){move_vertical, dir_down, WINDOW_HEIGHT, 24, 0, 0, NULL});
                        apply_Effect_to_Submenu(&background_sub, (Effect){move_vertical, dir_down, WINDOW_HEIGHT / 4, 6, 0, 0, NULL});
                        break;
                    case event_main_to_leaderboard:
                        render_leaderboard = true;
                        leaderboard_entries = read_file_contents("leaderboard.txt");
                        Array_file_entry_top10(&leaderboard_entries);
                        apply_Effect_to_Submenu(&main, (Effect){move_vertical, dir_up, WINDOW_HEIGHT, 24, 0, 0, NULL});
                        apply_Effect_to_Submenu(&leaderboard, (Effect){move_vertical, dir_up, WINDOW_HEIGHT, 24, 0, 0, NULL});
                        apply_Effect_to_Submenu(&background_sub, (Effect){move_vertical, dir_up, WINDOW_HEIGHT / 4, 6, 0, 0, NULL});

                        break;
                    case event_main_to_diff_select:
                        apply_Effect_to_Submenu(&main, (Effect){move_horizontal, dir_left, WINDOW_WIDTH, 40, 0, 0, NULL});
                        apply_Effect_to_Submenu(&difficulty_select, (Effect){move_horizontal, dir_left, WINDOW_WIDTH, 40, 0, 0, NULL});
                        apply_Effect_to_Submenu(&background_sub, (Effect){move_horizontal, dir_left, WINDOW_WIDTH / 4, 10, 0, 0, NULL});

                        break;
                    case event_diff_select_to_main:
                        apply_Effect_to_Submenu(&main, (Effect){move_horizontal, dir_right, WINDOW_WIDTH, 40, 0, 0, NULL});
                        logo.effect = (Effect){move_horizontal, dir_right, WINDOW_WIDTH, 40, 0, 0, set_title_effect_to_blink};
                        apply_Effect_to_Submenu(&difficulty_select, (Effect){move_horizontal, dir_right, WINDOW_WIDTH, 40, 0, 0, NULL});
                        apply_Effect_to_Submenu(&background_sub, (Effect){move_horizontal, dir_right, WINDOW_WIDTH / 4, 10, 0, 0, NULL});
                        break;
                    case event_game_tick:
                        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                        SDL_RenderClear(renderer);
                        SDL_PumpEvents();
                        render_Submenu(renderer, &background_sub);
                        render_Submenu(renderer, &main);
                        render_Submenu(renderer, &difficulty_select);
                        render_Submenu(renderer, &leaderboard);

                        // A dicsőséglista kiiratása, csak akkor, ha a menü a leaderboard pozícióban van, illetve az átfedés befejeződött
                        if(render_leaderboard && leaderboard_title_text.effect.effect_type == no_effect){
                            if(leaderboard_placement_text_label.alpha < 250){
                                leaderboard_name_text_label.alpha += 10;
                                leaderboard_score_text_label.alpha += 10;
                                leaderboard_placement_text_label.alpha += 10;
                            }
                            if(leaderboard_name_container.alpha < 180){
                                leaderboard_name_container.alpha += 5;
                                leaderboard_score_container.alpha += 5;
                                leaderboard_placement_container.alpha += 5;
                                leaderboard_not_available.alpha += 5;
                            }
                            if(leaderboard_entries.length == 0){
                                leaderboard_not_available.size.w = strlen(leaderboard_not_available.content) * 50;
                                render_text(renderer, &leaderboard_not_available);
                            }
                            else{
                                int y = 190;
                                render_text(renderer, &leaderboard_placement_text_label);
                                render_text(renderer, &leaderboard_name_text_label);
                                render_text(renderer, &leaderboard_score_text_label);

                                for(int i = 0; i < leaderboard_entries.length; i++){
                                    if(i == 0){
                                        leaderboard_name_container.color = (SDL_Color){255, 255, 0};
                                        leaderboard_score_container.color = (SDL_Color){255, 255, 0};
                                        leaderboard_placement_container.color = (SDL_Color){255, 255, 0};
                                    }
                                    else{
                                        leaderboard_name_container.color = (SDL_Color){255, 255, 255};
                                        leaderboard_score_container.color = (SDL_Color){255, 255, 255};
                                        leaderboard_placement_container.color = (SDL_Color){255, 255, 255};
                                    }
                                    strcpy(leaderboard_name_container.content, leaderboard_entries.obj[i].name);
                                    convert_score_to_string(leaderboard_entries.obj[i].score, leaderboard_score_container.content);
                                    sprintf(leaderboard_placement_container.content, "%d", i+1);
                                    leaderboard_name_container.size.w = strlen(leaderboard_name_container.content) * 40;
                                    //leaderboard_score_container.size.w = strlen(leaderboard_score_container.content) * 50;
                                    leaderboard_placement_container.size.w = strlen(leaderboard_placement_container.content) * 40;
                                    leaderboard_name_container.pos.y = y;
                                    leaderboard_score_container.pos.y = y;
                                    leaderboard_placement_container.pos.y = y;
                                    render_text(renderer, &leaderboard_name_container);
                                    render_text(renderer, &leaderboard_score_container);
                                    render_text(renderer, &leaderboard_placement_container);
                                    y += 40;

                                }
                            }
                        }

                        // A játékos nevének bekérése:
                        if(ask_for_score){
                            if(score - counter == 1){
                                SDL_SetTextureAlphaMod(final_score_text.texture.texture, 255);
                                render_text(renderer, &player_name_input_text);
                                render_text(renderer, &enter_name_text);
                                render_text(renderer, &press_enter_text);
                                counter += 1;
                            }
                            else if(score > counter){
                                counter += ((score - counter) / 50) + 1;
                                execute_effect_on_menu_item(&final_score_text);
                            }
                            else
                            {
                                bool done = false;
                                SDL_StartTextInput();
                                while (!done) {
                                    SDL_Event event;
                                    if (SDL_PollEvent(&event)) {
                                        switch (event.type) {
                                            case SDL_KEYDOWN:
                                                if (event.key.keysym.sym == SDLK_BACKSPACE){
                                                    if(player_name_curr_len > 0){
                                                        player_name_input_text.content[player_name_curr_len - 1] = '\0';
                                                        player_name[player_name_curr_len - 1] = '\0';
                                                        player_name_curr_len--;

                                                    }
                                                    done = true;
                                                }
                                                else if (event.key.keysym.sym == SDLK_RETURN && strlen(player_name) > 0){
                                                    ask_for_score = false;
                                                    send_user_event(event_name_input_to_main);
                                                    done = true;
                                                    Array_file_entry array =  read_file_contents("leaderboard.txt");


                                                    file_entry a;
                                                    strcpy(a.name, player_name);
                                                    a.score = score;


                                                    beszur_elem_majd_rendez(&array, a);

                                                    save_scores_in_file("leaderboard.txt", &array);
                                                    destroy_Array_file_entry(&array);
                                                }
                                            break;
                                            case SDL_QUIT:
                                                send_user_event(event_quit);
                                                done = true;
                                                break;
                                            case SDL_TEXTINPUT:
                                                // Engedélyezett karakterek
                                                if(player_name_curr_len < PLAYER_NAME_MAX_LENGTH && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_", event.text.text[0])){
                                                    player_name[player_name_curr_len] = event.text.text[0];
                                                    player_name[player_name_curr_len + 1] = '\0';
                                                    player_name_curr_len++;

                                                    done = true;
                                                }
                                                break;
                                        }
                                    }
                                }
                                for(int i = 0; i < player_name_curr_len; i++)
                                    player_name_input_text.content[i] = player_name[i];

                                for(int i = player_name_curr_len; i < PLAYER_NAME_MAX_LENGTH; i++)
                                    player_name_input_text.content[i] = '_';

                                player_name_input_text.content[PLAYER_NAME_MAX_LENGTH] = '\0';
                                render_text(renderer, &player_name_input_text);
                                render_text(renderer, &enter_name_text);
                                render_text(renderer, &press_enter_text);
                            }
                            convert_score_to_string(counter, score_text.content);
                            render_text(renderer, &score_text);
                            menu_item_rajzol(renderer, &final_score_text, button_normal);
                        }
                        SDL_RenderPresent(renderer);
                        break;
                }
            break;

            case SDL_QUIT:
                quit = true;
                break;
        }
    }
    /* Cleanup */
    destroy_Array_file_entry(&leaderboard_entries);
    free(background_sub.items);
    free(main.items);
    free(difficulty_select.items);
    free(leaderboard.items);
    free(leaderboard_name_container.content);
    free(leaderboard_score_container.content);
    free(leaderboard_placement_container.content);
    TTF_CloseFont(arcade_font);
    free_items_in_SDL_Texture_Array(&menu_textures);
    free(score_text.content);
    free(player_name_input_text.content);
    SDL_Quit();
    return 0;
}
