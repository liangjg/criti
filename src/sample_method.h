//
// Created by xaq on 10/27/17.
//

#ifndef CRITI_SAMPLE_METHOD_H
#define CRITI_SAMPLE_METHOD_H

#include "common.h"
#include "RNG.h"


BEGIN_DECL
double
sample_maxwell(double T,
               RNG_t *RNG);

double
sample_watt(double a,
            double b,
            RNG_t *RNG);
END_DECL

#endif //CRITI_SAMPLE_METHOD_H
