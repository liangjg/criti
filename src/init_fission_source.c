//
// Created by xaq on 10/26/17.
//

#include "criticality.h"
#include "sample_method.h"


extern criti_t base_criti;
extern RNG_t base_RNG;
extern double base_start_wgt;
extern RNG_t RNGs[NUMBERS_SLAVES];

void init_fission_source(){
    int i, j;
    int remainder1, remainder2;
    int quotient;

    printf("Initiating fission source...");

    base_criti.tot_start_wgt = ONE * base_criti.cycle_neutron_num;

    /* 计算第一代的fission_src_cnt */
    base_criti.tot_transfer_num = base_criti.cycle_neutron_num / NUMBERS_PER_TRANS;
    remainder1 = base_criti.cycle_neutron_num - NUMBERS_PER_TRANS * base_criti.tot_transfer_num;
    for(i = 0; i < NUMBERS_SLAVES; i++)
        base_criti.fission_src_cnt[i] = base_criti.tot_transfer_num * 400;
    if(remainder1 > 0){
        base_criti.tot_transfer_num++;
        quotient = remainder1 / NUMBERS_SLAVES;
        remainder2 = remainder1 - quotient * NUMBERS_SLAVES;
        for(i = 0; i < NUMBERS_SLAVES; i++)
            base_criti.fission_src_cnt[i] += quotient;
        if(remainder2 > 0)
            for(i = 0; i < remainder2; i++)
                base_criti.fission_src_cnt[i] += 1;
    }

    for(i = 0; i < NUMBERS_SLAVES; i++){
        int sz =  2 * base_criti.fission_src_cnt[i];
        if(sz < 100) sz = 100;
        base_criti.fission_src[i] = malloc(sz * sizeof(fission_bank_t));
        base_criti.fission_bank[i] = malloc(sz * sizeof(fission_bank_t));
    }

    double ksi1, ksi2, ksi3;
    fission_bank_t *fission_src;
    int fis_src_cnt = 0;
    switch(base_criti.ksrc_type){
        case POINT:{
            for(i = 0; i < NUMBERS_SLAVES; i++){
                fis_src_cnt = base_criti.fission_src_cnt[i];
                for(j = 0; j < fis_src_cnt; j++){
                    get_rand_seed_host(&base_RNG);
                    fission_src = &base_criti.fission_src[i][j];
                    fission_src->pos[0] = base_criti.ksrc_para[0];
                    fission_src->pos[1] = base_criti.ksrc_para[1];
                    fission_src->pos[2] = base_criti.ksrc_para[2];
                }
            }
            break;
        }
        case SLAB:{
            double len_x = base_criti.ksrc_para[1] - base_criti.ksrc_para[0];
            double len_y = base_criti.ksrc_para[3] - base_criti.ksrc_para[2];
            double len_z = base_criti.ksrc_para[5] - base_criti.ksrc_para[4];

            for(i = 0; i < NUMBERS_SLAVES; i++){
                fis_src_cnt = base_criti.fission_src_cnt[i];
                for(j = 0; j < fis_src_cnt; j++){
                    get_rand_seed_host(&base_RNG);
                    fission_src = &base_criti.fission_src[i][j];
                    fission_src->pos[0] = base_criti.ksrc_para[0] + get_rand_host(&base_RNG) * len_x;
                    fission_src->pos[1] = base_criti.ksrc_para[1] + get_rand_host(&base_RNG) * len_y;
                    fission_src->pos[2] = base_criti.ksrc_para[2] + get_rand_host(&base_RNG) * len_z;
                }
            }
            break;
        }
        case SPHERE:{
            for(i = 0; i < NUMBERS_SLAVES; i++){
                fis_src_cnt = base_criti.fission_src_cnt[i];
                for(j = 0; j < fis_src_cnt; j++){
                    get_rand_seed_host(&base_RNG);
                    fission_src = &base_criti.fission_src[i][j];
                    do{
                        ksi1 = TWO * get_rand_host(&base_RNG) - ONE;
                        ksi2 = TWO * get_rand_host(&base_RNG) - ONE;
                        ksi3 = TWO * get_rand_host(&base_RNG) - ONE;
                    } while(SQUARE(ksi1) + SQUARE(ksi2) + SQUARE(ksi3) > 1);
                    fission_src->pos[0] = base_criti.ksrc_para[0] + base_criti.ksrc_para[3] * ksi1;
                    fission_src->pos[1] = base_criti.ksrc_para[1] + base_criti.ksrc_para[3] * ksi2;
                    fission_src->pos[2] = base_criti.ksrc_para[2] + base_criti.ksrc_para[3] * ksi3;
                }
            }
            break;
        }
        default:
            puts("unknown fission source type.");
            break;
    }

    base_RNG.position_pre = -1000;
    base_RNG.position = 0;

    for(i = 0; i < NUMBERS_SLAVES; i++){
        fis_src_cnt = base_criti.fission_src_cnt[i];
        for(j = 0; j < fis_src_cnt; j++){
            get_rand_seed_host(&base_RNG);

            ksi1 = get_rand_host(&base_RNG);
            ksi2 = get_rand_host(&base_RNG);
            fission_src = &base_criti.fission_src[i][j];
            fission_src->dir[0] = TWO * ksi2 - ONE;
            fission_src->dir[1] = sqrt(ONE - SQUARE(fission_src->dir[0])) * cos(TWO * PI * ksi1);
            fission_src->dir[2] = sqrt(ONE - SQUARE(fission_src->dir[0])) * sin(TWO * PI * ksi1);

            double T = 4.0 / 3.0;
            fission_src->erg = sample_maxwell_host(&base_RNG, T);
        }
    }

    base_start_wgt = ONE;

    base_RNG.position_pre = -1000;
    base_RNG.position = 0;

    /* 为所有从核准备初始的RNG */
    memcpy(&RNGs[0], &base_RNG, sizeof(RNG_t));    /* 将当前的base_RNG复制到RNGs[0]，也就是准备好了0号从核的RNG */
    for(i = 1; i < NUMBERS_SLAVES; i++){       /* 准备1～63号从核的RNG */
        memcpy(&RNGs[i], &RNGs[i - 1], sizeof(RNG_t));

        int skip_src = base_criti.fission_src_cnt[i - 1];
        for(j = 1; j < skip_src; j++)
            get_rand_seed_host(&RNGs[i]);
    }

    puts("Finished.");
}