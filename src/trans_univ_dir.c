//
// Created by xaq on 2018/1/29.
//

#include "universe.h"


void trans_univ_dir(universe_t *obj, double dir[3]) {
    if(obj->is_rotated){
        double dir_temp[3];
        for(int i = 0; i < 3; i++)
            dir_temp[i] = dir[i];

        for(int i = 0; i < 3; i++){
            dir[i] = 0;
            for(int j = 0; j < 3; j++)
                dir[i] += obj->rotation[i][j] * dir_temp[j];
        }
    }
}