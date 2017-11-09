//
// Created by xaq on 10/27/17.
//

#include "sample_method.h"
#include "RNG.h"
#include "common.h"

double sample_maxwell(double T){
    //===============================================================================
    // MAXWELL_SPECTRUM samples an energy from the Maxwell fission distribution based
    // on a direct sampling scheme. The probability distribution function for a
    // Maxwellian is given as p(x) = 2/(T*sqrt(pi))*sqrt(x/T)*exp(-x/T). This PDF can
    // be sampled using rule C64 in the Monte Carlo Sampler LA-9721-MS.
    //===============================================================================

    // T: tabulated function of incoming E
    double ksi1, ksi2, ksi3;
    double c;       // cosine of pi/2*r3

    ksi1 = get_rand();
    ksi2 = get_rand();
    ksi3 = get_rand();
    c = cos(PI / TWO * ksi3);

    return -T * (log(ksi1) + log(ksi2) * SQUARE(c));
}

double sample_watt(double a, double b){
    //===============================================================================
    // WATT_SPECTRUM samples the outgoing energy from a Watt energy-dependent fission
    // spectrum. Although fitted parameters exist for many nuclides, generally the
    // continuous tabular distributions (LAW 4) should be used in lieu of the Watt
    // spectrum. This direct sampling scheme is an unpublished scheme based on the
    // original Watt spectrum derivation (See F. Brown's MC lectures).
    //===============================================================================

    double w = sample_maxwell(a);
    double ksi = get_rand();
    return w + a * a * b / 4. + (TWO * ksi - ONE) * sqrt(a * a * b * w);
}