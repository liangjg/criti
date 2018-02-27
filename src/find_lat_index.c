//
// Created by xaq on 2018/1/29.
//

#include "universe.h"


/* -------------------------- private prototypes ---------------------------- */
static int _find_lat_index_rect(universe_t *obj, const double pos[3], const double dir[3]);

static int _find_lat_index_hex(universe_t *obj, const double pos[3], const double dir[3]);

/* ----------------------------- API implementation ------------------------- */
int find_lat_index(universe_t *obj, const double pos[3], const double dir[3]) {
    int lat_index = -1;
    int filled_lat_univs_sz = obj->scope[0] * obj->scope[1];

    switch(obj->lattice_type){
        case 1:
            lat_index = _find_lat_index_rect(obj, pos, dir);
            filled_lat_univs_sz *= obj->scope[2];
            break;
        case 2:
            lat_index = _find_lat_index_hex(obj, pos, dir);
            break;
    }

    if(lat_index <= 0 || lat_index > filled_lat_univs_sz){
        puts("failed to locate lattice index.");
        lat_index = -1;
        base_warnings++;
    }
    return lat_index;
}

/* ------------------------ private API implementation ---------------------- */
int _find_lat_index_rect(universe_t *obj, const double pos[3], const double dir[3]) {
    int i;
    int lat_index = 0;
    int xyz[3];
    for(i = 0; i < 3; ++i){
        if(obj->scope[i] == 1)
            xyz[i] = 0;
        else {
            double temp1 = pos[i] / obj->pitch[i];
            int temp2 = (int)(temp1 + 0.5);

            if(fabs(temp1 - temp2) < EPSILON){
                if(dir[i] >= 0)
                    xyz[i] = temp2;
                else
                    xyz[i] = temp2 - 1;
            } else
                xyz[i] = (int)(temp1);
        }
    }
    lat_index = 1 + xyz[0] + obj->scope[0] * xyz[1] + obj->scope[0] * obj->scope[1] * xyz[2];

    return lat_index;
}

int _find_lat_index_hex(universe_t *obj, const double pos[3], const double dir[3]) {
    double pos1 = pos[0], pos2 = pos[1];
    double dir1 = dir[0], dir2 = dir[1];
    double len1 = obj->pitch[0];
    double dis2 = 0.5 * len1 * obj->cos_sita + obj->height * obj->sin_sita;


    double dK2 = pos2 / obj->height;
    double dK1 = (pos1 - dK2 * 0.5 * len1) / len1;
    int i1 = (int)(floor(dK1));
    int i2 = (int)(floor(dK2));
    pos1 = pos1 - (i1 * len1 + i2 * 0.5 * len1);
    pos2 = pos2 - i2 * obj->height;

    double sense1 = pos1 - len1;
    if(fabs(sense1) < EPSILON)
        sense1 = dir1 > 0 ? 1 : -1;
    if(sense1 > 0){
        double sense2 = (pos1 - len1) * obj->cos_sita + pos2 * obj->sin_sita - 0.5 * dis2;
        if(fabs(sense2) < EPSILON){
            double sense_dir = dir1 * obj->cos_sita + dir2 * obj->sin_sita;
            sense2 = sense_dir > 0 ? 1 : -1;
        }
        i1 = i1 + 1;
        if(sense2 > 0)
            i2 = i2 + 1;
    } else {
        double sense3 = pos1 - 0.5 * len1;
        if(fabs(sense3) < EPSILON)
            sense3 = dir1 > 0 ? 1 : -1;
        if(sense3 > 0){
            double sense4 = (pos1 - len1) * obj->cos_sita - pos2 * obj->sin_sita + 0.5 * dis2;
            if(fabs(sense4) < EPSILON){
                double sense_dir = dir1 * obj->cos_sita - dir2 * obj->sin_sita;
                sense4 = sense_dir > 0 ? 1 : -1;
            }
            if(sense4 > 0)
                i1 = i1 + 1;
            else
                i2 = i2 + 1;
        } else {
            double sense5 = pos1 * obj->cos_sita + pos2 * obj->sin_sita - 0.5 * dis2;
            if(fabs(sense5) < EPSILON){
                double sense_dir = dir1 * obj->cos_sita + dir2 * obj->sin_sita;
                sense5 = sense_dir > 0 ? 1 : -1;
            }
            if(sense5 > 0)
                i2 = i2 + 1;
        }
    }

    int nLat_index = -1;
    if(i1 >= 0 && i2 >= 0 && i1 < obj->scope[0] && i2 < obj->scope[1])
        nLat_index = 1 + i1 + obj->scope[0] * i2;

    return nLat_index;
}