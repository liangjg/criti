//
// Created by xaq on 11/14/17.
//

#include "neutron_transport.h"
#include "RNG.h"


extern RNG_t RNG_slave;

void rotate_dir(double mu, const double *old_dir, double *new_dir){
    double ksi1, ksi2;
    double r, s, t;

    if(fabs(mu) >= 1.) {   // special handling for 1.0 <= fabs(Mu) <= 1.0001 or more.
        new_dir[0] = mu * old_dir[0];
        new_dir[1] = mu * old_dir[1];
        new_dir[2] = mu * old_dir[2];
        return;
    }
    do {
        ksi1 = 2.0 * get_rand_slave(&RNG_slave) - 1.0;
        ksi2 = 2.0 * get_rand_slave(&RNG_slave) - 1.0;
        r = (ksi1 * ksi1) + (ksi2 * ksi2);
    } while(r > 1.0);
    r = sqrt((1.0 - (mu * mu)) / r);
    ksi1 = ksi1 * r;
    ksi2 = ksi2 * r;

    if(fabs(old_dir[2]) <= 0.9) { // Two ranges for fabs(Mu) < 1.0 .
        s = sqrt(old_dir[0] * old_dir[0] + old_dir[1] * old_dir[1]);
        t = 1.0 / s;
        new_dir[0] = old_dir[0] * mu + (ksi1 * old_dir[0] * old_dir[2] - ksi2 * old_dir[1]) * t;
        new_dir[1] = old_dir[1] * mu + (ksi1 * old_dir[1] * old_dir[2] + ksi2 * old_dir[0]) * t;
        new_dir[2] = old_dir[2] * mu - ksi1 * s;
    } //renormalize every 50 calls to prevent error buildup.    {}
    else {
        //special handling for the case of exceptionally large Dir_Old[2].
        s = sqrt(old_dir[0] * old_dir[0] + old_dir[2] * old_dir[2]);
        t = 1.0 / s;
        new_dir[0] = old_dir[0] * mu + (ksi1 * old_dir[0] * old_dir[1] + ksi2 * old_dir[2]) * t;
        new_dir[1] = old_dir[1] * mu - ksi1 * s;
        new_dir[2] = old_dir[2] * mu + (ksi1 * old_dir[2] * old_dir[1] - ksi2 * old_dir[0]) * t;
    }
}