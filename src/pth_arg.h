//
// Created by x1314aq on 18-3-10.
//

#ifndef CRITI_PTH_ARG_H
#define CRITI_PTH_ARG_H

#include "RNG.h"
#include "nuclide.h"


typedef struct pth_arg_t {
    bank_t *src;                   /* 要模拟的粒子源 */
    bank_t *bank;                  /* 产生的裂变源 */
    nuc_xs_t *nuc_xs;              /* 隶属于该线程的核素截面数组的起始地址 */
    RNG_t RNG;                     /* 该线程的RNG */
    int src_cnt;                   /* fis_src的长度 */
    int bank_cnt;                  /* 总共产生的裂变源数目 */
    int col_cnt;                   /* 总共碰撞次数 */
    int id;
    double keff_wgt_sum[3];
    double keff_final;
} pth_arg_t;

#endif //CRITI_PTH_ARG_H
