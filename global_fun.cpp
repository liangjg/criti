//
// Created by xaq on 9/2/17.
//

#include "global_fun.h"

void get_intplt_pos_double(double *vec, double val, int min, int max, int *pos){
    GetIntpltPos(vec, val, min, max, pos);
}

void get_intplt_pos_int(int *vec, int val, int min, int max, int *pos){
    GetIntpltPos(vec, val, min, max, pos);
}

void get_intplt_pos_fr_double(double *vec, double val, int min, int max, int *pos, double *fr){
    GetIntpltPosFr(vec, val, min, max, pos, fr);
}

void get_intplt_pos_fr_int(int *vec, int val, int min, int max, int *pos, double *fr){
    GetIntpltPosFr(vec, val, min, max, pos, fr);
}