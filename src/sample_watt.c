//
// Created by xaq on 2018/1/29.
//

#include "sample_method.h"
#include "common.h"


double sample_watt(RNG_t *RNG, double a, double b){
    //===============================================================================
    // WATT_SPECTRUM samples the outgoing energy from a Watt energy-dependent fission
    // spectrum. Although fitted parameters exist for many nuclides, generally the
    // continuous tabular distributions (LAW 4) should be used in lieu of the Watt
    // spectrum. This direct sampling scheme is an unpublished scheme based on the
    // original Watt spectrum derivation (See F. Brown's MC lectures).
    //===============================================================================

    double w = sample_maxwell(RNG, a);
    double ksi = get_rand_slave(RNG);
    return w + a * a * b / 4. + (TWO * ksi - ONE) * sqrt(a * a * b * w);
}