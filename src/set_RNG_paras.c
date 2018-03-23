//
// Created by xaq on 18-2-26.
//

#include "RNG.h"


void
set_RNG_paras(RNG_t *RNG,
              int type)
{
    RNG->seed0 = 1;
    RNG->seed = RNG->seed0;
    RNG->stride = 100000;
    RNG->position = 0;
    RNG->position_pre = -1000;

    switch(type) {
        case 1:RNG->mult = 19073486328125;
            RNG->add = 0;
            RNG->mod = 1ULL << 48;
            RNG->mask = RNG->mod - 1;
            RNG->norm = 1.0 / (double) RNG->mod;
            break;
        case 2:RNG->mult = 3512401965023503517;
            RNG->add = 0;
            RNG->mod = 1ULL << 63;
            RNG->mask = RNG->mod - 1;
            RNG->norm = 1.0 / (double) RNG->mod;
            break;
        case 3:RNG->mult = 9219741426499971445;
            RNG->add = 1;
            RNG->mod = 1ULL << 63;
            RNG->mask = RNG->mod - 1;
            RNG->norm = 1.0 / (double) RNG->mod;
            break;
    }
}