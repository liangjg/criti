//
// Created by xaq on 2018/1/29.
//

#include "universe.h"


void trans_univ_coord(universe_t *obj, double pos[3], double dir[3]) {
    int i, j;

    if(obj->is_moved)
        for(i = 0; i < 3; i++)
            pos[i] -= obj->origin[i];

    if(obj->is_rotated){
        double pos_temp[3];
        double dir_temp[3];
        for(i = 0; i < 3; i++){
            pos_temp[i] = pos[i];
            dir_temp[i] = dir[i];
        }

        for(i = 0; i < 3; i++){
            pos[i] = 0;
            dir[i] = 0;
            for(j = 0; j < 3; j++){
                pos[i] += obj->rotation[i][j] * pos_temp[j];
                dir[i] += obj->rotation[i][j] * dir_temp[j];
            }
        }
    }
}