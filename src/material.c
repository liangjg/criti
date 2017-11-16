//
// Created by xaq on 10/26/17.
//

#include "material.h"

mat_t *mat_init(){
    mat_t *_new_mat = (mat_t *)malloc(sizeof(mat_t));
    memset(_new_mat, 0x0, sizeof(mat_t));
    return _new_mat;
}