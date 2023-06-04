#include <stdlib.h>
#include "basic_constructs.h"
#include "_2d_object.h"



#define WINDOW_HEIGHT 720
#define WINDOW_WIDTH 1280

Ll_2d_object* dispose_Ll_2d_object_node(Ll_2d_object* node) {
    Ll_2d_object* next = node->next;
    free(node->obj.collider_circles.array);
    free(node);
    return next;
}

void rajzol_2d_object(SDL_Renderer *renderer, _2d_object *obj) {
    int w = obj->texture.size.w;
    int h = obj->texture.size.h;
    if(obj->texture.position.x < 0 || obj->texture.position.y < 0){
        SDL_QueryTexture(obj->texture.texture, NULL, NULL, &w, &h);
    }
    SDL_Rect src = { obj->texture.position.x, obj->texture.position.y, w, h };
    SDL_Rect dest = { obj->pos.x - (obj->size.w/2), obj->pos.y - (obj->size.h/2) , obj->size.w, obj->size.h };
    SDL_RenderCopyEx(renderer, obj->texture.texture, &src, &dest, obj->texture.rotation, NULL, SDL_FLIP_NONE);
}

void adjust_collision_boxes(_2d_object *obj) {
    double PI = 4 * atan(1);
    for(int i = 0; i < obj->collider_circles.length; i++){
        obj->collider_circles.array[i].current_pos = (Point){
                obj->pos.x + obj->collider_circles.array[i].offset_abs * cos(((obj->collider_circles.array[i].offset_rotation + obj->texture.rotation - 90) / 180) * PI),
                obj->pos.y + obj->collider_circles.array[i].offset_abs * sin(((obj->collider_circles.array[i].offset_rotation + obj->texture.rotation - 90) / 180) * PI)
        };
    }
}

void render_remaining_lives(SDL_Renderer* renderer, _2d_object player, int max_lives, int remaining_lives){
    player.pos = (Point){WINDOW_WIDTH - player.size.w / 2, 0 + player.size.h / 2};
    player.texture.rotation = 0;
    player.size = (Size){40, 40};
    SDL_SetTextureAlphaMod(player.texture.texture, 255);
    for(int i = 0; i < remaining_lives; i++){
        rajzol_2d_object(renderer, &player);
        player.pos.x = player.pos.x - (player.size.w + 10);
    }
    SDL_SetTextureAlphaMod(player.texture.texture, 100);
    for(int i = 0; i < max_lives - remaining_lives; i++){
        rajzol_2d_object(renderer, &player);
        player.pos.x = player.pos.x - (player.size.w + 10);
    }
        SDL_SetTextureAlphaMod(player.texture.texture, 255);
}

bool check_collision_between_2_obj(_2d_object *a, _2d_object *b) {
    for(int i = 0; i < a->collider_circles.length; i++){
        for(int j = 0; j < b->collider_circles.length; j++){
            int rad_a = a->collider_circles.array[i].radius;
            int rad_b = b->collider_circles.array[j].radius;
            if(point_dist(a->collider_circles.array[i].current_pos, b->collider_circles.array[j].current_pos) <= (double)(rad_a + rad_b)){
                return true;
            }
        }
    }
    return false;
}

void append_collider_to_array(Array_ColliderCircle *arr, ColliderCircle coll) {
    arr->length += 1;
    arr->array = realloc(arr->array, sizeof(ColliderCircle) * (arr->length));
    if (arr->array == NULL)
        exit(-10);
    arr->array[arr->length - 1] = coll;
}

Array_ColliderCircle init_Array_ColliderCircle() {
    return (Array_ColliderCircle){NULL, 0};
}

Ll_2d_object* append_2d_object_to_Ll(Ll_2d_object* list, _2d_object obj){
    Ll_2d_object* uj = malloc(sizeof(Ll_2d_object));
    uj->obj = obj;
    uj->next = NULL;

    if(list == NULL){
        return uj;
    }
    else
    {
        Ll_2d_object* utolso = find_last_node(list);
        utolso->next = uj;
        return list;
    }
}

Ll_2d_object* delete_item_from_Ll(Ll_2d_object* list, Ll_2d_object* delete){
    if(list == delete){
        return dispose_Ll_2d_object_node(list);
    }
    else{
        Ll_2d_object* mozg = list;
        while(mozg->next != NULL){
            if(mozg->next == delete){
                 mozg->next = dispose_Ll_2d_object_node(mozg->next);
                 return list;
            }
            mozg = mozg->next;
        }
        printf("A keresett elem nem találhato.");
        return list;
    }
}
Ll_2d_object* find_last_node(Ll_2d_object* list){
    while (list->next != NULL)
        list = list->next;
    return list;
}

void destroy_Ll_2d_object(Ll_2d_object* list){
    if(list != NULL){
        if(list->next != NULL){
            destroy_Ll_2d_object(list->next);
        }
        dispose_Ll_2d_object_node(list);
    }
}
