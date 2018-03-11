//
// Created by x1314aq on 18-1-17.
//

#include "fixed_source.h"
#include "neutron_transport.h"


extern fixed_src_t base_fixed_src;

void
track_history_fixed(particle_status_t *par_status)
{
    if(par_status->is_killed) return;

    /* 粒子在当前输运过程中碰撞次数 */
    base_fixed_src.col_cnt = 0;

    do {
        geometry_tracking_fixed(par_status);
        if(par_status->is_killed) break;

        sample_col_nuclide(par_status, NULL, NULL);
        if(par_status->is_killed) break;

        calc_col_nuc_cs(par_status, NULL);

        /* implicit capture(excluding fission) */
        treat_implicit_capture_fixed(par_status);
        if(par_status->is_killed) break;

        par_status->collision_type = sample_col_type_fixed(par_status);
        if(par_status->is_killed) break;

        get_exit_state_fixed(par_status);
        if(par_status->is_killed) break;

        /* update particle state */
        par_status->erg = par_status->exit_erg;
        for(int i = 0; i < 3; i++)
            par_status->dir[i] = par_status->exit_dir[i];
        double length = ONE / sqrt(SQUARE(par_status->dir[0]) +
                                   SQUARE(par_status->dir[1]) +
                                   SQUARE(par_status->dir[2]));
        par_status->dir[0] *= length;
        par_status->dir[1] *= length;
        par_status->dir[2] *= length;

    } while(++base_fixed_src.col_cnt < MAX_ITER);
    base_fixed_src.tot_col_cnt += base_fixed_src.col_cnt;
}