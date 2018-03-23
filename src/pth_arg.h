//
// Created by x1314aq on 18-3-10.
//

#ifndef CRITI_PTH_ARG_H
#define CRITI_PTH_ARG_H

#include "RNG.h"
#include "nuclide.h"


typedef struct pth_arg_t {
    bank_t *src;
    bank_t *bank;
    nuc_xs_t *nuc_xs;
    RNG_t RNG;
    int src_cnt;
    int bank_cnt;
    int col_cnt;
    int id;
    double keff_wgt_sum[3];
    double keff_final;
} pth_arg_t;

#endif

