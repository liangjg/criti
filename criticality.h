//
// Created by xaq on 10/26/17.
//

#include "common.h"

#ifndef TTYW_CRITICALITY_H
#define TTYW_CRITICALITY_H

typedef enum{
    POINT,
    SLAB,
    SPHERE
} KSRC_T;

typedef struct{
    double pos[3];
    double dir[3];
    double erg;
} fission_bank_t;

typedef struct{
    int neutron_num_per_cycle;
    int tot_cycle_num;
    int inactive_cycle_num;
    int active_cycle_num;

    KSRC_T ksrc_type;
    double ksrc_para[6];
    int ksrc_para_sz;

    int current_cycle_neu;    /* 当前代要模拟的中子数目 */
    int current_active_cycle;
    int current_cycle;
    int current_particle;

    double keff_final;
    double keff_wgt_sum[3];
    double keff_cycle[3];    /* 0: Collision estimator,  1: Absorption estimator,  2:Track Length estimator */
    double keff_sum[3];      /* keff_sum: cumulative sums */
    double keff_cp_sum[3][3];/* keff_cp_sum: keff cross product sums */
    double keff_individual_ave[3];    /* average and standard deviation of individual estimators */
    double keff_individual_std[3];
    double keff_covw_ave[4];    /* covariance-weighted combined averages and standard deviations */
    double keff_covw_std[4];
    double tot_start_wgt;

    unsigned long long tot_col_cnt;
    unsigned long col_cnt;

    int fission_bank_cnt;
    int tot_fission_bank_cnt;
    int fission_src_cnt;

    fission_bank_t *fission_src;
    int fission_src_sz;
    fission_bank_t *fission_bank;
    int fission_bank_sz;
} criti_t;
#endif //TTYW_CRITICALITY_H
