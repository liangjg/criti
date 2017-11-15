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
    int neu_num_per_cycle;
    int tot_cycle_num;
    int inactive_cycle_num;
    int active_cycle_num;

    KSRC_T ksrc_type;
    double ksrc_para[6];
    int ksrc_para_sz;

    int cycle_neutron_num;    /* 当前代要模拟的中子数目 */
    int current_cycle;
    //    int current_particle;

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

    unsigned long long tot_col_cnt;
    unsigned long col_cnt;

    int fission_bank_cnt;
    int tot_fission_bank_cnt;
    int fission_src_cnt;

    fission_bank_t *fission_src;      /* 当前代要模拟的中子源，每个中子都从其中抽样产生 */
    int fission_src_sz;               /* 一般来说，src_sz = bank_sz */
    fission_bank_t *fission_bank;     /* 存储每一代裂变产生的中子，供下一代模拟用 */
    int fission_bank_sz;
} criti_t;


#ifdef __cplusplus
extern "C"{
#endif

void init_fission_source();

void sample_fission_source();

void track_history();

void get_fis_neu_state(int fis_MT, double fis_wgt);

void process_cycle_end();

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

#ifdef __cplusplus
}
#endif

#endif //TTYW_CRITICALITY_H
