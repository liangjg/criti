//
// Created by xaq on 10/22/17.
//

#include "RNG.h"


typedef unsigned long long ULL;

/* -------------------------- private prototypes ---------------------------- */
void _get_skip_para(RNG_t *RNG, ULL skip, ULL *GK, ULL *CK);

/* -------------------------- API implementation ---------------------------- */
double get_rand(RNG_t *RNG){
    double res = (double)RNG->seed * RNG->norm;
    RNG->seed = (((RNG->mult * RNG->seed) & RNG->mask) + RNG->add) & RNG->mask;
    return res;
}

void set_RNG_paras(RNG_t *RNG, int type){
    RNG->seed0 = 1;
    RNG->seed = RNG->seed0;
    RNG->stride = 100000;
    RNG->position = 0;
    RNG->position_pre = -1000;

    switch(type){
        case 1:
            RNG->mult = 19073486328125;
            RNG->add = 0;
            RNG->mod = 1ULL << 48;
            RNG->mask = RNG->mod - 1;
            RNG->norm = 1.0 / (double) RNG->mod;
            break;
        case 2:
            RNG->mult = 3512401965023503517;
            RNG->add = 0;
            RNG->mod = 1ULL << 63;
            RNG->mask = RNG->mod - 1;
            RNG->norm = 1.0 / (double) RNG->mod;
            break;
        case 3:
            RNG->mult = 9219741426499971445;
            RNG->add = 1;
            RNG->mod = 1ULL << 63;
            RNG->mask = RNG->mod - 1;
            RNG->norm = 1.0 / (double) RNG->mod;
            break;
        default:
            break;
    }
}

/* stride RNGs
 * S(k) = g^k*S(0) + c*(g^k-1)/(g-1) mod p
 *      = G(k) + c(k) mod p
 * k = k-th random number
 * G(k) = g^k mod p;
 * c(k) = c*(g^k-1)/(g-1) mod p;
*/

void get_rand_seed(RNG_t *RNG){
    static ULL GK;
    static ULL CK;
    static ULL iseed;
    if(RNG->position_pre != RNG->position - 1){
        _get_skip_para(RNG, RNG->stride, &GK, &CK);
        ULL GK_temp, CK_temp;
        _get_skip_para(RNG, RNG->stride * RNG->position, &GK_temp, &CK_temp);
        iseed = (((GK_temp * RNG->seed0) & RNG->mask) + CK_temp) & RNG->mask;
    }
    else
        iseed = (((GK * iseed) & RNG->mask) + CK) & RNG->mask;
    RNG->seed = iseed;
    RNG->position_pre = RNG->position;
    RNG->position++;
}

void _get_skip_para(RNG_t *RNG, ULL skip, ULL *GK, ULL *CK){
    ULL i = skip & RNG->mask;
    ULL g = RNG->mult;
    ULL c = RNG->add;
    *GK = 1;
    *CK = 0;
    while(i > 0){
        if((i & 1) == 1){
            *GK = (*GK * g) & RNG->mask;
            *CK = (((*CK * g) & RNG->mask) + c) & RNG->mask;
        }
        c = (((g + 1) & RNG->mask) * c) & RNG->mask;
        g = (g * g) & RNG->mask;
        i >>= 1;
    }
}