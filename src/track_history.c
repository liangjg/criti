//
// Created by xaq on 10/27/17.
//

#include "criticality.h"
#include "neutron_transport.h"


extern criti_t base_criti;

void
track_history(particle_status_t *par_status)
{
    if(par_status->is_killed) return;

    /* 粒子在当前输运过程中发生碰撞的次数 */
    int col_cnt = 0;

    do {
        /* geometry tracking: free flying */
        geometry_tracking(par_status);
        if(par_status->is_killed) break;

        /* sample collision nuclide */
        sample_col_nuclide(par_status);
        if(par_status->is_killed) break;

        /* calculate cross-section */
        calc_col_nuc_cs(par_status);

        /* treat fission */
        treat_fission(par_status);

        /* implicit capture(including fission) */
        treat_implicit_capture(par_status);
        if(par_status->is_killed) break;

        /* sample collision type */
        par_status->collision_type = sample_col_type(par_status);
        if(par_status->is_killed) break;

        /* sample exit state */
        get_exit_state(par_status);
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

    } while(++col_cnt < MAX_ITER);

    base_criti.tot_col_cnt += col_cnt;
}