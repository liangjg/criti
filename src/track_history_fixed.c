//
// Created by x1314aq on 18-1-17.
//

#include "fixed_source.h"
#include "neutron_transport.h"


extern fixed_src_t base_fixed_src;

void track_history_fixed(particle_state_t *par_state){
    if(par_state->is_killed) return;

    /* 粒子在当前输运过程中碰撞次数 */
    base_fixed_src.col_cnt = 0;

    do{
        geometry_tracking_fixed(par_state);
        if(par_state->is_killed) return;

        sample_col_nuclide(par_state);
        if(par_state->is_killed) return;

        calc_col_nuc_cs(par_state);

        /* implicit capture(excluding fission) */
        treat_implicit_capture(par_state);
        if(par_state->is_killed) return;

        par_state->collision_type = sample_col_type_fixed(par_state);
        if(par_state->is_killed) return;

        get_exit_state_fixed(par_state);
        if(par_state->is_killed) return;

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

    } while(++base_fixed_src.col_cnt < MAX_ITER);
    base_fixed_src.tot_col_cnt += base_fixed_src.col_cnt;
}