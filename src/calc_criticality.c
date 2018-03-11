//
// Created by xaq on 10/26/17.
//


#include "RNG.h"
#include "IO_releated.h"
#include "pth_arg.h"
#include "geometry.h"
#include "neutron_transport.h"
#include "criticality.h"


extern double base_start_wgt;
extern universe_t *root_universe;

void *
do_calc(void *args);

void
calc_criticality(int tot_cycle_num)
{
    void *status;
    pth_arg_t pth_args[NUM_THREADS];
    pthread_t threads[NUM_THREADS];

    init_fission_src(pth_args);

    for(int cyc = 1; cyc <= tot_cycle_num; cyc++) {
        for(int i = 0; i < NUM_THREADS; i++)
            pthread_create(&threads[i], NULL, do_calc, &pth_args[i]);

        for(int i = 0; i < NUM_THREADS; i++)
            pthread_join(threads[i], &status);

        process_cycle_end(cyc, pth_args);
    }
    output_summary();

    for(int i = 0; i < NUM_THREADS; i++){
        free(pth_args[i].fis_src);
        free(pth_args[i].fis_bank);
    }
}

void *
do_calc(void *args)
{
    pth_arg_t *pth_args = (pth_arg_t *) args;
    particle_status_t par_status;

    int tot_neu = pth_args->fis_src_cnt;
    RNG_t *RNG = &pth_args->RNG;
    fission_bank_t *fis_src = pth_args->fis_src;
    nuc_xs_t *nuc_xs = pth_args->nuc_xs;
    double *keff_wgt_sum = pth_args->keff_wgt_sum;

    for(int neu = 0; neu < tot_neu; neu++) {
        get_rand_seed(RNG);

        /* sample fission source */
        memset(&par_status, 0x0, sizeof(particle_status_t));

        fission_bank_t *cur_src = &fis_src[neu];

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
            treat_fission(&par_status, pth_args);

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

        pth_args->col_cnt += col_cnt;
    }

    return NULL;
}