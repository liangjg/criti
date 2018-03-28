//
// Created by xaq on 10/26/17.
//

#include "criticality.h"
#include "IO_releated.h"
#include <athread.h>


extern SLAVE_FUN (do_calc)(void *args);

/*static void do_calc_master(void *args);*/

extern criti_t base_criti;
extern int base_num_threads;
extern RNG_t base_RNG;

void
calc_criticality()
{
    int cyc, i, j, skip_src;
    pth_arg_t *pth_args;

    pth_args = malloc((base_num_threads + 1) * sizeof(pth_arg_t));

    /* 初始化裂变源 */
    init_fission_source(pth_args);

    athread_init();

    for(cyc = 1; cyc <= base_criti.tot_cycle_num; cyc++) {
        memcpy(&pth_args[0].RNG, &base_RNG, sizeof(RNG_t));

        for(i = 0; i < base_num_threads; i++) {
            athread_create(i, do_calc, &pth_args[i]);
            memcpy(&pth_args[i + 1].RNG, &pth_args[i].RNG, sizeof(RNG_t));

            skip_src = pth_args[i].src_cnt;
            for(j = 0; j < skip_src; j++)
                get_rand_seed_host(&pth_args[i + 1].RNG);
        }

        /* do_calc_master(&pth_arg[base_num_threads]); */

        for(i = 0; i < base_num_threads; i++)
            athread_wait(i);

        /* 处理这一代计算完之后的结果 */
        process_cycle_end(cyc, pth_args);
    }
    output_summary();

    athread_halt();

    for(i = 0; i < base_num_threads; i++) {
        free(pth_args[i].src);
        free(pth_args[i].bank);
    }
    free(pth_args);
}

/*void
do_calc_master(void *args){
    particle_status_t par_status;
    pth_arg_t *pth_arg;
    int neu, tot_neu, col_cnt, i, j;
    RNG_t *RNG;
    bank_t *fis_src, *cur_src;
    nuc_xs_t *nuc_xs;
    double *keff_wgt_sum, length;
    cell_t *cell;

    pth_arg = (pth_arg_t *) args;
    tot_neu = pth_arg->src_cnt;
    RNG = &pth_arg->RNG;
    fis_src = pth_arg->src;
    nuc_xs = pth_arg->nuc_xs;
    keff_wgt_sum = pth_arg->keff_wgt_sum;

    for(neu = 0; neu < tot_neu; neu++) {
        get_rand_seed_host(RNG);

        [> 抽样要输运的粒子 <]
        memset(&par_status, 0x0, sizeof(particle_status_t));

        cur_src = &fis_src[neu];

        for(i = 0; i < 3; i++) {
            par_status.pos[i] = cur_src->pos[i];
            par_status.dir[i] = cur_src->dir[i];
        }

        par_status.erg = cur_src->erg;
        par_status.wgt = base_start_wgt;
        par_status.cell = locate_particle(&par_status, root_universe, par_status.pos, par_status.dir);

        if(!par_status.cell)
            continue;

        cell = par_status.cell;
        if(cell->imp == 0)
            continue;

        par_status.mat = cell->mat;
        par_status.cell_tmp = cell->tmp;

        [> 输运粒子 <]
        col_cnt = 0;
        do {
            geometry_tracking(&par_status, keff_wgt_sum, nuc_xs, RNG);
            if(par_status.is_killed) break;

            sample_col_nuclide(&par_status, nuc_xs, RNG);
            if(par_status.is_killed) break;

            calc_col_nuc_cs(&par_status, RNG);

            treat_fission(&par_status, pth_arg);

            treat_implicit_capture(&par_status, RNG);
            if(par_status.is_killed) break;

            par_status.collision_type = sample_col_type(&par_status, RNG);
            if(par_status.is_killed) break;

            get_exit_state(&par_status, RNG);
            if(par_status.is_killed) break;

            [> 更新粒子状态信息 <]
            par_status.erg = par_status.exit_erg;
            for(i = 0; i < 3; i++)
                par_status.dir[i] = par_status.exit_dir[i];
            length = ONE / sqrt(SQUARE(par_status.dir[0]) + SQUARE(par_status.dir[1]) + SQUARE(par_status.dir[2]));
            par_status.dir[0] *= length;
            par_status.dir[1] *= length;
            par_status.dir[2] *= length;

        } while(++col_cnt < MAX_ITER);
        pth_arg->col_cnt += col_cnt;
    }
}*/
