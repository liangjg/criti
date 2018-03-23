//
// Created by xaq on 9/3/17.
//

#include "surface.h"


surface_t *
surf_init()
{
    surface_t *_new_surf = (surface_t *) malloc(sizeof(surface_t));
    memset(_new_surf, 0x0, sizeof(surface_t));
    return _new_surf;
}

void
surf_free(surface_t *obj)
{
    free(obj);
}