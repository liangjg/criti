//
// Created by xaq on 10/27/17.
//

#include "criticality.h"
#include "neutron_transport.h"


extern criti_t base_criti;

void track_history(particle_state_t *par_state){
    if(par_state->is_killed) return;

    /* 粒子在当前输运过程中发生碰撞的次数 */
    base_criti.col_cnt = 0;

    do{
        /* geometry tracking: free flying */
        geometry_tracking(par_state);
        if(par_state->is_killed) break;

        /* sample collision nuclide */
        sample_col_nuclide(par_state);
        if(par_state->is_killed) break;

        /* calculate cross-section */
        calc_col_nuc_cs(par_state);

        /* treat fission */
        treat_fission(par_state);

        /* implicit capture(including fission) */
        treat_implicit_capture(par_state);
        if(par_state->is_killed) break;

        /* sample collision type */
        par_state->collision_type = sample_col_type(par_state);
        if(par_state->is_killed) break;

        /* sample exit state */
        get_exit_state(par_state);
        if(par_state->is_killed) break;

        /* update particle state */
        par_state->erg = par_state->exit_erg;
        for(int i = 0; i < 3; i++)
            par_state->dir[i] = par_state->exit_dir[i];
        double length = ONE / sqrt(SQUARE(par_state->dir[0]) +
                                   SQUARE(par_state->dir[1]) +
                                   SQUARE(par_state->dir[2]));
        par_state->dir[0] *= length;
        par_state->dir[1] *= length;
        par_state->dir[2] *= length;

    } while(++base_criti.col_cnt < MAX_ITER);

    base_criti.tot_col_cnt += base_criti.col_cnt;
}