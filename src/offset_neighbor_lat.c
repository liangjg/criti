//
// Created by xaq on 2018/1/29.
//

#include "universe.h"


/* -------------------------- private prototypes ---------------------------- */
static int _offset_neighbor_lat_rect(universe_t *obj, int lat_index, int lat_bound_surf, double pos[3]);

static int _offset_neighbor_lat_hex(universe_t *obj, int lat_index, int lat_bound_surf, double pos[3]);

/* ----------------------------- API implementation ------------------------- */
int offset_neighbor_lat(universe_t *obj, int lat_index, int lat_bound_surf, double pos[3]){
    int new_lat_index;
    if(obj->lattice_type == 1)
        new_lat_index = _offset_neighbor_lat_rect(obj, lat_index, lat_bound_surf, pos);
    else
        new_lat_index = _offset_neighbor_lat_hex(obj, lat_index, lat_bound_surf, pos);

    if(new_lat_index < 1 || new_lat_index > obj->filled_lat_univs_sz){
        puts("offset lattice index out of range.");
        new_lat_index = -1;
    }
    return new_lat_index;
}

/* ------------------------ private API implementation ---------------------- */
int _offset_neighbor_lat_rect(universe_t *obj, int lat_index, int lat_bound_surf, double *pos){
    int new_index = -1;
    switch(lat_bound_surf){
        case 1:
            new_index = lat_index - 1;
            pos[0] += obj->pitch[0];
            break;
        case 2:
            new_index = lat_index + 1;
            pos[0] -= obj->pitch[0];
            break;
        case 3:
            new_index = lat_index - obj->scope[0];
            pos[1] += obj->pitch[1];
            break;
        case 4:
            new_index = lat_index + obj->scope[0];
            pos[1] -= obj->pitch[1];
            break;
        case 5:
            new_index = lat_index - obj->scope[0] * obj->scope[1];
            pos[2] += obj->pitch[2];
            break;
        case 6:
            new_index = lat_index + obj->scope[0] * obj->scope[1];
            pos[2] -= obj->pitch[2];
            break;
    }
    return new_index;
}

int _offset_neighbor_lat_hex(universe_t *obj, int lat_index, int lat_bound_surf, double *pos){
    int i2 = (lat_index - 1) / obj->scope[0];
    int i1 = lat_index - 1 - i2 * obj->scope[0];

    int nNewLatIndex = -1;
    switch(lat_bound_surf) {
        case 1 : { // surface FA
            i1 = i1 - 1;
            pos[0] = pos[0] + obj->pitch[0] ;
            break;
        }
        case 2 : { // surface AB
            i2 = i2 - 1;
            pos[0] = pos[0] + 0.5 * obj->pitch[0] ;
            pos[1] = pos[1] + obj->height ;
            break;
        }
        case 3 : { // surface BC
            i1 = i1 + 1;
            i2 = i2 - 1;
            pos[0] = pos[0] - 0.5 * obj->pitch[0] ;
            pos[1] = pos[1] + obj->height ;
            break;
        }
        case 4 : { // surface CD
            i1 = i1 + 1;
            pos[0] = pos[0] - obj->pitch[0] ;
            break;
        }
        case 5 : { // surface DE
            i2 = i2 + 1;
            pos[0] = pos[0] - 0.5 * obj->pitch[0] ;
            pos[1] = pos[1] - obj->height ;
            break;
        }
        case 6 : { // surface EF
            i1 = i1 - 1;
            i2 = i2 + 1;
            pos[0] = pos[0] + 0.5 * obj->pitch[0] ;
            pos[1] = pos[1] - obj->height ;
            break;
        }
    }

    if(i1 >= 0 && i2 >= 0 && i1 < obj->scope[0] && i2 < obj->scope[1])
        nNewLatIndex = 1 + i1 + obj->scope[0] * i2;

    return nNewLatIndex;
}