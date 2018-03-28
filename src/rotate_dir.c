//
// Created by xaq on 11/14/17.
//

#include "neutron_transport.h"


void
rotate_dir(double mu,
           const double *old_dir,
           double *new_dir,
           RNG_t *RNG)
{
    double ksi1, ksi2;
    double r, s, t;

    if(fabs(mu) >= ONE) {
        new_dir[0] = mu * old_dir[0];
        new_dir[1] = mu * old_dir[1];
        new_dir[2] = mu * old_dir[2];
        return;
    }
    do {
        ksi1 = TWO * get_rand_slave(RNG) - ONE;
        ksi2 = TWO * get_rand_slave(RNG) - ONE;
        r = (ksi1 * ksi1) + (ksi2 * ksi2);
    } while(r > ONE);
    r = sqrt((ONE - (mu * mu)) / r);
    ksi1 = ksi1 * r;
    ksi2 = ksi2 * r;

    if(fabs(old_dir[2]) <= 0.9) {
        s = sqrt(old_dir[0] * old_dir[0] + old_dir[1] * old_dir[1]);
        t = ONE / s;
        new_dir[0] = old_dir[0] * mu + (ksi1 * old_dir[0] * old_dir[2] - ksi2 * old_dir[1]) * t;
        new_dir[1] = old_dir[1] * mu + (ksi1 * old_dir[1] * old_dir[2] + ksi2 * old_dir[0]) * t;
        new_dir[2] = old_dir[2] * mu - ksi1 * s;
    } else {
        s = sqrt(old_dir[0] * old_dir[0] + old_dir[2] * old_dir[2]);
        t = ONE / s;
        new_dir[0] = old_dir[0] * mu + (ksi1 * old_dir[0] * old_dir[1] + ksi2 * old_dir[2]) * t;
        new_dir[1] = old_dir[1] * mu - ksi1 * s;
        new_dir[2] = old_dir[2] * mu + (ksi1 * old_dir[2] * old_dir[1] - ksi2 * old_dir[0]) * t;
    }
}