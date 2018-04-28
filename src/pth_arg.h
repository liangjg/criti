//
// Created by x1314aq on 18-3-10.
//

#ifndef CRITI_PTH_ARG_H
#define CRITI_PTH_ARG_H

#include "RNG.h"
#include "nuclide.h"


typedef struct pth_arg_t {
    RNG_t RNG;
    bank_t *src;
    bank_t *bank;
    int src_cnt;
    int bank_cnt;
    int col_cnt;
    double keff_wgt_sum[3];
    double keff_final;
} pth_arg_t;

#endif

