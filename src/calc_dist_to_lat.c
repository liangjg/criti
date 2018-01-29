//
// Created by xaq on 2018/1/29.
//

#include "universe.h"


/* -------------------------- private prototypes ---------------------------- */
static double _calc_dist_to_lat_rect(universe_t *obj, const double pos[3], const double dir[3], int *which_surf);

static double _calc_dist_to_lat_hex(universe_t *obj, const double pos[3], const double dir[3], int *which_surf);

/* ----------------------------- API implementation ------------------------- */
double calc_dist_to_lat(universe_t *obj, const double pos[3], const double dir[3], int *which_surf) {
    double distance = -1.0;
    if(obj->lattice_type == 1)
        distance = _calc_dist_to_lat_rect(obj, pos, dir, which_surf);
    else if(obj->lattice_type == 2)
        distance = _calc_dist_to_lat_hex(obj, pos, dir, which_surf);
    else {
        puts("unknown lattice type.");
        base_warnings++;
    }

    return distance;
}

/* ------------------------ private API implementation ---------------------- */
double _calc_dist_to_lat_rect(universe_t *obj, const double pos[3], const double dir[3], int *which_surf) {
    double distance = 1.0E+24;
    double distance_temp;
    int surf_order;

    for(int i = 0; i < 3; i++){
        if(obj->scope[i] == 1 || EQ_ZERO(dir[i]))
            continue;

        if(GT_ZERO(dir[i])){
            distance_temp = (obj->pitch[i] - pos[i]) / dir[i];
            surf_order = 2 * i + 2;
        } else {
            distance_temp = -pos[i] / dir[i];
            surf_order = 2 * i + 1;
        }

        if(distance_temp < distance){
            distance = distance_temp;
            *which_surf = surf_order;
        }
    }

    return distance;
}

double _calc_dist_to_lat_hex(universe_t *obj, const double pos[3], const double dir[3], int *which_surf) {

    double pos1 = pos[0], pos2 = pos[1];
    double dir1 = dir[0], dir2 = dir[1];
    double L1 = obj->pitch[0];    //  L2 = Pitch[1];
    double D2 = 0.5 * L1 * obj->cos_sita + obj->height * obj->sin_sita;


    double distance_temp[7];

    ///// FA & CD
    if(dir1 > 0){
        distance_temp[1] = -1;
        distance_temp[4] = (0.5 * L1 - pos1) / dir1;
    } else {
        distance_temp[1] = (-0.5 * L1 - pos1) / dir1;
        distance_temp[4] = -1;
    }

    ///// AB & DE
    double t1 = dir1 * obj->cos_sita + dir2 * obj->sin_sita;
    double t2 = pos1 * obj->cos_sita + pos2 * obj->sin_sita;
    if(t1 > 0){ //// DE
        distance_temp[2] = -1;
        distance_temp[5] = (0.5 * D2 - t2) / t1;
    } else { //// AB
        distance_temp[2] = (-0.5 * D2 - t2) / t1;
        distance_temp[5] = -1;
    }

    ///// BC & EF
    double t3 = dir1 * obj->cos_sita - dir2 * obj->sin_sita;
    double t4 = pos1 * obj->cos_sita - pos2 * obj->sin_sita;
    if(t3 > 0){ //// BC
        distance_temp[3] = (0.5 * D2 - t4) / t3;
        distance_temp[6] = -1;
    } else { //// EF
        distance_temp[3] = -1;
        distance_temp[6] = (-0.5 * D2 - t4) / t3;
    }

    /////////////////////////////// select minimum distance to lattice boundary //////////////////////
    double distance = 1.0E+24;
    for(int i = 1; i <= 6; ++i){
        if(distance_temp[i] > 0){
            if(distance_temp[i] < distance){
                distance = distance_temp[i];
                *which_surf = i;
            }
        }
    }

    return distance;
}