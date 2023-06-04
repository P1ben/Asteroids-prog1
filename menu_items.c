#include "basic_constructs.h"
#include "menu_items.h"
#include <stdbool.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string.h>


bool is_hovered(menu_item *button, Point cursor_pos){
    int left = button->pos.x - button->size.w / 2;
    int right = button->pos.x + button->size.w / 2;
    int top = button->pos.y - button->size.h / 2;
    int bottom = button->pos.y + button->size.h / 2;
    if(cursor_pos.x <= right && cursor_pos.x >= left && cursor_pos.y <= bottom && cursor_pos.y >= top)
        return true;
    return false;
}

void render_text(SDL_Renderer* renderer, menu_text* inp){

    if(inp->alpha < 0) inp->alpha = 0;
    if(inp->alpha > 255) inp->alpha = 255;

    SDL_Surface* szoveg = TTF_RenderUTF8_Blended(inp->font, inp->content, inp->color);
    SDL_Texture* szoveg_text = SDL_CreateTextureFromSurface(renderer, szoveg);
    SDL_Rect dest;
    if(inp->size.h < 0 || inp->size.w < 0){
        SDL_QueryTexture(szoveg_text, NULL, NULL, &dest.w, &dest.h);

        dest.x = inp->pos.x - (dest.w / 2);
        dest.y = inp->pos.y - (dest.y / 2);
    }
    else{
        dest.x = inp->pos.x - (inp->size.w / 2);
        dest.y = inp->pos.y - (inp->size.h / 2);
        dest.w = inp->size.w;
        dest.h = inp->size.h;
    }
    SDL_SetTextureAlphaMod(szoveg_text, inp->alpha);
    SDL_RenderCopy(renderer, szoveg_text, NULL, &dest);
    SDL_FreeSurface(szoveg);
    SDL_DestroyTexture(szoveg_text);
}

void menu_item_rajzol(SDL_Renderer *renderer, menu_item *obj, enum button_states state) {
    Texture text;
    if(state == button_normal)
        text = obj->texture;
    else if(state == button_hovered)
        text = obj->texture_hover;
    else
        text = obj->texture_pressed;

    int w = text.size.w;
    int h = text.size.h;
    if(text.position.x < 0 || text.position.y < 0){
        SDL_QueryTexture(text.texture, NULL, NULL, &w, &h);
    }
    SDL_Rect src = { text.position.x, text.position.y, w, h };
    SDL_Rect dest = { obj->pos.x - (obj->size.w/2), obj->pos.y - (obj->size.h/2) , obj->size.w, obj->size.h };

    SDL_RenderCopyEx(renderer, text.texture, &src, &dest, 0, NULL, SDL_FLIP_NONE);
}

Submenu init_Submenu(){
    return (Submenu){NULL, 0};
}

void append_menu_item_to_Submenu(Submenu* sub, menu_item* item){
    sub->items = (menu_item*)realloc(sub->items, sizeof(menu_item*) * (sub->length + 1));
    sub->items[sub->length] = item;
    sub->length += 1;
    item->submenu = sub;
}

void render_Submenu(SDL_Renderer* renderer, Submenu* sub){
    for(int i = 0; i < sub->length; i++){
        execute_effect_on_menu_item(sub->items[i]);
        check_button_state(renderer, sub->items[i]);
    }
}

void apply_Effect_to_Submenu(Submenu* sub, Effect eff){
    for(int i = 0; i < sub->length; i++){
        sub->items[i]->effect = eff;
    }
}
void start_game(menu_item* inp){
    inp->effect = (Effect){no_effect};
    send_user_event(event_start_game);
}

void set_title_effect_to_blink(menu_item* inp){
    inp->effect = (Effect){blink, dir_out, 50, 255, 2, 255, NULL};
}

void back_from_diff_screen(menu_item* inp){
    inp->effect = (Effect){no_effect};
    send_user_event(event_diff_select_to_main);
}

