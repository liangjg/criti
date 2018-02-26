//
// Created by xaq on 11/15/17.
//

#include "criticality.h"
#include "IO_releated.h"
#include "RNG.h"


extern criti_t base_criti;
extern double base_start_wgt;
extern IOfp_t base_IOfp;
extern RNG_t RNGs[64];

void _combine_keff();

void _output_keff();

#define SWAP(_a, _b)    \
    do{    \
        void *_temp = (_a);    \
        (_a) = (_b);    \
        (_b) = _temp;    \
    } while(0)


void process_cycle_end(){
    int remainder1, remainder2;
    int quotient;

    /* process eigenvalue */
    if(base_criti.tot_fission_bank_cnt < 5){
        puts("Insufficient fission source to be sampled.");
        release_resource();
        exit(0);
    }

    for(int i = 0; i < 3; i++)
        base_criti.keff_cycle[i] = base_criti.keff_wgt_sum[i] / base_criti.tot_start_wgt;
    base_criti.keff_final = base_criti.keff_cycle[0];

    _combine_keff();
    _output_keff();

    /* process fission source and fission bank */
    /* 这里采用了一个小技巧，正常来说是应该将fission_bank 里面的值一一复制到fission_src里面，
     * 但是这样逐个复制的开销较大。因此这里直接交换两个数组的指针，同时交换bank_sz和src_sz，
     * 用O(1)时间完成交换
     */
    for(int i = 0; i < NUMBERS_SLAVES; i++)
        SWAP(base_criti.fission_bank[i], base_criti.fission_src[i]);

    base_criti.cycle_neutron_num = base_criti.tot_fission_bank_cnt;
    base_start_wgt = ONE * base_criti.tot_start_wgt / base_criti.tot_fission_bank_cnt;

    for(int i = 0; i < 3; i++)
        base_criti.keff_wgt_sum[i] = ZERO;
    base_criti.tot_fission_bank_cnt = 0;

    /* 更新tot_transfer_num和fission_src_cnt */
    base_criti.tot_transfer_num = base_criti.cycle_neutron_num / NUMBERS_PER_TRANS;
    remainder1 = base_criti.cycle_neutron_num - NUMBERS_PER_TRANS * base_criti.tot_transfer_num;
    for(int i = 0; i < NUMBERS_SLAVES; i++)
        base_criti.fission_src_cnt[i] = base_criti.tot_transfer_num * 400;
    if(remainder1 > 0){
        base_criti.tot_transfer_num++;
        quotient = remainder1 / NUMBERS_SLAVES;
        remainder2 = remainder1 - quotient * NUMBERS_SLAVES;
        for(int i = 0; i < NUMBERS_SLAVES; i++)
            base_criti.fission_src_cnt[i] += quotient;
        if(remainder2 > 0)
            for(int i = 0; i < remainder2; i++)
                base_criti.fission_src_cnt[i] += 1;
    }

    /* 为每个从核准备随机数发生器 */
    memcpy(&RNGs[0], &RNGs[63], sizeof(RNG_t));

    for(int i = 1; i < NUMBERS_SLAVES; i++){
        memcpy(&RNGs[i], &RNGs[i - 1], sizeof(RNG_t));

        int fis_src_cnt = base_criti.fission_src_cnt[i - 1];
        for(int j = 1; j < fis_src_cnt; j++)
            get_rand_seed_host(&RNGs[i]);
    }
}

