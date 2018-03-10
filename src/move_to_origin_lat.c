//
// Created by xaq on 2018/1/29.
//

#include "universe.h"


void
move_to_origin_lat(universe_t *obj,
                   int lat_index,
                   double pos[3])
{
    if(obj->lattice_type == 1) {
        int xyz[3];
        xyz[2] = (lat_index - 1) / (obj->scope[0] * obj->scope[1]);
        xyz[1] = (lat_index - 1 - obj->scope[0] * obj->scope[1] * xyz[2]) / obj->scope[0];
        xyz[0] = lat_index - 1 - obj->scope[0] * obj->scope[1] * xyz[2] - obj->scope[0] * xyz[1];

        for(int i = 0; i < 3; i++)
            if(obj->scope[i] != 1)
                pos[i] -= obj->pitch[i] * xyz[i];
    } else {
        double len1 = obj->pitch[0];
        int i2 = (lat_index - 1) / obj->scope[0];
        int i1 = lat_index - 1 - i2 * obj->scope[0];

        pos[0] -= (i1 * len1 + i2 * 0.5 * len1);
        pos[1] -= i2 * obj->height;
    }
}