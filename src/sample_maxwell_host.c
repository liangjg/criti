//
// Created by xaq on 10/27/17.
//

#include "sample_method.h"


double
sample_maxwell_host(RNG_t *RNG,
                    double T)
{
    //===============================================================================
    // MAXWELL_SPECTRUM samples an energy from the Maxwell fission distribution based
    // on a direct sampling scheme. The probability distribution function for a
    // Maxwellian is given as p(x) = 2/(T*sqrt(pi))*sqrt(x/T)*exp(-x/T). This PDF can
    // be sampled using rule C64 in the Monte Carlo Sampler LA-9721-MS.
    //===============================================================================

    // T: tabulated function of incoming E
    double ksi1, ksi2, ksi3;
    double c;       // cosine of pi/2*r3

    ksi1 = get_rand_host(RNG);
    ksi2 = get_rand_host(RNG);
    ksi3 = get_rand_host(RNG);
    c = cos(PI / TWO * ksi3);

    return -T * (log(ksi1) + log(ksi2) * SQUARE(c));
}