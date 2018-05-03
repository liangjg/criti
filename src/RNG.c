//
// Created by xaq on 18-2-26.
//

#include "RNG.h"


/* -------------------------- private prototypes ---------------------------- */
static void
_get_skip_para(RNG_t *RNG,
               ULL skip,
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
        _get_skip_para(RNG, RNG->stride, &GK, &CK);
        ULL GK_temp, CK_temp;
        _get_skip_para(RNG, RNG->stride * RNG->position, &GK_temp, &CK_temp);
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
_get_skip_para(RNG_t *RNG,
               ULL skip,
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
