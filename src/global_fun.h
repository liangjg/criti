//
// Created by xaq on 9/2/17.
//

#ifndef CRITI_GLOBAL_FUN_H
#define CRITI_GLOBAL_FUN_H

#include "common.h"


#ifdef __cplusplus
extern "C" {
#endif

double intplt_by_pos_fr(const double *vec, int pos, double frac);

int get_intplt_pos(const double *vec, double key, int low, int high);

int get_intplt_pos_fr(const double *vec, double key, int low, int high, double *frac);

#ifdef __cplusplus
}
#endif

#endif //CRITI_GLOBAL_FUN_H
