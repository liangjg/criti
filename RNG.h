//
// Created by xaq on 10/22/17.
//

#ifndef TTYW_RNG_H
#define TTYW_RNG_H

/* linear congruential RNGs
 * r(k) = S(k)/p
 * S(k+1) = [ g * S(k) + c ] mod p
 * r(k) = psuedo-random number,  0 <= r(k) <= 1
 * S(k) = seed
 * g = multiplier
 * c = increment, 0 or 1
 * p = modulus, p = 2^m
 */

typedef struct RNG{
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
} RNG_t;

extern RNG_t *base_RNG;

#ifdef __cplusplus
extern "C" {
#endif

double get_rand();
void set_RNG_paras(int type);
void get_rand_seed();

#ifdef __cplusplus
}
#endif

#endif //TTYW_RNG_H
