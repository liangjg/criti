//
// Created by 叶鑫 on 2018/3/26.
//

#include "fixed_source.h"
#include "IO_releated.h"
#include <athread.h>


extern SLAVE_FUN (do_calc_fixed)(void *args);

extern fixed_src_t base_fixed_src;
extern int base_num_threads;
extern RNG_t base_RNG;

void
calc_fixed_src()
{
    int cyc, i, j, skip_src;
    pth_arg_t *pth_args;

    pth_args = malloc((base_num_threads + 1) * sizeof(pth_arg_t));

    init_external_src(pth_args);

    memcpy(&pth_args[0].RNG, &base_RNG, sizeof(RNG_t));

    athread_init();

    for(i = 0; i < base_num_threads; i++) {
        athread_create(i, do_calc_fixed, &pth_args[i]);
        memcpy(&pth_args[i + 1].RNG, &pth_args[i].RNG, sizeof(RNG_t));

        skip_src = pth_args[i].src_cnt;
        for(j = 0; j < skip_src; j++)
            get_rand_seed(&pth_args[i + 1].RNG);
    }

    for(i = 0; i < base_num_threads; i++)
        athread_wait(i);

    printf("neutron: %d\n", base_fixed_src.tot_neu_num);

    athread_halt();

    for(i = 0; i < base_num_threads; i++) {
        base_fixed_src.tot_col_cnt += pth_args[i].col_cnt;
        free(pth_args[i].src);
        free(pth_args[i].bank);
    }
    free(pth_args);

    output_summary_fixed();
}