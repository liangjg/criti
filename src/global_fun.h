//
// Created by xaq on 9/2/17.
//

#ifndef TTYW_GLOBAL_FUN_H
#define TTYW_GLOBAL_FUN_H

#include "common.h"

#define INTPLT_BY_POS_FR(addr, pos, fr)  \
    (*((addr) + (pos)) + (*((addr) + ((pos) + 1)) - *((addr) + (pos))) * (fr))

#ifdef __cplusplus
template <typename T> void GetIntpltPos(const T *vec, T val, int min, int max, int *pos)
{
#define POS (*(pos))
    if(val <= *(vec + min)) {
        POS = min;
        return;
    }
    if(val >= *(vec + max)) {
        POS = max ;   //// note : this place is different with GetIntpltPosFr
        return;
    }

    while(max - min > 1) {
        int mid = (min + max) / 2;
        if(val >= *(vec + mid))
            min = mid;
        else
            max = mid;
    }
    POS = min;
}

template <typename T> void GetIntpltPosFr(const T *vec, T val, int min, int max, int *pos, double *fr)
{
    if(val <= *(vec + min)) {
        POS = min;
        *fr = 0;
        return;
    }
    if(val >= *(vec + max)) {
        POS = max - 1;
        *fr = 1;
        return;
    }

    while(max - min > 1) {
        int mid = (min + max) / 2;
        if(val >= *(vec + mid))
            min = mid;
        else
            max = mid;
    }

    POS = min;
    if(*(vec + min + 1) ==  *(vec + min))
        *fr = 0 ;
    else
        *fr = (val - * (vec + min)) / (*(vec + min + 1) -  * (vec + min));
}
#endif

#ifdef __cplusplus
extern "C" {
#endif

void get_intplt_pos_double(double *vec, double val, int min, int max, int *pos);
void get_intplt_pos_int(int *vec, int val, int min, int max, int *pos);
void get_intplt_pos_fr_double(double *vec, double val, int min, int max, int *pos, double *fr);
void get_intplt_pos_fr_int(int *vec, int val, int min, int max, int *pos, double *fr);

#ifdef __cplusplus
}
#endif

#endif //TTYW_GLOBAL_FUN_H
