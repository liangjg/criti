//
// Created by xaq on 2018/1/29.
//

#include "surface.h"


int calc_surf_sense(surface_t *obj, const double pos[3], const double dir[3]){
    double sense;
    double x = pos[0];
    double y = pos[1];
    double z = pos[2];

    switch(obj->type){
        case P:{
            sense = obj->paras[0] * x + obj->paras[1] * y + obj->paras[2] * z - obj->paras[3];
            break;
        }
        case PX:{
            sense = x - obj->paras[0];
            break;
        }
        case PY:{
            sense = y - obj->paras[0];
            break;
        }
        case PZ:{
            sense = z - obj->paras[0];
            break;
        }
        case SO:{
            sense = SQUARE(x) + SQUARE(y) + SQUARE(z) - SQUARE(obj->paras[0]);
            break;
        }
        case S:{
            sense = SQUARE(x - obj->paras[0]) + SQUARE(y - obj->paras[1]) + SQUARE(z - obj->paras[2]) - SQUARE(obj->paras[3]);
            break;
        }
        case SX:{
            sense = SQUARE(x - obj->paras[0]) + SQUARE(y) + SQUARE(z) - SQUARE(obj->paras[1]);
            break;
        }
        case SY:{
            sense = SQUARE(x) + SQUARE(y - obj->paras[0]) + SQUARE(z) - SQUARE(obj->paras[1]);
            break;
        }
        case SZ:{
            sense = SQUARE(x) + SQUARE(y) + SQUARE(z - obj->paras[0]) - SQUARE(obj->paras[1]);
            break;
        }
        case C_X:{
            sense = SQUARE(y - obj->paras[0]) + SQUARE(z - obj->paras[1]) - SQUARE(obj->paras[2]);
            break;
        }
        case C_Y:{
            sense = SQUARE(x - obj->paras[0]) + SQUARE(z - obj->paras[1]) - SQUARE(obj->paras[2]);
            break;
        }
        case C_Z:{
            sense = SQUARE(x - obj->paras[0]) + SQUARE(y - obj->paras[1]) - SQUARE(obj->paras[2]);
            break;
        }
        case CX:{
            sense = SQUARE(y) + SQUARE(z) - SQUARE(obj->paras[0]);
            break;
        }
        case CY:{
            sense = SQUARE(x) + SQUARE(z) - SQUARE(obj->paras[0]);
            break;
        }
        case CZ:{
            sense = SQUARE(x) + SQUARE(y) - SQUARE(obj->paras[0]);
            break;
        }
        default:{
            puts("unknown surface type.");
            release_resource();
            exit(0);
        }
    }

    if(fabs(sense) < EPSILON){
        double norm_vec[3];
        get_surf_norm_vec(obj, pos, norm_vec);
        sense = ZERO;
        for(int i = 0; i < 3; i++)
            sense += dir[i] * norm_vec[i];
    }

    if(GT_ZERO(sense)) return 1;
    else if(LT_ZERO(sense)) return -1;
    else return 0;
}