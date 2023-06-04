#include "_2d_object.h"
#include <time.h>
#include <stdlib.h>
#include <SDL.h>



#define WINDOW_HEIGHT 720
#define WINDOW_WIDTH 1280

bool check_if_outside(_2d_object obj) {
    int counter = 0;
    for(int i = 0; i < obj.collider_circles.length; i++){
        if(obj.rotation > 0 && obj.rotation < 180 && obj.collider_circles.array[i].current_pos.x > WINDOW_WIDTH)
            counter++;
        else if(obj.rotation > 90 && obj.rotation < 270 && obj.collider_circles.array[i].current_pos.y > WINDOW_HEIGHT)
            counter++;
        else if(obj.rotation > 180 && obj.rotation < 360 && obj.collider_circles.array[i].current_pos.x < 0)
            counter++;
        else if((obj.rotation > 270 || obj.rotation < 90) && obj.collider_circles.array[i].current_pos.y < 0)
            counter++;
    }
    return counter == obj.collider_circles.length;
}

void generate_random_pos_outside_boundaries(Point *pos, double *rotation, int obj_radius) {
    double rot;
    int zero_or_max;
    int y_or_x = rand() % 2;
    switch (y_or_x) {
        case 0:;
            zero_or_max = rand() % 2;
            switch (zero_or_max) {
                case 0:
                    pos->y = (double)(WINDOW_HEIGHT + obj_radius);
                    rot = 0;
                    break;
                case 1:
                    pos->y = (double)(0 - obj_radius);
                    rot = 180;
                    break;
            }
            int rand_x = rand() % (WINDOW_WIDTH + 1);
            pos->x = rand_x;
            if((rand_x > WINDOW_WIDTH / 2 && rot < 90) || (rand_x < WINDOW_WIDTH / 2 && rot > 90))
                rot -= rand() % 30;
            else
                rot += rand() % 30;
            break;
        case 1:;
            zero_or_max = rand() % 2;
            switch (zero_or_max) {
                case 0:
                    pos->x = (double)(WINDOW_WIDTH + obj_radius);
                    rot = 270;
                    break;
                case 1:
                    pos->x = (double)(0 - obj_radius);
                    rot = 90;
                    break;
            }
            int rand_y = rand() % (WINDOW_HEIGHT + 1);
            pos->y = rand_y;
            if((rand_y < WINDOW_HEIGHT / 2 && rot > 180) || (rand_y > WINDOW_HEIGHT / 2 && rot < 180))
                rot -= rand() % 30;
            else
                rot += rand() % 30;
            break;
    }
    *rotation = rot;
}

Ll_2d_object* spawn_asteroid(Ll_2d_object* list, enum object_type asteroid_type, SDL_Texture *text, Point starting_point, bool strating_point_is_random) {
    Point pos;
    double rotation;
    int radius;
    int speed = (rand() % 5) + 1;
    int rotation_speed = 2 - (rand() % 4);

    switch ((int)asteroid_type) {
        case obj_asteroid_big:
            radius = 80;
            break;
        case obj_asteroid_medium:
            radius = 50;
            break;
        case obj_asteroid_small:
            radius = 25;
            break;
    }
        generate_random_pos_outside_boundaries(&pos, &rotation, radius);
    if(!strating_point_is_random){
        pos = starting_point;
    }

    _2d_object object = {asteroid_type,
                         (Texture){text, {-1, -1}, {0, 0}, 0},
                         pos,
                         {radius*2, radius*2},
                         speed,
                         rotation_speed,
                         rotation,
                         init_Array_ColliderCircle()};
    append_collider_to_array(&object.collider_circles, (ColliderCircle){0, 0, (Point){0, 0}, radius});

    list = append_2d_object_to_Ll(list, object);
}
Ll_2d_object* generate_laser(Ll_2d_object* list, _2d_object* player, SDL_Texture* text) {
    _2d_object laser = {obj_laser,
                         (Texture){text, {-1, -1}, {0, 0}, player->rotation},
                         player->pos,
                         {20, 40},
                         10,
                         0,
                         player->rotation,
                         init_Array_ColliderCircle()};
    append_collider_to_array(&laser.collider_circles, (ColliderCircle){15, 0, (Point){0, 0}, 5});

    list = append_2d_object_to_Ll(list, laser);
}

bool check_if_outside_player(_2d_object *obj) {
    int counter = 0;
    int dir = 0;
    for(int i = 0; i < obj->collider_circles.length; i++){
        if(obj->rotation > 0 && obj->rotation < 180 && obj->collider_circles.array[i].current_pos.x > WINDOW_WIDTH){
            counter++;
            dir = 0;
        }
        else if(obj->rotation > 90 && obj->rotation < 270 && obj->collider_circles.array[i].current_pos.y > WINDOW_HEIGHT){
            counter++;
            dir = 1;
        }
        else if(obj->rotation > 180 && obj->rotation < 360 && obj->collider_circles.array[i].current_pos.x < 0){
            counter++;
            dir = 2;
        }
        else if((obj->rotation > 270 || obj->rotation < 90) && obj->collider_circles.array[i].current_pos.y < 0){
            counter++;
            dir = 3;
        }
    }
    if(counter == obj->collider_circles.length){
        switch (dir) {
            case 0:
                obj->pos.x = 0 - obj->size.w / 2;
                break;
            case 1:
                obj->pos.y = 0 - obj->size.h / 2;
                break;
            case 2:
                obj->pos.x = WINDOW_WIDTH + obj->size.w / 2;
                break;
            case 3:
                obj->pos.y = WINDOW_HEIGHT + obj->size.h / 2;
                break;
        }
        return true;
    }
    return false;
}
