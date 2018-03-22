//
// Created by xaq on 10/26/17.
//

#define _GNU_SOURCE

#include <sched.h>
#include "RNG.h"
#include "IO_releated.h"
#include "pth_arg.h"
#include "geometry.h"
#include "neutron_transport.h"
#include "criticality.h"


extern double base_start_wgt;
extern universe_t *root_universe;
extern int base_num_threads;
extern RNG_t base_RNG;

static int
_set_cpu(int i);

static void *
do_calc(void *args);

void
calc_criticality(int tot_cycle_num)
{
    void *status;
    int i, j, cyc;
    pth_arg_t *pth_args = malloc(base_num_threads * sizeof(pth_arg_t));

#ifdef USE_PTHREAD
    pthread_t *threads = malloc(base_num_threads * sizeof(pthread_t));
#endif

    init_fission_src(pth_args);

    for(cyc = 1; cyc <= tot_cycle_num; cyc++) {
        memcpy(&pth_args[0].RNG, &base_RNG, sizeof(RNG_t));

#ifdef USE_PTHREAD
        for(i = 1; i < base_num_threads; i++) {
            pthread_create(&threads[i - 1], NULL, do_calc, &pth_args[i - 1]);
            memcpy(&pth_args[i].RNG, &pth_args[i - 1].RNG, sizeof(RNG_t));

            int skip_src = pth_args[i - 1].src_cnt;
            for(j = 0; j < skip_src; j++)
                get_rand_seed(&pth_args[i].RNG);
        }
#endif

        do_calc(&pth_args[base_num_threads - 1]);

#ifdef USE_PTHREAD
        for(i = 0; i < base_num_threads - 1; i++)
            pthread_join(threads[i], &status);
#endif

        process_cycle_end(cyc, pth_args);
    }
    output_summary();

    for(i = 0; i < base_num_threads; i++) {
        free(pth_args[i].src);
        free(pth_args[i].bank);
    }
    free(pth_args);

#ifdef USE_PTHREAD
    free(threads);
#endif
}

void *
do_calc(void *args)
{
    particle_status_t par_status;
    pth_arg_t *pth_arg = (pth_arg_t *) args;

    int tot_neu = pth_arg->src_cnt;
    RNG_t *RNG = &pth_arg->RNG;
    bank_t *fis_src = pth_arg->src;
    nuc_xs_t *nuc_xs = pth_arg->nuc_xs;
    double *keff_wgt_sum = pth_arg->keff_wgt_sum;

    _set_cpu(pth_arg->id);

    for(int neu = 0; neu < tot_neu; neu++) {
        get_rand_seed(RNG);

        /* sample fission source */
        memset(&par_status, 0x0, sizeof(particle_status_t));

        bank_t *cur_src = &fis_src[neu];

        for(int i = 0; i < 3; i++) {
            par_status.pos[i] = cur_src->pos[i];
            par_status.dir[i] = cur_src->dir[i];
        }

        par_status.erg = cur_src->erg;
        par_status.wgt = base_start_wgt;
        par_status.cell = locate_particle(&par_status, root_universe, par_status.pos, par_status.dir);

        if(!par_status.cell)
            continue;

        cell_t *cell = par_status.cell;
        if(cell->imp == 0)
            continue;

        par_status.mat = cell->mat;
        par_status.cell_tmp = cell->tmp;

        /* 粒子在当前输运过程中发生碰撞的次数 */
        int col_cnt = 0;

        do {
            /* geometry tracking: free flying */
            geometry_tracking(&par_status, keff_wgt_sum, nuc_xs, RNG);
            if(par_status.is_killed) break;

            /* sample collision nuclide */
            sample_col_nuclide(&par_status, nuc_xs, RNG);
            if(par_status.is_killed) break;

            /* calculate cross-section */
            calc_col_nuc_cs(&par_status, RNG);

            /* treat fission */
            treat_fission(&par_status, pth_arg);

            /* implicit capture(including fission) */
            treat_implicit_capture(&par_status, RNG);
            if(par_status.is_killed) break;

            /* sample collision type */
            par_status.collision_type = sample_col_type(&par_status, RNG);
            if(par_status.is_killed) break;

            /* sample exit state */
            get_exit_state(&par_status, RNG);
            if(par_status.is_killed) break;

            /* update particle state */
            par_status.erg = par_status.exit_erg;
            for(int i = 0; i < 3; i++)
                par_status.dir[i] = par_status.exit_dir[i];
            double length = ONE / sqrt(SQUARE(par_status.dir[0]) +
                                       SQUARE(par_status.dir[1]) +
                                       SQUARE(par_status.dir[2]));
            par_status.dir[0] *= length;
            par_status.dir[1] *= length;
            par_status.dir[2] *= length;

        } while(++col_cnt < MAX_ITER);

        pth_arg->col_cnt += col_cnt;
    }

    return NULL;
}

int
_set_cpu(int i)
{
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(i, &mask);
    return pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask);
}