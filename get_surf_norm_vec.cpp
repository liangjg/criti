//
// Created by xaq on 9/3/17.
//

#include "surface.h"

void get_surf_norm_vec(surface_t *obj, const double pos[3], double *surf_norm_vec){
    double x = pos[0];
    double y = pos[1];
    double z = pos[2];

    switch(obj->type){
        case P: {
            surf_norm_vec[0] = obj->paras[0];
            surf_norm_vec[1] = obj->paras[1];
            surf_norm_vec[2] = obj->paras[2];
            break;
        }
        case PX: {
            surf_norm_vec[0] = 1;
            surf_norm_vec[1] = 0;
            surf_norm_vec[2] = 0;
            break;
        }
        case PY: {
            surf_norm_vec[0] = 0;
            surf_norm_vec[1] = 1;
            surf_norm_vec[2] = 0;
            break;
        }
        case PZ: {
            surf_norm_vec[0] = 0;
            surf_norm_vec[1] = 0;
            surf_norm_vec[2] = 1;
            break;
        }
        case SO: {
            surf_norm_vec[0] = x;
            surf_norm_vec[1] = y;
            surf_norm_vec[2] = z;
            break;
        }
        case S: {
            surf_norm_vec[0] = x - obj->paras[0];
            surf_norm_vec[1] = y - obj->paras[1];
            surf_norm_vec[2] = z - obj->paras[2];
            break;
        }
        case SX: {
            surf_norm_vec[0] = x - obj->paras[0];
            surf_norm_vec[1] = y;
            surf_norm_vec[2] = z;
            break;
        }
        case SY: {
            surf_norm_vec[0] = x;
            surf_norm_vec[1] = y - obj->paras[0];
            surf_norm_vec[2] = z;
            break;
        }
        case SZ: {
            surf_norm_vec[0] = x;
            surf_norm_vec[1] = y;
            surf_norm_vec[2] = z - obj->paras[0];
            break;
        }
        case CX: {
            surf_norm_vec[0] = 0;
            surf_norm_vec[1] = y;
            surf_norm_vec[2] = z;
            break;
        }
        case CY: {
            surf_norm_vec[0] = x;
            surf_norm_vec[1] = 0;
            surf_norm_vec[2] = z;
            break;
        }
        case CZ: {
            surf_norm_vec[0] = x;
            surf_norm_vec[1] = y;
            surf_norm_vec[2] = 0;
            break;
        }
        default:{
            puts("unknown surface type.");
            exit(0);
        }
    }

    double length = ONE / sqrt(SQUARE(surf_norm_vec[0]) + SQUARE(surf_norm_vec[1]) + SQUARE(surf_norm_vec[2]));
    surf_norm_vec[0] *= length;
    surf_norm_vec[1] *= length;
    surf_norm_vec[2] *= length;
}