void execute_effect_on_menu_item(menu_item* item){
    if(item->effect.effect_type == no_effect) return;

    switch(item->effect.effect_type){
        case move_horizontal:;
            if(item->clickable) item->clickable = false;
            switch (item->effect.direction){
                case dir_left:;
                    if(item->effect.param0 <= 0){
                       if(item->effect.setNextEffect == NULL){
                            item->effect.effect_type = no_effect;
                        }
                        else{
                            item->effect.setNextEffect(item);
                        }
                        item->clickable = true;
                    }
                    else if(item->effect.param0 < item->effect.param1){
                        item->pos.x -= (double)item->effect.param0;
                        item->effect.param0 = 0;
                    }
                    else
                    {
                        item->pos.x -= (double)item->effect.param1;
                        item->effect.param0 -= item->effect.param1;
                    }
                break;
                case dir_right:
                    if(item->effect.param0 == 0){
                        if(item->effect.setNextEffect == NULL){
                            item->effect.effect_type = no_effect;
                        }
                        else{
                            item->effect.setNextEffect(item);
                        }
                        item->clickable = true;
                    }
                    else if(item->effect.param0 < item->effect.param1){
                        item->pos.x += item->effect.param0;
                        item->effect.param0 = 0;
                    }
                    else
                    {
                        item->pos.x += item->effect.param1;
                        item->effect.param0 -= item->effect.param1;
                    }
                break;
            }
        break;
        case move_vertical:;
            if(item->clickable) item->clickable = false;
            switch (item->effect.direction){
                case dir_up:;
                    if(item->effect.param0 <= 0){
                        item->effect.effect_type = no_effect;
                        if(item->effect.setNextEffect == NULL){
                            item->effect.effect_type = no_effect;
                        }
                        else{
                            item->effect.setNextEffect(item);
                        }
                        item->clickable = true;
                    }
                    else if(item->effect.param0 < item->effect.param1){
                        item->pos.y -= item->effect.param0;
                        item->effect.param0 = 0;
                    }
                    else
                    {
                        item->pos.y -= item->effect.param1;
                        item->effect.param0 -= item->effect.param1;
                    }
                    break;
                case dir_down:
                    if(item->effect.param0 <= 0){
                        item->effect.effect_type = no_effect;
                        if(item->effect.setNextEffect == NULL){
                            item->effect.effect_type = no_effect;
                        }
                        else{
                            item->effect.setNextEffect(item);
                        }
                        item->clickable = true;
                    }
                    else if(item->effect.param0 < item->effect.param1){
                        item->pos.y += item->effect.param0;
                        item->effect.param0 = 0;
                    }
                    else
                    {
                        item->pos.y += item->effect.param1;
                        item->effect.param0 -= item->effect.param1;
                    }
                    break;
            }
            break;
        case blink:
                // direction: in | out
                // param0: alpha_min
                // param1: alpha_max
                // param2: speed
                // param3: current_alpha
                if(item->effect.param0 < 0) item->effect.param0 = 0;
                if(item->effect.param1 > 255) item->effect.param1 = 255;

                switch (item->effect.direction)
                {
                    case dir_in:
                        if(item->effect.param2 + item->effect.param3 > item->effect.param1){
                            item->effect.param3 = item->effect.param1;
                            item->effect.direction = dir_out;
                        }
                        else{
                            item->effect.param3 += item->effect.param2;
                        }
                        SDL_SetTextureAlphaMod(item->texture.texture, item->effect.param3);
                        break;
                    case dir_out:
                        if(item->effect.param3 - item->effect.param2 < item->effect.param0){
                            item->effect.param3 = item->effect.param0;
                            item->effect.direction = dir_in;
                        }
                        else{
                            item->effect.param3 -= item->effect.param2;
                        }
                        SDL_SetTextureAlphaMod(item->texture.texture, item->effect.param3);
                        break;
                break;
            }
            break;
    }
}
