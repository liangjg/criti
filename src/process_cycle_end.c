//
// Created by xaq on 11/15/17.
//

#include "criticality.h"
#include "IO_releated.h"


extern criti_t base_criti;
extern double base_start_wgt;
extern IOfp_t base_IOfp;
extern RNG_t base_RNG;
extern int base_num_threads;

void
_combine_keff(int current_cycle);

void
_output_keff(int current_cycle);

#define SWAP(_a, _b)    \
    do{    \
        void *_temp = (_a);    \
        (_a) = (_b);    \
        (_b) = _temp;    \
    } while(0)

void
process_cycle_end(int currenr_cycle,
                  pth_arg_t *pth_args)
{
    int i, j;
    int quotient, remainder;

    for(i = 0; i < base_num_threads; i++) {
        base_criti.tot_col_cnt += pth_args[i].col_cnt;
        base_criti.tot_fis_bank_cnt += pth_args[i].fis_bank_cnt;
        for(j = 0; j < 3; j++)
            base_criti.keff_wgt_sum[j] += pth_args[i].keff_wgt_sum[j];
    }

    if(base_criti.tot_fis_bank_cnt < 5) {
        puts("Insufficient fission source to be sampled.");
        release_resource();
        exit(0);
    }

    for(i = 0; i < 3; i++)
        base_criti.keff_cycle[i] = base_criti.keff_wgt_sum[i] / base_criti.tot_start_wgt;
    base_criti.keff_final = base_criti.keff_cycle[0];

    _combine_keff(currenr_cycle);
    _output_keff(currenr_cycle);

    base_criti.cycle_neu_num = base_criti.tot_fis_bank_cnt;
    base_start_wgt = ONE * base_criti.tot_start_wgt / base_criti.tot_fis_bank_cnt;

    quotient = base_criti.tot_fis_bank_cnt / base_num_threads;
    remainder = base_criti.tot_fis_bank_cnt - quotient * base_num_threads;

    for(i = 0; i < base_num_threads; i++) {
        SWAP(pth_args[i].fis_src, pth_args[i].fis_bank);
        pth_args[i].fis_src_cnt = quotient;
    }
    for(i = 0; i < remainder; i++)
        pth_args[i].fis_src_cnt++;

    for(i = 0; i < base_num_threads; i++) {
        /* 负载均衡策略 */
        fission_bank_t *dest, *src;
        int diff = pth_args[i].fis_bank_cnt - pth_args[i].fis_src_cnt;
        if(diff > 0) {
            dest = pth_args[i + 1].fis_src + diff;
            src = pth_args[i].fis_src + pth_args[i].fis_src_cnt;
            memmove(dest, pth_args[i + 1].fis_src, pth_args[i + 1].fis_bank_cnt * sizeof(fission_bank_t));
            memcpy(pth_args[i + 1].fis_src, src, diff * sizeof(fission_bank_t));
            pth_args[i + 1].fis_bank_cnt += diff;
        } else if(diff < 0) {
            diff = ~(diff - 1);    /* 求diff的绝对值 */
            dest = pth_args[i].fis_src + pth_args[i].fis_bank_cnt;
            src = pth_args[i + 1].fis_src + diff;
            memcpy(dest, pth_args[i + 1].fis_src, diff * sizeof(fission_bank_t));
            pth_args[i + 1].fis_bank_cnt -= diff;
            memcpy(pth_args[i + 1].fis_src, src, pth_args[i + 1].fis_bank_cnt * sizeof(fission_bank_t));
        }

        pth_args[i].fis_bank_cnt = 0;
        pth_args[i].col_cnt = 0;
        pth_args[i].keff_final = base_criti.keff_final;
        for(j = 0; j < 3; j++)
            pth_args[i].keff_wgt_sum[j] = ZERO;
    }

#ifdef USE_PTHREAD
    memcpy(&base_RNG, &pth_args[base_num_threads - 1].RNG, sizeof(RNG_t));
    memcpy(&pth_args[0].RNG, &base_RNG, sizeof(RNG_t));
    for(i = 1; i < base_num_threads; i++) {
        memcpy(&pth_args[i].RNG, &pth_args[i - 1].RNG, sizeof(RNG_t));

        int skip_src = pth_args[i - 1].fis_src_cnt;
        for(j = 0; j < skip_src; j++)
            get_rand_seed(&pth_args[i].RNG);
    }
#endif

    /* reset criticality */
    base_criti.tot_fis_bank_cnt = 0;
    for(i = 0; i < 3; i++)
        base_criti.keff_wgt_sum[i] = ZERO;
}

