//
// Created by xaq on 11/14/17.
//

#include "neutron_transport.h"
#include "RNG.h"

void rotate_dir(double mu, const double *old_dir, double *new_dir){
    double dksi1, dksi2;
    double r, s, t;

    if(fabs(mu) >= 1.) {   // special handling for 1.0 <= fabs(Mu) <= 1.0001 or more.
        new_dir[0] = mu * old_dir[0];
        new_dir[1] = mu * old_dir[1];
        new_dir[2] = mu * old_dir[2];
        return;
    }
    do {
        dksi1 = 2.0 * get_rand() - 1.0;
        dksi2 = 2.0 * get_rand() - 1.0;
        r = (dksi1 * dksi1) + (dksi2 * dksi2);
    } while(r > 1.0);
    r = sqrt((1.0 - (mu * mu)) / r);
    dksi1 = dksi1 * r;
    dksi2 = dksi2 * r;

    if(fabs(old_dir[2]) <= 0.9) { // Two ranges for fabs(Mu) < 1.0 .
        s = sqrt(old_dir[0] * old_dir[0] + old_dir[1] * old_dir[1]);
        t = 1.0 / s;
        new_dir[0] = old_dir[0] * mu + (dksi1 * old_dir[0] * old_dir[2] - dksi2 * old_dir[1]) * t;
        new_dir[1] = old_dir[1] * mu + (dksi1 * old_dir[1] * old_dir[2] + dksi2 * old_dir[0]) * t;
        new_dir[2] = old_dir[2] * mu - dksi1 * s;
    } //renormalize every 50 calls to prevent error buildup.    {}
    else {
        //special handling for the case of exceptionally large Dir_Old[2].
        s = sqrt(old_dir[0] * old_dir[0] + old_dir[2] * old_dir[2]);
        t = 1.0 / s;
        new_dir[0] = old_dir[0] * mu + (dksi1 * old_dir[0] * old_dir[1] + dksi2 * old_dir[2]) * t;
        new_dir[1] = old_dir[1] * mu - dksi1 * s;
        new_dir[2] = old_dir[2] * mu + (dksi1 * old_dir[2] * old_dir[1] - dksi2 * old_dir[0]) * t;
    }
}