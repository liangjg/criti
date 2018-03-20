//
// Created by x1314aq on 18-1-16.
//

#ifndef CRITI_FIXED_SOURCE_H
#define CRITI_FIXED_SOURCE_H

#include "common.h"
#include "particle_status.h"
#include "pth_arg.h"


typedef struct {
    /* 读取输入文件得到的参数 */
    int tot_neu_num;                       /* 总共要模拟的粒子数 */
    SRC_TYPE_T fsrc_type;                  /* 初始源的类型 */
    double fsrc_paras[6];                  /* 初始源的参数 */
    double fsrc_erg;                       /* 粒子初始能量 */
    double tot_start_wgt;

    int tot_bank_cnt;

    /* 总碰撞次数 */
    int tot_col_cnt;
} fixed_src_t;

BEGIN_DECL
void
init_external_src(pth_arg_t *pth_args);

void
geometry_tracking_fixed(particle_status_t *par_status,
                        nuc_xs_t *nuc_xs,
                        RNG_t *RNG);

int
get_fis_neu_state_fixed(particle_status_t *par_status,
                        bank_t *cur_fixed_bank,
                        RNG_t *RNG,
                        double nu);

END_DECL

#endif //CRITI_FIXED_SOURCE_H
