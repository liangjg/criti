//
// Created by xaq on 10/26/17.
//

#include "criticality.h"
#include "sample_method.h"


#ifdef USE_MPI
#include "parallel.h"


extern parallel_t base_parallel;
#endif

extern criti_t base_criti;
extern RNG_t base_RNG;
extern double base_start_wgt;
extern nuc_xs_t **base_nuc_xs;
extern int base_num_threads;

void
init_fission_source(pth_arg_t *pth_args)
{
    int i, j, id;
    int quotient, remainder;

#ifdef USE_MPI
    if(IS_MASTER)
#endif
        printf("Initiating fission source...");

    base_criti.tot_start_wgt = ONE * base_criti.cycle_neu_num;

#ifdef USE_MPI
    int prev_neu_num = base_criti.cycle_neu_num;
    quotient = prev_neu_num / base_parallel.tot_procs;
    remainder = prev_neu_num - quotient * base_parallel.tot_procs;

    for(id = 0; id < base_parallel.tot_procs; id++)
        base_parallel.src_load[id] = quotient;
    for(id = 0; id < remainder; id++)
        base_parallel.src_load[id]++;
    base_criti.cycle_neu_num = base_parallel.src_load[base_parallel.id];

    base_parallel.rand_num_pos[0] = base_parallel.rand_num_sum;
    for(id = 1; id < base_parallel.tot_procs; id++)
        base_parallel.rand_num_pos[id] = base_parallel.rand_num_pos[id - 1] + base_parallel.src_load[id - 1];
    base_RNG.position = base_parallel.rand_num_pos[base_parallel.id];
#endif

    quotient = base_criti.cycle_neu_num / base_num_threads;
    remainder = base_criti.cycle_neu_num - quotient * base_num_threads;
    for(i = 0; i < base_num_threads; i++) {
        pth_args[i].src_cnt = quotient;
        pth_args[i].nuc_xs = base_nuc_xs[i];
        pth_args[i].keff_final = base_criti.keff_final;
    }
    if(remainder)
        for(i = 0; i < remainder; i++)
            pth_args[i].src_cnt++;

    /* TODO:可以考虑让主核来模拟多余的部分
     * pth_args[base_num_threads].src_cnt = remainder;
     * pth_args[base_num_threads].nuc_xs = base_nuc_xs[base_num_threads];
     * pth_args[base_num_threads].keff_final = base_criti.keff_final;
    */

    for(i = 0; i < base_num_threads; i++) {
        int sz = 5 * pth_args[i].src_cnt;
        if(sz < 100) sz = 100;
        pth_args[i].src = malloc(sz * sizeof(bank_t));
        pth_args[i].bank = malloc(sz * sizeof(bank_t));
    }

#ifdef USE_MPI
    base_parallel.bank = malloc(2 * base_criti.cycle_neu_num * sizeof(bank_t));
    base_parallel.src = malloc(2 * base_criti.cycle_neu_num * sizeof(bank_t));
#endif

    double ksi1, ksi2, ksi3;
    bank_t *ksrc;
    int ksrc_cnt = 0;
    switch(base_criti.ksrc_type) {
        case POINT: {
            for(i = 0; i < base_num_threads; i++) {
                ksrc_cnt = pth_args[i].src_cnt;
                for(j = 0; j < ksrc_cnt; j++) {
                    get_rand_seed_host(&base_RNG);
                    ksrc = &pth_args[i].src[j];
                    ksrc->pos[0] = base_criti.ksrc_para[0];
                    ksrc->pos[1] = base_criti.ksrc_para[1];
                    ksrc->pos[2] = base_criti.ksrc_para[2];
                }
            }
            break;
        }
        case SLAB: {
            double len_x = base_criti.ksrc_para[1] - base_criti.ksrc_para[0];
            double len_y = base_criti.ksrc_para[3] - base_criti.ksrc_para[2];
            double len_z = base_criti.ksrc_para[5] - base_criti.ksrc_para[4];

            for(i = 0; i < base_num_threads; i++) {
                ksrc_cnt = pth_args[i].src_cnt;
                for(j = 0; j < ksrc_cnt; j++) {
                    get_rand_seed_host(&base_RNG);
                    ksrc = &pth_args[i].src[j];
                    ksrc->pos[0] = base_criti.ksrc_para[0] + get_rand_host(&base_RNG) * len_x;
                    ksrc->pos[1] = base_criti.ksrc_para[1] + get_rand_host(&base_RNG) * len_y;
                    ksrc->pos[2] = base_criti.ksrc_para[2] + get_rand_host(&base_RNG) * len_z;
                }
            }
            break;
        }
        case SPHERE: {
            for(i = 0; i < base_num_threads; i++) {
                ksrc_cnt = pth_args[i].src_cnt;
                for(j = 0; j < ksrc_cnt; j++) {
                    get_rand_seed_host(&base_RNG);
                    ksrc = &pth_args[i].src[j];
                    do {
                        ksi1 = TWO * get_rand_host(&base_RNG) - ONE;
                        ksi2 = TWO * get_rand_host(&base_RNG) - ONE;
                        ksi3 = TWO * get_rand_host(&base_RNG) - ONE;
                    } while(SQUARE(ksi1) + SQUARE(ksi2) + SQUARE(ksi3) > 1);
                    ksrc->pos[0] = base_criti.ksrc_para[0] + base_criti.ksrc_para[3] * ksi1;
                    ksrc->pos[1] = base_criti.ksrc_para[1] + base_criti.ksrc_para[3] * ksi2;
                    ksrc->pos[2] = base_criti.ksrc_para[2] + base_criti.ksrc_para[3] * ksi3;
                }
            }
            break;
        }
    }

    base_RNG.position_pre = -1000;
    base_RNG.position = 0;

#ifdef USE_MPI
    base_RNG.position = base_parallel.rand_num_pos[base_parallel.id];
#endif

    for(i = 0; i < base_num_threads; i++) {
        ksrc_cnt = pth_args[i].src_cnt;
        for(j = 0; j < ksrc_cnt; j++) {
            get_rand_seed_host(&base_RNG);
            ksi1 = get_rand_host(&base_RNG);
            ksi2 = get_rand_host(&base_RNG);
            ksrc = &pth_args[i].src[j];
            ksrc->dir[0] = TWO * ksi2 - ONE;
            ksrc->dir[1] = sqrt(ONE - SQUARE(ksrc->dir[0])) * cos(TWO * PI * ksi1);
            ksrc->dir[2] = sqrt(ONE - SQUARE(ksrc->dir[0])) * sin(TWO * PI * ksi1);

            double T = 4.0 / 3.0;
            ksrc->erg = sample_maxwell_host(&base_RNG, T);
        }
    }

    base_start_wgt = ONE;

    base_RNG.position_pre = -1000;
    base_RNG.position = 0;

#ifdef USE_MPI
    base_RNG.position = base_parallel.rand_num_pos[base_parallel.id];

    if(IS_MASTER)
#endif
        puts("Finished.");
}