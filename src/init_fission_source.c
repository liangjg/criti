//
// Created by xaq on 10/26/17.
//

#include "criticality.h"
#include "RNG.h"
#include "sample_method.h"


#define NUMBERS_PER_TRANS    25600
#define NUMBERS_SLAVES       64

extern criti_t base_criti;
extern RNG_t base_RNG;
extern double base_start_wgt;

void init_fission_source(){
    int remainder1, remainder2;
    int quotient;

    printf("Initiating fission source...");

    base_criti.tot_start_wgt = 1.0 * base_criti.cycle_neutron_num;

    /* 计算第一代的fission_src_cnt */
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

    for(int i = 0; i < NUMBERS_SLAVES; i++){
        size_t sz = (size_t) (1.2 * base_criti.fission_src_cnt[i]);
        base_criti.fission_src[i] = (fission_bank_t *) malloc(sz * sizeof(fission_bank_t));
        base_criti.fission_bank[i] = (fission_bank_t *) malloc(sz * sizeof(fission_bank_t));
    }

    double ksi1, ksi2, ksi3;
    fission_bank_t *fission_src;
    int fis_src_cnt = 0;
    switch(base_criti.ksrc_type){
        case POINT:{
            for(int i = 0; i < NUMBERS_SLAVES; i++){
                fis_src_cnt = base_criti.fission_src_cnt[i];
                for(int j = 0; j < fis_src_cnt; j++){
                    get_rand_seed();
                    fission_src = base_criti.fission_src[i] + j;
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

            for(int i = 0; i < NUMBERS_SLAVES; i++){
                fis_src_cnt = base_criti.fission_src_cnt[i];
                for(int j = 0; j < fis_src_cnt; j++){
                    get_rand_seed();
                    fission_src = base_criti.fission_src[i] + j;
                    fission_src->pos[0] = base_criti.ksrc_para[0] + get_rand() * len_x;
                    fission_src->pos[1] = base_criti.ksrc_para[1] + get_rand() * len_y;
                    fission_src->pos[2] = base_criti.ksrc_para[2] + get_rand() * len_z;
                }
            }
            break;
        }
        case SPHERE:{
            for(int i = 0; i < NUMBERS_SLAVES; i++){
                fis_src_cnt = base_criti.fission_src_cnt[i];
                for(int j = 0; j < fis_src_cnt; j++){
                    get_rand_seed();
                    fission_src = base_criti.fission_src[i] + j;
                    do{
                        ksi1 = TWO * get_rand() - ONE;
                        ksi2 = TWO * get_rand() - ONE;
                        ksi3 = TWO * get_rand() - ONE;
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

    for(int i = 0; i < NUMBERS_SLAVES; i++){
        fis_src_cnt = base_criti.fission_src_cnt[i];
        for(int j = 0; j < fis_src_cnt; j++){
            get_rand_seed();

            ksi1 = get_rand();
            ksi2 = get_rand();
            fission_src = base_criti.fission_src[i] + j;
            fission_src->dir[0] = TWO * ksi2 - ONE;
            fission_src->dir[1] = sqrt(ONE - SQUARE(fission_src->dir[0])) * cos(TWO * PI * ksi1);
            fission_src->dir[2] = sqrt(ONE - SQUARE(fission_src->dir[0])) * sin(TWO * PI * ksi1);

            double T = 4.0 / 3.0;
            fission_src->erg = sample_maxwell(T);
        }
    }

    base_start_wgt = ONE;

    base_RNG.position_pre = -1000;
    base_RNG.position = 0;

    puts("Finished.");
}