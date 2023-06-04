#include "menu_items.h"
#include "basic_constructs.h"
#include <SDL.h>


void check_button_state(SDL_Renderer* renderer, menu_item* button){
    Point cursor_pos;
    int a, b;
    bool left_down = SDL_GetMouseState(&a, &b) & SDL_BUTTON(1);
    cursor_pos = (Point){(double)a, (double)b};
    if(is_hovered(button, cursor_pos)){
        if(left_down && button->clickable){
            if(button->onClickEvent != NULL){
                button->onClickEvent(button);
            }
            menu_item_rajzol(renderer, button, button_pressed);
        }
        else{
            if(button->onHoverEvent != NULL){
                button->onHoverEvent(button);
            }
            menu_item_rajzol(renderer, button, button_hovered);
        }
    }
    else{
        menu_item_rajzol(renderer, button, button_normal);
    }
}

void play_button_onClick(menu_item* self){
    send_user_event(event_main_to_diff_select);
}

void diff_back_button_onClick(menu_item* self){
    send_user_event(event_diff_select_to_main);
}

void exit_button_onClick(menu_item* self){
    send_user_event(event_quit);
}
void leaderboard_button_onClick(menu_item* self){
    send_user_event(event_main_to_leaderboard);
}
void leaderboard_back_button_onClick(menu_item* self){
    send_user_event(event_leaderboard_to_main);
}
void diff_select_easy_button_onClick(menu_item* self){
    send_user_event(event_diff_select_easy);
}
void diff_select_normal_button_onClick(menu_item* self){
    send_user_event(event_diff_select_normal);
}
void diff_select_hard_button_onClick(menu_item* self){
    send_user_event(event_diff_select_hard);
}
