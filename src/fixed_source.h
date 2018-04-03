// 
// Created by x1314aq on 18-1-16.
//

#ifndef CRITI_FIXED_SOURCE_H
#define CRITI_FIXED_SOURCE_H

#include "common.h"
#include "particle_status.h"
#include "pth_arg.h"


typedef struct fixed_src_t {
    int tot_neu_num;
    SRC_TYPE_T fsrc_type;
    double fsrc_paras[6];
    double fsrc_erg;
    double tot_start_wgt;

    /* 总碰撞次数 */
    int tot_col_cnt;
} fixed_src_t;

BEGIN_DECL
void
init_external_src(pth_arg_t *pth_args);

int
get_fis_neu_state_fixed(particle_status_t *par_status,
                        bank_t *cur_fixed_bank,
                        RNG_t *RNG,
                        double nu);

END_DECL

#endif
