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
    printf("Initiating fission source...");

    base_criti.cycle_neutron_num = base_criti.neu_num_per_cycle;
    base_criti.tot_start_wgt = 1.0 * base_criti.cycle_neutron_num;

    size_t general_bank_sz = (size_t)(1.2 * base_criti.neu_num_per_cycle);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-arith"
    base_criti.fission_bank.ele_size = sizeof(fission_bank_t);
    base_criti.fission_bank.start = malloc(general_bank_sz * sizeof(fission_bank_t));
    base_criti.fission_bank.value_free = NULL;
    base_criti.fission_bank.end_of_storage = base_criti.fission_bank.start + general_bank_sz * sizeof(fission_bank_t);
    base_criti.fission_bank.finish = base_criti.fission_bank.start;

    base_criti.fission_src.ele_size = sizeof(fission_bank_t);
    base_criti.fission_src.start = malloc(general_bank_sz * sizeof(fission_bank_t));
    base_criti.fission_src.value_free = NULL;
    base_criti.fission_src.end_of_storage = base_criti.fission_src.start + general_bank_sz * sizeof(fission_bank_t);
    base_criti.fission_src.finish = base_criti.fission_src.start;
#pragma GCC diagnostic pop

    double ksi1, ksi2, ksi3;
    fission_bank_t *fission_src;
    int source_cnt = base_criti.cycle_neutron_num;
    switch(base_criti.ksrc_type){
        case POINT:{
            for(int i = 0; i < source_cnt; i++){
                get_rand_seed();
                fission_src = (fission_bank_t *)vector_at(&base_criti.fission_src, i);
                fission_src->pos[0] = base_criti.ksrc_para[0];
                fission_src->pos[1] = base_criti.ksrc_para[1];
                fission_src->pos[2] = base_criti.ksrc_para[2];
            }
            break;
        }
        case SLAB:{
            double len_x = base_criti.ksrc_para[1] - base_criti.ksrc_para[0];
            double len_y = base_criti.ksrc_para[3] - base_criti.ksrc_para[2];
            double len_z = base_criti.ksrc_para[5] - base_criti.ksrc_para[4];

            for(int i = 0; i < source_cnt; i++){
                get_rand_seed();
                fission_src = (fission_bank_t *)vector_at(&base_criti.fission_src, i);
                fission_src->pos[0] = base_criti.ksrc_para[0] + get_rand() * len_x;
                fission_src->pos[1] = base_criti.ksrc_para[1] + get_rand() * len_y;
                fission_src->pos[2] = base_criti.ksrc_para[2] + get_rand() * len_z;
            }
            break;
        }
        case SPHERE:{
            for(int i = 0; i < source_cnt; i++){
                get_rand_seed();
                fission_src = (fission_bank_t *)vector_at(&base_criti.fission_src, i);
                do{
                    ksi1 = TWO * get_rand() - ONE;
                    ksi2 = TWO * get_rand() - ONE;
                    ksi3 = TWO * get_rand() - ONE;
                } while(SQUARE(ksi1) + SQUARE(ksi2) + SQUARE(ksi3) > 1);
                fission_src->pos[0] = base_criti.ksrc_para[0] + base_criti.ksrc_para[3] * ksi1;
                fission_src->pos[1] = base_criti.ksrc_para[1] + base_criti.ksrc_para[3] * ksi2;
                fission_src->pos[2] = base_criti.ksrc_para[2] + base_criti.ksrc_para[3] * ksi3;
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
        fission_src = (fission_bank_t *)vector_at(&base_criti.fission_src, i);
        fission_src->dir[0] = TWO * ksi2 - ONE;
        fission_src->dir[1] = sqrt(ONE - SQUARE(fission_src->dir[0])) * cos(TWO * PI * ksi1);
        fission_src->dir[2] = sqrt(ONE - SQUARE(fission_src->dir[0])) * sin(TWO * PI * ksi1);

        double T = 4.0 / 3.0;
        fission_src->erg = sample_maxwell(T);
    }

    base_start_wgt = ONE;

    base_RNG.position_pre = -1000;
    base_RNG.position = 0;

    puts("Finished.");
}