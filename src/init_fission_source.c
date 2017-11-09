//
// Created by xaq on 10/26/17.
//

#include "criticality.h"
#include "RNG.h"
#include "sample_method.h"

extern criti_t base_criti;

extern RNG_t base_RNG;

extern double base_start_wgt;

void init_fission_source(){
    puts("Initiating fission source...");

    base_criti.cycle_neutron_num = base_criti.neu_num_per_cycle;
    base_criti.tot_start_wgt = 1.0 * base_criti.cycle_neutron_num;

    int bank_sz = (int)(1.2 * base_criti.neu_num_per_cycle);
    base_criti.fission_bank_sz = bank_sz;
    base_criti.fission_src_sz = bank_sz;

    base_criti.fission_bank = (fission_bank_t *)malloc(bank_sz * sizeof(fission_bank_t));
    base_criti.fission_src = (fission_bank_t *)malloc(bank_sz * sizeof(fission_bank_t));

    double ksi1, ksi2, ksi3;
    int source_cnt = base_criti.cycle_neutron_num;
    switch(base_criti.ksrc_type){
        case POINT:{
            for(int i = 0; i < source_cnt; i++){
                get_rand_seed();
                base_criti.fission_src[i].pos[0] = base_criti.ksrc_para[0];
                base_criti.fission_src[i].pos[1] = base_criti.ksrc_para[1];
                base_criti.fission_src[i].pos[2] = base_criti.ksrc_para[2];
            }
            break;
        }
        case SLAB:{
            double len_x = base_criti.ksrc_para[1] - base_criti.ksrc_para[0];
            double len_y = base_criti.ksrc_para[3] - base_criti.ksrc_para[2];
            double len_z = base_criti.ksrc_para[5] - base_criti.ksrc_para[4];

            for(int i = 0; i < source_cnt; i++){
                get_rand_seed();
                base_criti.fission_src[i].pos[0] = base_criti.ksrc_para[0] + get_rand() * len_x;
                base_criti.fission_src[i].pos[1] = base_criti.ksrc_para[1] + get_rand() * len_x;
                base_criti.fission_src[i].pos[2] = base_criti.ksrc_para[2] + get_rand() * len_x;
            }
            break;
        }
        case SPHERE:{
            for(int i = 0; i < source_cnt; i++){
                get_rand_seed();
                do{
                    ksi1 = 2 * get_rand() - 1;
                    ksi2 = 2 * get_rand() - 1;
                    ksi3 = 2 * get_rand() - 1;
                } while(SQUARE(ksi1) + SQUARE(ksi2) + SQUARE(ksi3) > 1);
                base_criti.fission_src[i].pos[0] = base_criti.ksrc_para[0] + base_criti.ksrc_para[3] * ksi1;
                base_criti.fission_src[i].pos[1] = base_criti.ksrc_para[1] + base_criti.ksrc_para[3] * ksi2;
                base_criti.fission_src[i].pos[2] = base_criti.ksrc_para[2] + base_criti.ksrc_para[3] * ksi3;
            }
            break;
        }
        default:
            puts("unknown fission source type.");
            break;
    }

    base_RNG.position_pre = -1000;
    base_RNG.position = 0;

    for(int i = 0; i < source_cnt; i++){
        get_rand_seed();

        ksi1 = get_rand();
        ksi2 = get_rand();
        base_criti.fission_src[i].dir[0] = 2 * ksi2 - 1;
        base_criti.fission_src[i].dir[1] = sqrt(ONE - SQUARE(base_criti.fission_src[i].dir[0])) * cos(TWO * PI * ksi1);
        base_criti.fission_src[i].dir[2] = sqrt(ONE - SQUARE(base_criti.fission_src[i].dir[0])) * sin(TWO * PI * ksi1);

        double T = 4.0 / 3.0;
        base_criti.fission_src[i].erg = sample_maxwell(T);
    }

    base_start_wgt = 1.0;

    base_RNG.position_pre = -1000;
    base_RNG.position = 0;

    puts("Finished.");
}