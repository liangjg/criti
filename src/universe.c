//
// Created by xaq on 10/7/17.
//

#include "universe.h"


/* ----------------------------- API implementation ------------------------- */
universe_t *univ_init() {
    universe_t *_new_univ = (universe_t *)malloc(sizeof(universe_t));
    memset(_new_univ, 0x0, sizeof(universe_t));
    return _new_univ;
}

void univ_free(universe_t *obj){
    free(obj->cells);
    free(obj->filled_lat_univs);
    if(obj->neighbor_lists){
        map_free(obj->neighbor_lists);
        free(obj->neighbor_lists);
    }
    free(obj);
}