void
_combine_keff(int current_cycle)
{
    if(current_cycle <= base_criti.inactive_cycle_num)
        return;

    double keff_corr[3];                          // Correlation
    int i, j, k, diffrence;
    double d0, d1, d2, temp;
    double keff_covw[3][3], keff_covw_sum;

    diffrence = current_cycle - base_criti.inactive_cycle_num;
    for(i = 0; i < 3; ++i) {
        base_criti.keff_sum[i] = base_criti.keff_sum[i] + base_criti.keff_cycle[i];
        for(j = 0; j < 3; ++j)
            base_criti.keff_cp_sum[i][j] =
                base_criti.keff_cp_sum[i][j] + base_criti.keff_cycle[i] * base_criti.keff_cycle[j];
    }

    for(i = 0; i < 4; ++i) {
        base_criti.keff_covw_ave[i] = ZERO;
        base_criti.keff_covw_std[i] = ZERO;
    }

    /// Calculate the individual average, std. dev., as well as the covariance ////.
    keff_covw_sum = ZERO;
    d0 = diffrence;
    d1 = sqrt(d0 / MAX(1, diffrence - 1));
    for(i = 0; i < 3; ++i) {
        base_criti.keff_individual_ave[i] = base_criti.keff_sum[i] / diffrence;
        keff_corr[i] = ZERO;
        for(j = 0; j < 3; ++j) {
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
    for(i = 0; i < 3; ++i) {
        j = (i + 1) % 3;
        base_criti.keff_covw_ave[i] = base_criti.keff_individual_ave[i];
        base_criti.keff_covw_std[i] = base_criti.keff_individual_std[i];
        temp = keff_covw[i][i] + keff_covw[j][j] - TWO * keff_covw[i][j];
        if(temp > 1.e-10 * (fabs(keff_covw[i][i]) + fabs(keff_covw[i][j]) + fabs(keff_covw[j][j]))) {
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
    for(i = 0; i < 3; ++i) {
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

void
_output_keff(int current_cycle)
{
    /* cycle finish time */
    gettimeofday(&finish_time, NULL);
    double compute_time_min =
        (finish_time.tv_sec - start_time.tv_sec + (finish_time.tv_usec - start_time.tv_usec) / 1000000.0) / 60.0;

    if(current_cycle == base_criti.inactive_cycle_num + 1) {
        puts("************* Start Active Cycle *************");
        puts("Cycle      Active       Keff        Average        Std      Time(min)");
    } else if(current_cycle == 1) {
        puts("\n************ Start Inactive Cycle ************");
        puts("Cycle        Keff      Time(min)");
    }

    if(current_cycle <= base_criti.inactive_cycle_num)
        printf("%-6d     %-f     %-.4f\n", current_cycle, base_criti.keff_cycle[0], compute_time_min);
    else
        printf("%-6d     %-6d     %-f     %-f     %-f     %-.4f\n",
               current_cycle, current_cycle - base_criti.inactive_cycle_num,
               base_criti.keff_cycle[0], base_criti.keff_individual_ave[0], base_criti.keff_individual_std[0],
               compute_time_min);


    /* output to file */
    if(current_cycle == base_criti.inactive_cycle_num + 1)
        fputs(
            "================== start active cycles ==================================================================================================\n",
            base_IOfp.opt_fp);
    else if(current_cycle == 1) {
        fputs(
            "\n=========================================================================================================================================\n",
            base_IOfp.opt_fp);
        fputs(
            "                      keff estimators by cycle             individual average keff and deviation           combined average keff\n",
            base_IOfp.opt_fp);
        fputs(
            "cycle   history      k(col)    k(abs)     k(tl)     k(col)   st dev     k(abs)  st dev      k(tl) st dev     k(c/a/t)  st dev    Time(min)\n",
            base_IOfp.opt_fp);
    } else if(current_cycle % 10 == 1)
        fputs(
            "=========================================================================================================================================\n",
            base_IOfp.opt_fp);

    if(current_cycle <= base_criti.inactive_cycle_num) {
        fprintf(base_IOfp.opt_fp,
                "%-6d  %-9d | %-f  %-f  %-f |                                                                                %-.4f\n",
                current_cycle,
                base_criti.tot_fis_bank_cnt,
                base_criti.keff_cycle[0],
                base_criti.keff_cycle[1],
                base_criti.keff_cycle[2],
                compute_time_min);
    }
    if(current_cycle > base_criti.inactive_cycle_num) {
        fprintf(base_IOfp.opt_fp, "%-6d  %-9d | %-f  %-f  %-f | %-f %-f  %-f %-f  %-f %-f | %-f %-f |  %-.4f\n",
                current_cycle, base_criti.tot_fis_bank_cnt, base_criti.keff_cycle[0],
                base_criti.keff_cycle[1], base_criti.keff_cycle[2],
                base_criti.keff_individual_ave[0], base_criti.keff_individual_std[0], base_criti.keff_individual_ave[1],
                base_criti.keff_individual_std[1], base_criti.keff_individual_ave[2], base_criti.keff_individual_std[2],
                base_criti.keff_covw_ave[3], base_criti.keff_covw_std[3], compute_time_min);
    }
}