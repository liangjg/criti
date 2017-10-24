//
// Created by xaq on 10/22/17.
//

#include "RNG.h"

typedef unsigned long long ULL;

RNG_t *base_RNG = new RNG_t;

/* -------------------------- private prototypes ---------------------------- */
void _get_skip_para(ULL skip, ULL *GK, ULL *CK);


/* -------------------------- API implementation ---------------------------- */
double get_rand(){
    double res = (double)base_RNG->seed * base_RNG->norm;
    base_RNG->seed = (((base_RNG->mult * base_RNG->seed) & base_RNG->mask) + base_RNG->add) & base_RNG->mask;
    return res;
}

void set_RNG_paras(int type){
    switch(type){
        case 1:
            base_RNG->mult = 19073486328125;
            base_RNG->add = 0;
            base_RNG->mod = 1ULL << 48;
            base_RNG->mask = base_RNG->mod - 1;
            base_RNG->norm = 1.0 / (double) base_RNG->mod;
            break;
        case 2:
            base_RNG->mult = 3512401965023503517;
            base_RNG->add = 0;
            base_RNG->mod = 1ULL << 63;
            base_RNG->mask = base_RNG->mod - 1;
            base_RNG->norm = 1.0 / (double) base_RNG->mod;
            break;
        case 3:
            base_RNG->mult = 9219741426499971445;
            base_RNG->add = 1;
            base_RNG->mod = 1ULL << 63;
            base_RNG->mask = base_RNG->mod - 1;
            base_RNG->norm = 1.0 / (double) base_RNG->mod;
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

void get_rand_seed(){
    static ULL GK;
    static ULL CK;
    static ULL iseed;
    if(base_RNG->position_pre != base_RNG->position - 1){
        _get_skip_para(base_RNG->stride, &GK, &CK);
        ULL GK_temp, CK_temp;
        _get_skip_para(base_RNG->stride * base_RNG->position, &GK_temp, &CK_temp);
        iseed = (((GK_temp * base_RNG->seed0) & base_RNG->mask) + CK_temp) & base_RNG->mask;
    }
    else
        iseed = (((GK * iseed) & base_RNG->mask) + CK) & base_RNG->mask;
    base_RNG->seed = iseed;
    base_RNG->position_pre = base_RNG->position;
    base_RNG->position++;
}

void _get_skip_para(ULL skip, ULL *GK, ULL *CK){
    ULL i = skip & base_RNG->mask;
    ULL g = base_RNG->mult;
    ULL c = base_RNG->add;
    *GK = 1;
    *CK = 0;
    while(i > 0){
        if((i & 1) == 1){
            *GK = (*GK * g) & base_RNG->mask;
            *CK = (((*CK * g) & base_RNG->mask) + c) & base_RNG->mask;
        }
        c = (((g + 1) & base_RNG->mask) * c) & base_RNG->mask;
        g = (g * g) & base_RNG->mask;
        i >>= 1;
    }
}