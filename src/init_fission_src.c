//
// Created by xaq on 10/26/17.
//

#include "criticality.h"
#include "RNG.h"
#include "sample_method.h"
#include "pth_arg.h"


extern criti_t base_criti;
extern RNG_t base_RNG;
extern double base_start_wgt;
extern nuc_xs_t **base_nuc_xs;
extern int base_num_threads;

void
init_fission_src(void *args)
{
    int quotient, remainder;
    int i, j;

    printf("Initiating fission source...");
    pth_arg_t *pth_args = (pth_arg_t *) (args);

    base_criti.tot_start_wgt = ONE * base_criti.cycle_neu_num;

    quotient = base_criti.cycle_neu_num / base_num_threads;
    remainder = base_criti.cycle_neu_num - quotient * base_num_threads;
    for(i = 0; i < base_num_threads; i++){
        pth_args[i].fis_src_cnt = quotient;
        pth_args[i].fis_bank_cnt = 0;
        pth_args[i].nuc_xs = base_nuc_xs[i];
        pth_args[i].col_cnt = 0;
        pth_args[i].keff_final = base_criti.keff_final;
        for(j = 0; j < 3; j++)
            pth_args[i].keff_wgt_sum[j] = ZERO;
    }

    if(remainder){
        for(i = 0; i < remainder; i++)
            pth_args[i].fis_src_cnt++;
    }

    for(i = 0; i < base_num_threads; i++){
        int bank_sz = 5 * pth_args[i].fis_src_cnt;
        if(bank_sz < 100) bank_sz = 100;    /* 如果要模拟的粒子数目太少了的话会导致fission_bank空间过小，有越界的可能 */
        pth_args[i].fis_src = malloc(bank_sz * sizeof(fission_bank_t));
        pth_args[i].fis_bank = malloc(bank_sz * sizeof(fission_bank_t));
    }

    double ksi1, ksi2, ksi3;
    fission_bank_t *fis_src;
    int fis_src_cnt = 0;
    switch(base_criti.ksrc_type) {
    case POINT: {
        for(i = 0; i < base_num_threads; i++) {
            fis_src_cnt = pth_args[i].fis_src_cnt;
            for(j = 0; j < fis_src_cnt; j++){
                get_rand_seed(&base_RNG);
                fis_src = &pth_args[i].fis_src[j];
                fis_src->pos[0] = base_criti.ksrc_para[0];
                fis_src->pos[1] = base_criti.ksrc_para[1];
                fis_src->pos[2] = base_criti.ksrc_para[2];
            }
        }
        break;
    }
    case SLAB: {
        double len_x = base_criti.ksrc_para[1] - base_criti.ksrc_para[0];
        double len_y = base_criti.ksrc_para[3] - base_criti.ksrc_para[2];
        double len_z = base_criti.ksrc_para[5] - base_criti.ksrc_para[4];

        for(i = 0; i < base_num_threads; i++){
            fis_src_cnt = pth_args[i].fis_src_cnt;
            for(j = 0; j < fis_src_cnt; j++){
                get_rand_seed(&base_RNG);
                fis_src = &pth_args[i].fis_src[j];
                fis_src->pos[0] = base_criti.ksrc_para[0] + get_rand(&base_RNG) * len_x;
                fis_src->pos[1] = base_criti.ksrc_para[1] + get_rand(&base_RNG) * len_y;
                fis_src->pos[2] = base_criti.ksrc_para[2] + get_rand(&base_RNG) * len_z;
            }
        }
        break;
    }
    case SPHERE: {
        for(i = 0; i < base_num_threads; i++){
            fis_src_cnt = pth_args[i].fis_src_cnt;
            for(j = 0; j < fis_src_cnt; j++){
                get_rand_seed(&base_RNG);
                fis_src = &pth_args[i].fis_src[j];
                do {
                    ksi1 = TWO * get_rand(&base_RNG) - ONE;
                    ksi2 = TWO * get_rand(&base_RNG) - ONE;
                    ksi3 = TWO * get_rand(&base_RNG) - ONE;
                } while(SQUARE(ksi1) + SQUARE(ksi2) + SQUARE(ksi3) > 1);
                fis_src->pos[0] = base_criti.ksrc_para[0] + base_criti.ksrc_para[3] * ksi1;
                fis_src->pos[1] = base_criti.ksrc_para[1] + base_criti.ksrc_para[3] * ksi2;
                fis_src->pos[2] = base_criti.ksrc_para[2] + base_criti.ksrc_para[3] * ksi3;
            }
        }
        break;
    }
    default:puts("unknown fission source type.");
        break;
    }

    base_RNG.position_pre = -1000;
    base_RNG.position = 0;

    for(i = 0; i < base_num_threads; i++){
        fis_src_cnt = pth_args[i].fis_src_cnt;
        for(j = 0; j < fis_src_cnt; j++){
            get_rand_seed(&base_RNG);
            ksi1 = get_rand(&base_RNG);
            ksi2 = get_rand(&base_RNG);
            fis_src = &pth_args[i].fis_src[j];
            fis_src->dir[0] = TWO * ksi2 - ONE;
            fis_src->dir[1] = sqrt(ONE - SQUARE(fis_src->dir[0])) * cos(TWO * PI * ksi1);
            fis_src->dir[2] = sqrt(ONE - SQUARE(fis_src->dir[0])) * sin(TWO * PI * ksi1);

            double T = 4.0 / 3.0;
            fis_src->erg = sample_maxwell(T, &base_RNG);
        }
    }

    base_start_wgt = ONE;

    base_RNG.position_pre = -1000;
    base_RNG.position = 0;

    memcpy(&pth_args[0].RNG, &base_RNG, sizeof(RNG_t));
    for(i = 1; i < base_num_threads; i++){
        memcpy(&pth_args[i].RNG, &pth_args[i - 1].RNG, sizeof(RNG_t));

        int skip_src = pth_args[i - 1].fis_src_cnt;
        for(j = 0; j < skip_src; j++)
            get_rand_seed(&pth_args[i].RNG);
    }

    puts("Finished.");
}