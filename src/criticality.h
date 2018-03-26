//
// Created by xaq on 10/26/17.
//

#ifndef CRITI_CRITICALITY_H
#define CRITI_CRITICALITY_H

#include "common.h"
#include "particle_status.h"
#include "RNG.h"
#include "pth_arg.h"


typedef struct criti_t {
    /* 读取输入文件时的参数 */
    int tot_cycle_num;
    int inactive_cycle_num;
    int cycle_neu_num;                  /* 当前代要模拟的中子数目 */

    /* 初始源参数 */
    SRC_TYPE_T ksrc_type;
    double ksrc_para[6];

    double keff_final;                  /* final keff of each cycle */
    double keff_wgt_sum[3];             /* 0: Collision estimator,  1: Absorption estimator,  2:Track Length estimator */
    double keff_cycle[3];               /* 0: Collision estimator,  1: Absorption estimator,  2:Track Length estimator */
    double keff_sum[3];                 /* keff_sum: cumulative sums */
    double keff_cp_sum[3][3];           /* keff_cp_sum: keff cross product sums */
    double keff_individual_ave[3];      /* average  deviation of individual estimators */
    double keff_individual_std[3];      /* standard deviation of individual estimators */
    double keff_covw_ave[4];            /* covariance-weighted combined averages deviations */
    double keff_covw_std[4];            /* covariance-weighted combined standard deviations */
    double tot_start_wgt;

    int tot_col_cnt;                    /* 总碰撞次数 */
    int tot_bank_cnt;
    int tot_transfer_num;               /* 主核总共要向从核传输多少次数据 */
} criti_t;

BEGIN_DECL
void
init_fission_source(pth_arg_t *pth_args);

int
get_fis_neu_state(particle_status_t *par_status,
                  bank_t *cur_fis_bank,
                  RNG_t *RNG,
                  int fis_MT,
                  double fis_wgt,
                  double nu);

void
process_cycle_end(int current_cycle,
                  pth_arg_t *pth_args);
END_DECL

#endif //CRITI_CRITICALITY_H
