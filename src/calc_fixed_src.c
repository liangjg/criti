//
// Created by x1314aq on 18-1-16.
//

#define _GNU_SOURCE

#include <sched.h>
#include "fixed_source.h"
#include "IO_releated.h"
#include "geometry.h"
#include "neutron_transport.h"


extern double base_start_wgt;
extern universe_t *root_universe;
extern fixed_src_t base_fixed_src;
extern int base_num_threads;
extern RNG_t base_RNG;

#ifdef USE_PTHREAD
static int
_set_cpu(int i);
#endif

static void *
do_calc(void *args);

void
calc_fixed_src()
{
    void *status;
    int i, j;
    pth_arg_t *pth_args = malloc(base_num_threads * sizeof(pth_arg_t));

#ifdef USE_PTHREAD
    pthread_t *threads = malloc(base_num_threads * sizeof(pthread_t));
#endif

    init_external_src(pth_args);

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

    printf("neutron: %d\n", base_fixed_src.tot_neu_num);

#ifdef USE_PTHREAD
    for(i = 0; i < base_num_threads - 1; i++)
        pthread_join(threads[i], &status);
#endif

    for(i = 0; i < base_num_threads; i++) {
        base_fixed_src.tot_col_cnt += pth_args[i].col_cnt;
        free(pth_args[i].src);
        free(pth_args[i].bank);
    }
    free(pth_args);

#if USE_PTHREAD
    free(threads);
#endif

    output_summary_fixed();
}

void *
do_calc(void *args)
{
    particle_status_t par_status;
    pth_arg_t *pth_arg = (pth_arg_t *) args;

    int tot_neu = pth_arg->src_cnt;
    RNG_t *RNG = &pth_arg->RNG;
    bank_t *fsrc = pth_arg->src;
    nuc_xs_t *nuc_xs = pth_arg->nuc_xs;

//    _set_cpu(pth_arg->id);

    for(int neu = 0; neu < tot_neu; neu++) {
        if(neu % 1000 == 0 && pth_arg->id == 0)
            printf("neutron: %d\n", neu * base_num_threads);

        get_rand_seed(RNG);

        /* sample fixed source */
        memset(&par_status, 0x0, sizeof(particle_status_t));

        bank_t *cur_src = &fsrc[neu];

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

        while(1) {
            int col_cnt = 0;
            do {
                geometry_tracking_fixed(&par_status, nuc_xs, RNG);
                if(par_status.is_killed) break;

                sample_col_nuclide(&par_status, nuc_xs, RNG);
                if(par_status.is_killed) break;

                calc_col_nuc_cs(&par_status, RNG);

                /* implicit capture(excluding fission) */
                treat_implicit_capture_fixed(&par_status, RNG);
                if(par_status.is_killed) break;

                par_status.collision_type = sample_col_type_fixed(&par_status, RNG);
                if(par_status.is_killed) break;

                get_exit_state_fixed(&par_status, pth_arg);
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

            if(pth_arg->bank_cnt) {
                memset(&par_status, 0x0, sizeof(par_status));

                cur_src = &pth_arg->bank[--pth_arg->bank_cnt];
                for(int i = 0; i < 3; i++) {
                    par_status.pos[i] = cur_src->pos[i];
                    par_status.dir[i] = cur_src->dir[i];
                }

                par_status.erg = cur_src->erg;
                par_status.wgt = cur_src->wgt;
                par_status.cell = locate_particle(&par_status, root_universe, par_status.pos, par_status.dir);

                if(!par_status.cell)
                    continue;

                cell = par_status.cell;
                if(cell->imp == 0)
                    continue;

                par_status.mat = cell->mat;
                par_status.cell_tmp = cell->tmp;
            } else
                break;
        }
    }

    return NULL;
}

#ifdef USE_PTHREAD
int
_set_cpu(int i)
{
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(i, &mask);
    return pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask);
}
#endif