//
// Created by xaq on 10/26/17.
//

#ifndef CRITI_CRITICALITY_H
#define CRITI_CRITICALITY_H

#include "common.h"
#include "particle_status.h"
#include "RNG.h"
#include "pth_arg.h"


typedef struct {
    /* 读取输入文件时的参数 */
    int tot_cycle_num;
    int inactive_cycle_num;
    int cycle_neu_num;                /* 当前代要模拟的中子数目 */

    /* 初始源参数 */
    SRC_TYPE_T ksrc_type;
    double ksrc_para[6];


    double keff_final;                /* final keff of each cycle */
    double keff_wgt_sum[3];           /* 0: Collision estimator,  1: Absorption estimator,  2:Track Length estimator */
    double keff_cycle[3];             /* 0: Collision estimator,  1: Absorption estimator,  2:Track Length estimator */
    double keff_sum[3];               /* keff_sum: cumulative sums */
    double keff_cp_sum[3][3];         /* keff_cp_sum: keff cross product sums */
    double keff_individual_ave[3];    /* average  deviation of individual estimators */
    double keff_individual_std[3];    /* standard deviation of individual estimators */
    double keff_covw_ave[4];          /* covariance-weighted combined averages deviations */
    double keff_covw_std[4];          /* covariance-weighted combined standard deviations */
    double tot_start_wgt;

    int tot_col_cnt;                  /* 所有粒子发生的碰撞次数之和 */
    int tot_bank_cnt;                 /* 用来统计所有线程产生的粒子之和 */
} criti_t;

BEGIN_DECL
void
init_fission_src(pth_arg_t *pth_args);

int
get_fis_neu_state(particle_status_t *par_status,
                  bank_t *cur_fis_bank,
                  RNG_t *RNG,
                  int fis_MT,
                  double fis_wgt,
                  double nu);

void
process_cycle_end(int currenr_cycle,
                  pth_arg_t *pth_args);

#define Estimate_keff_col(wgt, macro_mu_fis_xs, macro_tot_xs)  \
    do{  \
        base_criti.keff_wgt_sum[0] += (wgt) * (macro_mu_fis_xs) / (macro_tot_xs);  \
    } while(0)

#define Estimate_keff_abs(wgt, nu, micro_fis_xs, micro_tot_xs)  \
    do{  \
        base_criti.keff_wgt_sum[1] += (wgt) * (nu) * (micro_fis_xs) / (micro_tot_xs);  \
    } while(0)

#define Estimate_keff_tl(wgt, macro_mu_fis_xs, tl)  \
    do{  \
        base_criti.keff_wgt_sum[2] += (wgt) * (tl) * (macro_mu_fis_xs);  \
    } while(0)
END_DECL

#endif //CRITI_CRITICALITY_H
