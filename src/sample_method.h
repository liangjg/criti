//
// Created by xaq on 10/27/17.
//

#ifndef CRITI_SAMPLE_METHOD_H
#define CRITI_SAMPLE_METHOD_H

#include "RNG.h"
#include "common.h"


BEGIN_DECL
double
sample_maxwell(RNG_t *RNG,
                    double T);

double
sample_watt(RNG_t *RNG,
            double a,
            double b);

END_DECL

#endif //CRITI_SAMPLE_METHOD_H
