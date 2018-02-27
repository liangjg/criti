//
// Created by xaq on 10/27/17.
//

#include "neutron_transport.h"
#include "criticality.h"
#include "geometry.h"


void geometry_tracking(particle_state_t *par_state, double *keff_wgt_sum_slave, nuc_cs_t *nuc_cs_slave, RNG_t *RNG_slave){
    double FFL;
    double DTB;
    double distance;
    int iter_cnt = 0;
    bool par_on_surf = false;
    par_state->surf = 0;

    do{
        if(iter_cnt++ > MAX_ITER){
            par_state->is_killed = true;
            puts("too many times of surface crossing.");
            base_warnings++;
            return;
        }

        if(par_on_surf)
            find_next_cell(par_state);

        if(par_state->is_killed) return;

        DTB = calc_dist_to_bound(par_state);
        if(LT_ZERO(DTB)){
            puts("failed to calculate distance to boundary.");
            par_state->is_killed = true;
            DTB = ZERO;
        }

        FFL = sample_free_fly_dis(par_state, nuc_cs_slave, RNG_slave, !par_on_surf);

        if(FFL >= DTB){
            par_on_surf = true;
            distance = DTB;
        } else{
            par_on_surf = false;
            distance = FFL;
        }

        Estimate_keff_tl(par_state->wgt, par_state->macro_nu_fis_cs, distance);

        Fly_by_length(distance);
    } while(par_on_surf);

    Estimate_keff_col(par_state->wgt, par_state->macro_nu_fis_cs, par_state->macro_tot_cs);
}
