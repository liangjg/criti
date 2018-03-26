//
// Created by xaq on 10/22/17.
//

#ifndef CRITI_RNG_H
#define CRITI_RNG_H


typedef unsigned long long ULL;

/* linear congruential RNGs
 * r(k) = S(k)/p
 * S(k+1) = [ g * S(k) + c ] mod p
 * r(k) = psuedo-random number,  0 <= r(k) <= 1
 * S(k) = seed
 * g = multiplier
 * c = increment, 0 or 1
 * p = modulus, p = 2^m
 */

typedef struct RNG {
    unsigned long long mult;
    unsigned long long add;
    unsigned long long mod;
    unsigned long long mask;
    double norm;
    unsigned long long seed;
    unsigned long long seed0;
    unsigned long long stride;
    int position;
    int position_pre;
    ULL GK;
    ULL CK;
    ULL iseed;
} RNG_t;

#ifdef __cplusplus
extern "C" {
#endif

double
get_rand_slave(RNG_t *RNG);

double
get_rand_host(RNG_t *RNG);

void
get_rand_seed_slave(RNG_t *RNG);

void
get_rand_seed_host(RNG_t *RNG);

void
set_RNG_paras(RNG_t *RNG,
              int type);

#ifdef __cplusplus
}
#endif

#endif //CRITI_RNG_H