void _combine_keff(){
    if(base_criti.current_cycle <= base_criti.inactive_cycle_num)
        return;

    double keff_corr[3];                          // Correlation
    int i, j, k, diffrence;
    double d0, d1, d2, temp;
    double keff_covw[3][3], keff_covw_sum;

    diffrence = base_criti.current_cycle - base_criti.inactive_cycle_num;
    for(i = 0; i < 3; ++i){
        base_criti.keff_sum[i] = base_criti.keff_sum[i] + base_criti.keff_cycle[i];
        for(j = 0; j < 3; ++j)
            base_criti.keff_cp_sum[i][j] =
                    base_criti.keff_cp_sum[i][j] + base_criti.keff_cycle[i] * base_criti.keff_cycle[j];
    }

    for(i = 0; i < 4; ++i){
        base_criti.keff_covw_ave[i] = ZERO;
        base_criti.keff_covw_std[i] = ZERO;
    }

    /// Calculate the individual average, std. dev., as well as the covariance ////.
    keff_covw_sum = ZERO;
    d0 = diffrence;
    d1 = sqrt(d0 / MAX(1, diffrence - 1));
    for(i = 0; i < 3; ++i){
        base_criti.keff_individual_ave[i] = base_criti.keff_sum[i] / diffrence;
        keff_corr[i] = ZERO;
        for(j = 0; j < 3; ++j){
            keff_covw[i][j] =
                    base_criti.keff_cp_sum[i][j] - base_criti.keff_sum[i] * base_criti.keff_sum[j] / diffrence;
            keff_covw_sum = keff_covw_sum + keff_covw[i][j];
        }
        base_criti.keff_individual_std[i] = d1 * sqrt(fabs(keff_covw[i][i])) / diffrence;
    }

    /// calculate simple/covariance-weighted combined averages and std. devs.  ///
    /// Combined sequence 0 - 3 : (col/abs), (abs,tl), (tl,col), (col/abs/tl)  ///
    if(diffrence <= 2)
        return;
    d2 = sqrt(d0 / (d0 - TWO));
    for(i = 0; i < 3; ++i){
        j = (i + 1) % 3;
        base_criti.keff_covw_ave[i] = base_criti.keff_individual_ave[i];
        base_criti.keff_covw_std[i] = base_criti.keff_individual_std[i];
        temp = keff_covw[i][i] + keff_covw[j][j] - TWO * keff_covw[i][j];
        if(temp > 1.e-10 * (fabs(keff_covw[i][i]) + fabs(keff_covw[i][j]) + fabs(keff_covw[j][j]))){
            base_criti.keff_covw_ave[i] = base_criti.keff_individual_ave[i] -
                                          (base_criti.keff_individual_ave[i] - base_criti.keff_individual_ave[j]) *
                                          (keff_covw[i][i] - keff_covw[j][j]) / temp;
            base_criti.keff_covw_std[i] =
                    d2 * sqrt(fabs((keff_covw[i][i] * keff_covw[j][j] - keff_covw[i][j] * keff_covw[i][j]) *
                                   (temp +
                                    d0 * (base_criti.keff_individual_ave[i] - base_criti.keff_individual_ave[j]) *
                                    (base_criti.keff_individual_ave[i] - base_criti.keff_individual_ave[j])) /
                                   (temp * temp))) / diffrence;
        }
        if(keff_covw[i][i] * keff_covw[j][j] > 0)
            keff_corr[i] = keff_covw[i][j] / sqrt(fabs(keff_covw[i][i] * keff_covw[j][j]));
    }

    /// calculate the combined average (col/abs/tl), zc(4). //
    if(diffrence == 3)
        return;
    double al;
    double a = ZERO;
    double s1 = ZERO;
    double s2 = ZERO;
    double s3 = ZERO;
    k = 2;
    if(fabs(keff_corr[0] - ONE) < 1.0E-5)
        goto go60;
    for(i = 0; i < 3; ++i){
        j = (i + 1) % 3;
        k = (j + 1) % 3;
        al = keff_covw[j][j] * keff_covw[k][k] - keff_covw[j][j] * keff_covw[i][k] - keff_covw[k][k] * keff_covw[i][j] +
             keff_covw[j][k] * (keff_covw[i][j] + keff_covw[i][k] - keff_covw[j][k]);
        a += al;
        base_criti.keff_covw_ave[3] = base_criti.keff_covw_ave[3] + al * base_criti.keff_individual_ave[i];
        s1 = s1 + al * keff_covw[i][0];
        s2 = s2 + (keff_covw[j][j] + keff_covw[k][k] - 2 * keff_covw[j][k]) *
                  (base_criti.keff_individual_ave[i] * base_criti.keff_individual_ave[i]);
        s3 = s3 + (keff_covw[k][k] + keff_covw[i][j] - keff_covw[j][k] - keff_covw[i][k]) *
                  base_criti.keff_individual_ave[i] * base_criti.keff_individual_ave[j];
    }
    if(a == ZERO || base_criti.keff_covw_ave[3] == ZERO)
        goto go60;
    base_criti.keff_covw_ave[3] = base_criti.keff_covw_ave[3] / a;
    base_criti.keff_covw_std[3] = sqrt(fabs(s1 * (ONE + d0 * (s2 - s3 - s3) / a) / (a * d0 * (d0 - 3.0))));
    base_criti.keff_final = base_criti.keff_covw_ave[3];
    return;

go60:
    base_criti.keff_covw_ave[3] = base_criti.keff_covw_ave[k];
    base_criti.keff_covw_std[3] = base_criti.keff_covw_std[k];
    base_criti.keff_final = base_criti.keff_covw_ave[3];
}


