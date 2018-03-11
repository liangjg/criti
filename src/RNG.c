//
// Created by xaq on 10/22/17.
//

#include "RNG.h"


/* base_RNG is a global variable declared in main.cpp */
extern RNG_t base_RNG;

/* -------------------------- private prototypes ---------------------------- */
void
_get_skip_para(ULL skip,
               RNG_t *RNG,
               ULL *GK,
               ULL *CK);

/* -------------------------- API implementation ---------------------------- */
double
get_rand(RNG_t *RNG)
{
    double res = (double) RNG->seed * RNG->norm;
    RNG->seed = (((RNG->mult * RNG->seed) & RNG->mask) + RNG->add) & RNG->mask;
    return res;
}

void
set_RNG_paras(int type)
{
    base_RNG.seed0 = 1;
    base_RNG.seed = base_RNG.seed0;
    base_RNG.stride = 100000;
    base_RNG.position = 0;
    base_RNG.position_pre = -1000;

    switch(type) {
    case 1:base_RNG.mult = 19073486328125;
        base_RNG.add = 0;
        base_RNG.mod = 1ULL << 48;
        base_RNG.mask = base_RNG.mod - 1;
        base_RNG.norm = 1.0 / (double) base_RNG.mod;
        break;
    case 2:base_RNG.mult = 3512401965023503517;
        base_RNG.add = 0;
        base_RNG.mod = 1ULL << 63;
        base_RNG.mask = base_RNG.mod - 1;
        base_RNG.norm = 1.0 / (double) base_RNG.mod;
        break;
    case 3:base_RNG.mult = 9219741426499971445;
        base_RNG.add = 1;
        base_RNG.mod = 1ULL << 63;
        base_RNG.mask = base_RNG.mod - 1;
        base_RNG.norm = 1.0 / (double) base_RNG.mod;
        break;
    default:break;
    }
}

/* stride RNGs
 * S(k) = g^k*S(0) + c*(g^k-1)/(g-1) mod p
 *      = G(k) + c(k) mod p
 * k = k-th random number
 * G(k) = g^k mod p;
 * c(k) = c*(g^k-1)/(g-1) mod p;
*/

void
get_rand_seed(RNG_t *RNG)
{
    ULL GK = RNG->GK;
    ULL CK = RNG->CK;
    ULL iseed = RNG->iseed;
    if(RNG->position_pre != RNG->position - 1) {
        _get_skip_para(RNG->stride, RNG, &GK, &CK);
        ULL GK_temp, CK_temp;
        _get_skip_para(RNG->stride * RNG->position, RNG, &GK_temp, &CK_temp);
        iseed = (((GK_temp * RNG->seed0) & RNG->mask) + CK_temp) & RNG->mask;
    } else
        iseed = (((GK * iseed) & RNG->mask) + CK) & RNG->mask;
    RNG->seed = iseed;
    RNG->position_pre = RNG->position;
    RNG->position++;
    RNG->GK = GK;
    RNG->CK = CK;
    RNG->iseed = iseed;
}

void
_get_skip_para(ULL skip,
               RNG_t *RNG,
               ULL *GK,
               ULL *CK)
{
    ULL i = skip & RNG->mask;
    ULL g = RNG->mult;
    ULL c = RNG->add;
    *GK = 1;
    *CK = 0;
    while(i > 0) {
        if((i & 1) == 1) {
            *GK = (*GK * g) & RNG->mask;
            *CK = (((*CK * g) & RNG->mask) + c) & RNG->mask;
        }
        c = (((g + 1) & RNG->mask) * c) & RNG->mask;
        g = (g * g) & RNG->mask;
        i >>= 1;
    }
}