//
// Created by x1314aq on 18-1-16.
//

#include "fixed_source.h"
#include "RNG.h"
#include "sample_method.h"


extern fixed_src_t base_fixed_src;
extern RNG_t base_RNG;
extern double base_start_wgt;

void init_external_source(){
    printf("Initiating external source...");

    base_fixed_src.tot_start_wgt = 1.0 * base_fixed_src.tot_neu_num;
    size_t bank_sz = (size_t) (1.2 * base_fixed_src.tot_neu_num);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-arith"
    base_fixed_src.fixed_src.ele_size = sizeof(fixed_src_bank_t);
    base_fixed_src.fixed_src.start = malloc(bank_sz * sizeof(fixed_src_bank_t));
    base_fixed_src.fixed_src.value_free = NULL;
    base_fixed_src.fixed_src.end_of_storage = base_fixed_src.fixed_src.start + bank_sz * sizeof(fixed_src_bank_t);
    base_fixed_src.fixed_src.finish = base_fixed_src.fixed_src.start;

    base_fixed_src.fixed_src_bank.ele_size = sizeof(fixed_src_bank_t);
    base_fixed_src.fixed_src_bank.start = malloc(bank_sz * sizeof(fixed_src_bank_t));
    base_fixed_src.fixed_src_bank.value_free = NULL;
    base_fixed_src.fixed_src_bank.end_of_storage =
            base_fixed_src.fixed_src_bank.start + bank_sz * sizeof(fixed_src_bank_t);
    base_fixed_src.fixed_src_bank.finish = base_fixed_src.fixed_src_bank.start;
#pragma GCC diagnostic pop

    double ksi1, ksi2, ksi3;
    int src_cnt = base_fixed_src.tot_neu_num;
    fixed_src_bank_t *fixed_src;
    switch(base_fixed_src.src_type){
        case POINT:{
            for(int i = 0; i < src_cnt; i++){
                get_rand_seed();
                fixed_src = (fixed_src_bank_t *) vector_at(&base_fixed_src.fixed_src, i);
                fixed_src->pos[0] = base_fixed_src.src_paras[0];
                fixed_src->pos[1] = base_fixed_src.src_paras[1];
                fixed_src->pos[2] = base_fixed_src.src_paras[2];
            }
            break;
        }
        case SLAB:{
            double len_x = base_fixed_src.src_paras[1] - base_fixed_src.src_paras[0];
            double len_y = base_fixed_src.src_paras[3] - base_fixed_src.src_paras[2];
            double len_z = base_fixed_src.src_paras[5] - base_fixed_src.src_paras[4];
            for(int i = 0; i < src_cnt; i++){
                get_rand_seed();
                fixed_src = (fixed_src_bank_t *) vector_at(&base_fixed_src.fixed_src, i);
                fixed_src->pos[0] = base_fixed_src.src_paras[0] + get_rand() * len_x;
                fixed_src->pos[1] = base_fixed_src.src_paras[1] + get_rand() * len_y;
                fixed_src->pos[2] = base_fixed_src.src_paras[2] + get_rand() * len_z;
            }
            break;
        }
        case SPHERE:{
            for(int i = 0; i < src_cnt; i++){
                get_rand_seed();
                fixed_src = (fixed_src_bank_t *) vector_at(&base_fixed_src.fixed_src, i);
                do{
                    ksi1 = TWO * get_rand() - ONE;
                    ksi2 = TWO * get_rand() - ONE;
                    ksi3 = TWO * get_rand() - ONE;
                } while(SQUARE(ksi1) + SQUARE(ksi2) + SQUARE(ksi3) > ONE);
                fixed_src->pos[0] = base_fixed_src.src_paras[0] + base_fixed_src.src_paras[3] * ksi1;
                fixed_src->pos[1] = base_fixed_src.src_paras[1] + base_fixed_src.src_paras[3] * ksi2;
                fixed_src->pos[2] = base_fixed_src.src_paras[2] + base_fixed_src.src_paras[3] * ksi3;
            }
            break;
        }
        case CYL_X:{
            double height = base_fixed_src.src_paras[4] - base_fixed_src.src_paras[3];
            for(int i = 0; i < src_cnt; i++){
                get_rand_seed();
                fixed_src = (fixed_src_bank_t *) vector_at(&base_fixed_src.fixed_src, i);
                do{
                    ksi1 = TWO * get_rand() - ONE;
                    ksi2 = TWO * get_rand() - ONE;
                } while(SQUARE(ksi1) + SQUARE(ksi2) > ONE);
                fixed_src->pos[0] = base_fixed_src.src_paras[3] + get_rand() * height;
                fixed_src->pos[1] = base_fixed_src.src_paras[0] + base_fixed_src.src_paras[2] * ksi1;
                fixed_src->pos[2] = base_fixed_src.src_paras[1] + base_fixed_src.src_paras[2] * ksi2;
            }
            break;
        }
        case CYL_Y:{
            double height = base_fixed_src.src_paras[4] - base_fixed_src.src_paras[3];
            for(int i = 0; i < src_cnt; i++){
                get_rand_seed();
                fixed_src = (fixed_src_bank_t *) vector_at(&base_fixed_src.fixed_src, i);
                do{
                    ksi1 = TWO * get_rand() - ONE;
                    ksi2 = TWO * get_rand() - ONE;
                } while(SQUARE(ksi1) + SQUARE(ksi2) > ONE);
                fixed_src->pos[0] = base_fixed_src.src_paras[0] + base_fixed_src.src_paras[2] * ksi1;
                fixed_src->pos[1] = base_fixed_src.src_paras[3] + get_rand() * height;
                fixed_src->pos[2] = base_fixed_src.src_paras[1] + base_fixed_src.src_paras[2] * ksi2;
            }
            break;
        }
        case CYL_Z:{
            double height = base_fixed_src.src_paras[4] - base_fixed_src.src_paras[3];
            for(int i = 0; i < src_cnt; i++){
                get_rand_seed();
                fixed_src = (fixed_src_bank_t *) vector_at(&base_fixed_src.fixed_src, i);
                do{
                    ksi1 = TWO * get_rand() - ONE;
                    ksi2 = TWO * get_rand() - ONE;
                } while(SQUARE(ksi1) + SQUARE(ksi2) > ONE);
                fixed_src->pos[0] = base_fixed_src.src_paras[0] + base_fixed_src.src_paras[2] * ksi1;
                fixed_src->pos[1] = base_fixed_src.src_paras[1] + base_fixed_src.src_paras[2] * ksi2;
                fixed_src->pos[2] = base_fixed_src.src_paras[3] + get_rand() * height;
            }
            break;
        }
        default:{
            puts("Unknown initial source type!");
            release_resource();
            exit(0);
        }
    }

    base_RNG.position_pre = -1000;
    base_RNG.position = 0;

    for(int i = 0; i < src_cnt; i++){
        get_rand_seed();

        ksi1 = get_rand();
        ksi2 = get_rand();
        fixed_src = (fixed_src_bank_t *) vector_at(&base_fixed_src.fixed_src, i);
        fixed_src->dir[0] = TWO * ksi2 - ONE;
        fixed_src->dir[1] = sqrt(ONE - SQUARE(fixed_src->dir[0])) * cos(TWO * PI * ksi1);
        fixed_src->dir[2] = sqrt(ONE - SQUARE(fixed_src->dir[0])) * sin(TWO * PI * ksi1);

        if(base_fixed_src.fixed_src_erg > ZERO)
            fixed_src->erg = base_fixed_src.fixed_src_erg;
        else{
            double T = 4.0 / 3.0;
            fixed_src->erg = sample_maxwell(T);
        }
    }

    base_start_wgt = ONE;

    base_RNG.position_pre = -1000;
    base_RNG.position = 1;

    puts("Finished.");
}