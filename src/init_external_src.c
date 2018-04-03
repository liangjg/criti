//
// Created by 叶鑫 on 2018/3/26.
//


#include "fixed_source.h"
#include "sample_method.h"


extern fixed_src_t base_fixed_src;
extern RNG_t base_RNG;
extern double base_start_wgt;
extern nuc_xs_t **base_nuc_xs;
extern int base_num_threads;

#define BANK_SZ    256

void
init_external_src(pth_arg_t *pth_args)
{
    double ksi1, ksi2, ksi3;
    int tot_neu_num;
    int quotient, remainder;
    int i, j;
    int fsrc_cnt;
    bank_t *fsrc;

    printf("Initiating external source...");
    tot_neu_num = base_fixed_src.tot_neu_num;
    base_fixed_src.tot_start_wgt = ONE * tot_neu_num;

    quotient = tot_neu_num / base_num_threads;
    remainder = tot_neu_num - quotient * base_num_threads;
    for(i = 0; i < base_num_threads; i++) {
        pth_args[i].src_cnt = quotient;
        pth_args[i].nuc_xs = base_nuc_xs[i];
    }

    if(remainder)
        for(i = 0; i < remainder; i++)
            pth_args[i].src_cnt++;

    for(i = 0; i < base_num_threads; i++) {
        pth_args[i].src = malloc(pth_args[i].src_cnt * sizeof(bank_t));
        pth_args[i].bank = malloc(BANK_SZ * sizeof(bank_t));
    }

    switch(base_fixed_src.fsrc_type) {
        case POINT: {
            for(i = 0; i < base_num_threads; i++) {
                fsrc_cnt = pth_args[i].src_cnt;
                for(j = 0; j < fsrc_cnt; j++) {
                    get_rand_seed_host(&base_RNG);
                    fsrc = &pth_args[i].src[j];
                    fsrc->pos[0] = base_fixed_src.fsrc_paras[0];
                    fsrc->pos[1] = base_fixed_src.fsrc_paras[1];
                    fsrc->pos[2] = base_fixed_src.fsrc_paras[2];
                }
            }
            break;
        }
        case SLAB: {
            double len_x = base_fixed_src.fsrc_paras[1] - base_fixed_src.fsrc_paras[0];
            double len_y = base_fixed_src.fsrc_paras[3] - base_fixed_src.fsrc_paras[2];
            double len_z = base_fixed_src.fsrc_paras[5] - base_fixed_src.fsrc_paras[4];

            for(i = 0; i < base_num_threads; i++) {
                fsrc_cnt = pth_args[i].src_cnt;
                for(j = 0; j < fsrc_cnt; j++) {
                    get_rand_seed_host(&base_RNG);
                    fsrc = &pth_args[i].src[j];
                    fsrc->pos[0] = base_fixed_src.fsrc_paras[0] + get_rand_host(&base_RNG) * len_x;
                    fsrc->pos[1] = base_fixed_src.fsrc_paras[1] + get_rand_host(&base_RNG) * len_y;
                    fsrc->pos[2] = base_fixed_src.fsrc_paras[2] + get_rand_host(&base_RNG) * len_z;
                }
            }
            break;
        }
        case SPHERE: {
            for(i = 0; i < base_num_threads; i++) {
                fsrc_cnt = pth_args[i].src_cnt;
                for(j = 0; j < fsrc_cnt; j++) {
                    get_rand_seed_host(&base_RNG);
                    fsrc = &pth_args[i].src[j];
                    do {
                        ksi1 = TWO * get_rand_host(&base_RNG) - ONE;
                        ksi2 = TWO * get_rand_host(&base_RNG) - ONE;
                        ksi3 = TWO * get_rand_host(&base_RNG) - ONE;
                    } while(SQUARE(ksi1) + SQUARE(ksi2) + SQUARE(ksi3) > ONE);
                    fsrc->pos[0] = base_fixed_src.fsrc_paras[0] + base_fixed_src.fsrc_paras[3] * ksi1;
                    fsrc->pos[1] = base_fixed_src.fsrc_paras[1] + base_fixed_src.fsrc_paras[3] * ksi2;
                    fsrc->pos[2] = base_fixed_src.fsrc_paras[2] + base_fixed_src.fsrc_paras[3] * ksi3;
                }
            }
            break;
        }
        case CYL_X: {
            double height = base_fixed_src.fsrc_paras[4] - base_fixed_src.fsrc_paras[3];
            for(i = 0; i < base_num_threads; i++) {
                fsrc_cnt = pth_args[i].src_cnt;
                for(j = 0; j < fsrc_cnt; j++) {
                    get_rand_seed_host(&base_RNG);
                    fsrc = &pth_args[i].src[j];
                    do {
                        ksi1 = TWO * get_rand_host(&base_RNG) - ONE;
                        ksi2 = TWO * get_rand_host(&base_RNG) - ONE;
                    } while(SQUARE(ksi1) + SQUARE(ksi2) > ONE);
                    fsrc->pos[0] = base_fixed_src.fsrc_paras[3] + get_rand_host(&base_RNG) * height;
                    fsrc->pos[1] = base_fixed_src.fsrc_paras[0] + base_fixed_src.fsrc_paras[2] * ksi1;
                    fsrc->pos[2] = base_fixed_src.fsrc_paras[1] + base_fixed_src.fsrc_paras[2] * ksi2;
                }
            }
            break;
        }
        case CYL_Y: {
            double height = base_fixed_src.fsrc_paras[4] - base_fixed_src.fsrc_paras[3];
            for(i = 0; i < base_num_threads; i++) {
                fsrc_cnt = pth_args[i].src_cnt;
                for(j = 0; j < fsrc_cnt; j++) {
                    get_rand_seed_host(&base_RNG);
                    fsrc = &pth_args[i].src[j];
                    do {
                        ksi1 = TWO * get_rand_host(&base_RNG) - ONE;
                        ksi2 = TWO * get_rand_host(&base_RNG) - ONE;
                    } while(SQUARE(ksi1) + SQUARE(ksi2) > ONE);
                    fsrc->pos[0] = base_fixed_src.fsrc_paras[0] + base_fixed_src.fsrc_paras[2] * ksi1;
                    fsrc->pos[1] = base_fixed_src.fsrc_paras[3] + get_rand_host(&base_RNG) * height;
                    fsrc->pos[2] = base_fixed_src.fsrc_paras[1] + base_fixed_src.fsrc_paras[2] * ksi2;
                }
            }
            break;
        }
        case CYL_Z: {
            double height = base_fixed_src.fsrc_paras[4] - base_fixed_src.fsrc_paras[3];
            for(i = 0; i < base_num_threads; i++) {
                fsrc_cnt = pth_args[i].src_cnt;
                for(j = 0; j < fsrc_cnt; j++) {
                    get_rand_seed_host(&base_RNG);
                    fsrc = &pth_args[i].src[j];
                    do {
                        ksi1 = TWO * get_rand_host(&base_RNG) - ONE;
                        ksi2 = TWO * get_rand_host(&base_RNG) - ONE;
                    } while(SQUARE(ksi1) + SQUARE(ksi2) > ONE);
                    fsrc->pos[0] = base_fixed_src.fsrc_paras[0] + base_fixed_src.fsrc_paras[2] * ksi1;
                    fsrc->pos[1] = base_fixed_src.fsrc_paras[1] + base_fixed_src.fsrc_paras[2] * ksi2;
                    fsrc->pos[2] = base_fixed_src.fsrc_paras[3] + get_rand_host(&base_RNG) * height;
                }
            }
            break;
        }
    }

    base_RNG.position_pre = -1000;
    base_RNG.position = 0;

    for(i = 0; i < base_num_threads; i++) {
        fsrc_cnt = pth_args[i].src_cnt;
        for(j = 0; j < fsrc_cnt; j++) {
            get_rand_seed_host(&base_RNG);
            ksi1 = get_rand_host(&base_RNG);
            ksi2 = get_rand_host(&base_RNG);
            fsrc = &pth_args[i].src[j];
            fsrc->dir[0] = TWO * ksi2 - ONE;
            fsrc->dir[1] = sqrt(ONE - SQUARE(fsrc->dir[0])) * cos(TWO * PI * ksi1);
            fsrc->dir[2] = sqrt(ONE - SQUARE(fsrc->dir[0])) * sin(TWO * PI * ksi1);
            if(base_fixed_src.fsrc_erg > ZERO)
                fsrc->erg = base_fixed_src.fsrc_erg;
            else {
                double T = 4.0 / 3.0;
                fsrc->erg = sample_maxwell_host(&base_RNG, T);
            }
        }
    }

    base_start_wgt = ONE;

    base_RNG.position_pre = -1000;
    base_RNG.position = 1;

    puts("Finished.");
}
