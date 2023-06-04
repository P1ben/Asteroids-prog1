#include <stdio.h>
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL_image.h>
#include "basic_constructs.h"
#include "menu_items.h"
#include "_2d_object.h"
#include "objects.h"
#include <string.h>
#include "game_view.h"

#define WINDOW_HEIGHT 720
#define WINDOW_WIDTH 1280
#define GAME_TICK_DELAY 10
#define SHOOTING_DELAY 20
#define CHECK printf("check");

int GameView(SDL_Renderer* renderer, enum difficulty diff, Submenu background_sub) {

    // Textúrák betöltése
    SDL_Texture_Array game_textures = init_SDL_Texture_Array();
    SDL_Texture *raketa = load_texture(renderer, "raketa.png", &game_textures);
    SDL_Texture *asteroid_small = load_texture(renderer, "asteroid_small_0.png", &game_textures);
    SDL_Texture *asteroid_medium = load_texture(renderer, "asteroid_medium_0.png", &game_textures);
    SDL_Texture *asteroid_big = load_texture(renderer, "asteroid_big_0.png", &game_textures);
    SDL_Texture *laser_texture = load_texture(renderer, "laser.png", &game_textures);

    // Font betöltése
    TTF_Font *arcade_font = TTF_OpenFont("ARCADE_N.TTF", 100);
    if (arcade_font == NULL) {
        SDL_Log("Nem sikerult megnyitni a fontot! %s\n", TTF_GetError());
        exit(1);
    }
    double score_multiplier;
    int asteroid_spawn_rate;
    int asteroid_big_spawn_chance;
    int asteroid_medium_spawn_chance;
    int asteroid_small_spawn_chance;
    int num_o_lives;
    char difficulty_name[10];

    int current_score = 0, score_counter = 0;
    int safe_time_counter = 200;

    // Nehézségi szint beállítása
    switch(diff){
        case easy:
            score_multiplier = 0.5;
            asteroid_big_spawn_chance = 10;
            asteroid_medium_spawn_chance = 30;
            asteroid_small_spawn_chance = 60;
            num_o_lives = 5;
            asteroid_spawn_rate = 1;
            strcpy(difficulty_name, "easy");
            break;
        case normal:
            score_multiplier = 1.;
            asteroid_big_spawn_chance = 20;
            asteroid_medium_spawn_chance = 50;
            asteroid_small_spawn_chance = 30;
            num_o_lives = 4;
            asteroid_spawn_rate = 2;
            strcpy(difficulty_name, "normal");
            break;
        case hard:
            score_multiplier = 1.5;
            asteroid_big_spawn_chance = 40;
            asteroid_medium_spawn_chance = 40;
            asteroid_small_spawn_chance = 20;
            num_o_lives = 3;
            asteroid_spawn_rate = 3;
            strcpy(difficulty_name, "hard");
            break;
    }
    int lives_remaining = num_o_lives;

    // Szövegdobozok inicializálása
    menu_text difficulty_text = {
        difficulty_name,
        (SDL_Color){255, 255, 255},
        arcade_font,
        100,
        (Point){(strlen(difficulty_name) * 40) / 2, 0 + 25},
        (Size){strlen(difficulty_name) * 40, 50}
    };
    menu_text score_text = {
        "SCORE",
        (SDL_Color){255, 255, 255},
        arcade_font,
        100,
        (Point){0 + 100 + 400, 0 + 25},
        (Size){200, 50}
    };
    menu_text score_number_text = {
        malloc((6 + 1) * sizeof(char)),
        (SDL_Color){255, 255, 255},
        arcade_font,
        100,
        (Point){0 + 150 + 400 + 200 + 25, 0 + 25},
        (Size){300, 50}
    };
    convert_score_to_string(score_counter, score_number_text.content);
    Ll_2d_object* asteroid_list = NULL;
    Ll_2d_object* laser_list = NULL;

    // Player objektum inicializálása
    _2d_object player = (_2d_object){
        obj_player,
        (Texture){
            raketa,
            (Point){-1, -1},
            (Size){0, 0},
            0},
        (Point){WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2},
        (Size){50, 50},
        0,
        0,
        0,
        init_Array_ColliderCircle()}; //Player
    
    append_collider_to_array(&player.collider_circles, (ColliderCircle){20, 0, (Point){0, 0}, 5});
    append_collider_to_array(&player.collider_circles, (ColliderCircle){30, 140, (Point){0, 0}, 5});
    append_collider_to_array(&player.collider_circles, (ColliderCircle){30, 220, (Point){0, 0}, 5});
    adjust_collision_boxes(&player);
    double PI = 4 * atan(1);
    unsigned int ticks_elapsed = 0;
    unsigned int shooting_delay = 0;
    bool quit = false;
    bool kd_w = false, kd_a = false, kd_d = false, kd_space = false;
    while (!quit) {
        if(ticks_elapsed > (60 / GAME_TICK_DELAY) * 120)
            ticks_elapsed = 0;
        SDL_Event event;
        SDL_WaitEvent(&event);
        switch (event.type) {
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym){
                    case SDLK_w:
                        kd_w = true;
                        break;
                    case SDLK_a:
                        kd_a = true;
                        break;
                    case SDLK_d:
                        kd_d = true;
                        break;
                    case SDLK_SPACE:
                        kd_space = true;
                        break;
                }

                break;
            case SDL_KEYUP:
                switch(event.key.keysym.sym){
                    case SDLK_w:
                        kd_w = false;
                        break;
                    case SDLK_a:
                        kd_a = false;
                        break;
                    case SDLK_d:
                        kd_d = false;
                        break;
                    case SDLK_SPACE:
                        kd_space = false;
                        break;
                }

                break;

            case SDL_USEREVENT:
                ticks_elapsed++;
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderClear(renderer);
                render_Submenu(renderer, &background_sub);

                // Aszteroida spawnolása
                if((ticks_elapsed) % ((1000 / GAME_TICK_DELAY) / asteroid_spawn_rate) == 0 && safe_time_counter == 0){
                    int a = (rand() % 100) + 1;
                    if(isInRange(a, 0, asteroid_big_spawn_chance))
                        asteroid_list = spawn_asteroid(asteroid_list, obj_asteroid_big, asteroid_big, (Point){0, 0}, true);
                    else if(isInRange(a, asteroid_big_spawn_chance, (asteroid_big_spawn_chance + asteroid_medium_spawn_chance)))
                        asteroid_list = spawn_asteroid(asteroid_list, obj_asteroid_medium, asteroid_medium, (Point){0, 0}, true);
                    else
                        asteroid_list = spawn_asteroid(asteroid_list, obj_asteroid_small, asteroid_small, (Point){0, 0}, true);
                }

                // Sebesség megváltoztatása az inputok alapján
                if(kd_a){
                    player.rotation -= 0.5;
                    if(player.speed_ang > -2){
                        player.speed_ang -= 0.5;
                    }
                }
                else if(kd_d){
                    player.rotation += 0.5;
                    if(player.speed_ang < 2){
                        player.speed_ang += 0.5;
                    }
                }
                else{
                    if(player.speed_ang < 0){
                        player.speed_ang += 0.1;
                    }
                    else if(player.speed_ang > 0){
                        player.speed_ang -= 0.1;
                    }
                }
                if(kd_w)
                {
                    if(player.speed_abs < 7.5)
                    {
                        player.speed_abs += 0.4;
                    }
                }
                else
                {
                    if(player.speed_abs > 0)
                    {
                        if(player.speed_abs >= 0.3)
                            player.speed_abs -= 0.3;
                        else
                            player.speed_abs = 0;
                    }
                }

                // Lövés
                if(kd_space && shooting_delay == 0){
                    laser_list = generate_laser(laser_list, &player, laser_texture);
                      shooting_delay = SHOOTING_DELAY;
                }

                // A player mozgatása, illetve forgatása
                player.rotation += player.speed_ang;
                player.pos.x += player.speed_abs * cos(((player.rotation - 90) / 180) * PI);
                player.pos.y += player.speed_abs * sin(((player.rotation - 90) / 180) * PI);

                // A player forgásának 0 - 360 között tartása
                if(player.rotation > 360)
                    player.rotation -= 360;
                else if(player.rotation < 0)
                    player.rotation += 360;
                player.texture.rotation = player.rotation;

                // A pálya elhagyásának érzékelése
                check_if_outside_player(&player);

                // Az ütközésérzékelő körök pozíciójának frisítése
                adjust_collision_boxes(&player);
                Ll_2d_object* mozg = laser_list;

                // Ez a loop ellenőrzi, hogy történt e ütközés egy aszteroida és egy lövedék között
                // Amennyiben történt, a szétlőtt aszteroidát kisebbekre robbantja szét, illetve a lézert törli
                while(mozg != NULL){
                    adjust_collision_boxes(&mozg->obj);
                    bool destroys = false;
                    if(check_if_outside(mozg->obj)){
                        Ll_2d_object* del = mozg;
                        mozg = mozg->next;
                        laser_list = delete_item_from_Ll(laser_list, del);
                        continue;
                    }
                    Ll_2d_object* asteroid_mozg = asteroid_list;
                    while(asteroid_mozg != NULL){
                        if(check_collision_between_2_obj(&asteroid_mozg->obj, &mozg->obj)){
                            switch(asteroid_mozg->obj.type){
                                case obj_asteroid_big:
                                    current_score += 50 * score_multiplier;
                                    asteroid_list = spawn_asteroid(asteroid_list, obj_asteroid_medium, asteroid_medium, asteroid_mozg->obj.pos, false);
                                    asteroid_list = spawn_asteroid(asteroid_list, obj_asteroid_medium, asteroid_medium, asteroid_mozg->obj.pos, false);

                                    break;
                                case obj_asteroid_medium:
                                    current_score += 20 * score_multiplier;
                                    asteroid_list = spawn_asteroid(asteroid_list, obj_asteroid_small, asteroid_small, asteroid_mozg->obj.pos, false);
                                    asteroid_list = spawn_asteroid(asteroid_list, obj_asteroid_small, asteroid_small, asteroid_mozg->obj.pos, false);
                                    break;
                                case obj_asteroid_small:
                                    current_score += 10 * score_multiplier;
                                    break;
                            }
                            Ll_2d_object* del = asteroid_mozg;
                            asteroid_mozg = asteroid_mozg->next;
                            asteroid_list = delete_item_from_Ll(asteroid_list, del);
                            destroys = true;
                            break;
                        }
                        asteroid_mozg = asteroid_mozg->next;
                    }
                    if(destroys){
                        Ll_2d_object* del = mozg;
                        mozg = mozg->next;
                        laser_list = delete_item_from_Ll(laser_list, del);
                    }
                    else{
                        mozg->obj.pos.x += mozg->obj.speed_abs * cos(((mozg->obj.rotation - 90) / 180) * PI);
                        mozg->obj.pos.y += mozg->obj.speed_abs * sin(((mozg->obj.rotation - 90) / 180) * PI);

                        rajzol_2d_object(renderer, &mozg->obj);

                        mozg = mozg->next;
                    }
                }

                // Ellenőrzi, hogy a játékos nekiment-e egy aszteroidának
                // Amennyiben igen, a játékos elveszít egy életet, vagy visszakerül a főmenübe
                // Ha a játékos csak életet vesztett, újraspawnol a képernyő közepén, eltűnik az összes aszteroida, és elindul egy 2 másodperces safe period
                mozg = asteroid_list;
                while(mozg != NULL){
                    if(check_collision_between_2_obj(&player, &mozg->obj)){
                        // Do cleanup
                        if(lives_remaining == 1){

                            // Cleanup
                            free_items_in_SDL_Texture_Array(&game_textures);
                            free(player.collider_circles.array);
                            destroy_Ll_2d_object(asteroid_list);
                            destroy_Ll_2d_object(laser_list);
                            TTF_CloseFont(arcade_font);
                            free(score_number_text.content);
                            SDL_FlushEvent(SDL_USEREVENT);

                            // return score
                            return current_score;
                        }
                        else{
                            lives_remaining--;
                            destroy_Ll_2d_object(asteroid_list);
                            destroy_Ll_2d_object(laser_list);
                            asteroid_list = NULL;
                            laser_list = NULL;
                            player = (_2d_object){obj_player, (Texture){raketa, {-1, -1}, {0, 0}, 0}, {WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2}, {50, 50}, 0, 0, 0, player.collider_circles};
                            safe_time_counter = 200;
                            break;
                        }
                    }
                    if(check_if_outside(mozg->obj)){
                        Ll_2d_object* del = mozg;
                        mozg = mozg->next;
                        asteroid_list = delete_item_from_Ll(asteroid_list, del);
                        continue;
                    }
                    mozg->obj.pos.x += mozg->obj.speed_abs * cos(((mozg->obj.rotation - 90) / 180) * PI);
                    mozg->obj.pos.y += mozg->obj.speed_abs * sin(((mozg->obj.rotation - 90) / 180) * PI);
                    if(mozg->obj.speed_ang > 0.1 || mozg->obj.speed_ang < -0.1)
                        mozg->obj.texture.rotation += mozg->obj.speed_ang;
                    adjust_collision_boxes(&mozg->obj);

                    rajzol_2d_object(renderer, &mozg->obj);
                    SDL_SetRenderDrawColor( renderer, 255, 0, 0, 255 );
                    mozg = mozg->next;
                }

                // Egyesével növeli a score szövegdoboz tartalmát, amég nem lesz egyenlő a jelenlegi pontszámmal
                if(current_score > score_counter) score_counter++;
                convert_score_to_string(score_counter, score_number_text.content);

                // Kirajzolja a játékost, illetve a szövegdobozokat
                rajzol_2d_object(renderer, &player);
                render_text(renderer, &score_number_text);
                render_text(renderer, &score_text);
                render_text(renderer, &difficulty_text);
                render_remaining_lives(renderer, player, num_o_lives, lives_remaining);

                // Kirajzol mindent a képernyőre.
                SDL_RenderPresent(renderer);
                if (shooting_delay > 0) shooting_delay--;
                if(safe_time_counter > 0) safe_time_counter--;
                break;
            case SDL_QUIT:
                // Cleanup
                free_items_in_SDL_Texture_Array(&game_textures);
                free(player.collider_circles.array);
                destroy_Ll_2d_object(asteroid_list);
                destroy_Ll_2d_object(laser_list);
                TTF_CloseFont(arcade_font);
                free(score_number_text.content);
                send_user_event(event_quit);

                // A -1 return value miatt a program kilép
                return -1;
                break;
        }
    }
}