void _output_keff(){
    /* cycle finish time */
    finish_time = clock();
    double compute_time_min = (double) (finish_time - start_time) / CLOCKS_PER_SEC / 60.0;

    if(base_criti.current_cycle == base_criti.inactive_cycle_num + 1){
        puts("************* Start Active Cycle *************");
        puts("Cycle      Active       Keff        Average        Std      Time(min)");
    } else if(base_criti.current_cycle == 1){
        puts("\n************ Start Inactive Cycle ************");
        puts("Cycle        Keff      Time(min)");
    }

    if(base_criti.current_cycle <= base_criti.inactive_cycle_num)
        printf("%-6d     %-f     %-.4f\n", base_criti.current_cycle, base_criti.keff_cycle[0], compute_time_min);
    if(base_criti.current_cycle > base_criti.inactive_cycle_num){
        printf("%-6d     %-6d     %-f     %-f     %-f     %-.4f\n",
               base_criti.current_cycle, base_criti.current_cycle - base_criti.inactive_cycle_num,
               base_criti.keff_cycle[0], base_criti.keff_individual_ave[0], base_criti.keff_individual_std[0],
               compute_time_min);
    }

    /* output to file */
    if(base_criti.current_cycle == base_criti.inactive_cycle_num + 1)
        fputs("================== start active cycles ==================================================================================================\n",
              base_IOfp.opt_fp);
    else if(base_criti.current_cycle == 1){
        fputs("\n=========================================================================================================================================\n",
              base_IOfp.opt_fp);
        fputs("                      keff estimators by cycle             individual average keff and deviation           combined average keff\n",
              base_IOfp.opt_fp);
        fputs("cycle   history      k(col)    k(abs)     k(tl)     k(col)   st dev     k(abs)  st dev      k(tl) st dev     k(c/a/t)  st dev    Time(min)\n",
              base_IOfp.opt_fp);
    } else if(base_criti.current_cycle % 10 == 1)
        fputs("=========================================================================================================================================\n",
              base_IOfp.opt_fp);


    if(base_criti.current_cycle <= base_criti.inactive_cycle_num){
        fprintf(base_IOfp.opt_fp,
                "%-6d  %-9d | %-f  %-f  %-f |                                                                                %-.4f\n",
                base_criti.current_cycle, base_criti.tot_fission_bank_cnt, base_criti.keff_cycle[0],
                base_criti.keff_cycle[1], base_criti.keff_cycle[2], compute_time_min);
    }
    if(base_criti.current_cycle > base_criti.inactive_cycle_num){
        fprintf(base_IOfp.opt_fp, "%-6d  %-9d | %-f  %-f  %-f | %-f %-f  %-f %-f  %-f %-f | %-f %-f |  %-.4f\n",
                base_criti.current_cycle, base_criti.tot_fission_bank_cnt, base_criti.keff_cycle[0],
                base_criti.keff_cycle[1], base_criti.keff_cycle[2],
                base_criti.keff_individual_ave[0], base_criti.keff_individual_std[0], base_criti.keff_individual_ave[1],
                base_criti.keff_individual_std[1], base_criti.keff_individual_ave[2], base_criti.keff_individual_std[2],
                base_criti.keff_covw_ave[3], base_criti.keff_covw_std[3], compute_time_min);
    }
}