//
// Created by xaq on 10/27/17.
//

#include "neutron_transport.h"
#include "criticality.h"
#include "particle_state.h"
#include "geometry.h"

extern criti_t base_criti;

extern particle_state_t base_par_state;

void geometry_tracking(){
    double FFL;    /* free fly length */
    double DTB;    /* distance to boundary */

    FFL = sample_free_fly_dis(true);

//    DTB = calc_dist_to_bound();

    if(DTB < ZERO){
        puts("failed to calculate distance to boundary.");
        base_par_state.is_killed = true;
        return;
    }

    int iter_cnt = 1;

    while(FFL >= DTB){
        if(iter_cnt++ > MAX_ITER){
            base_par_state.is_killed = true;
            puts("too many times of surface crossing.");
            return;
        }

        Estimate_keff_tl(base_par_state.wgt, 123, DTB);

        Fly_by_length(DTB);

//        find_next_cell();

        if(base_par_state.is_killed) return;

//        DTB = calc_dist_to_bound();
        if(DTB < ZERO){
            puts("failed to calculate distance to boundary.");
            base_par_state.is_killed = true;
            return;
        }

        FFL = sample_free_fly_dis(false);
    }

    Estimate_keff_tl(base_par_state.wgt, 0, FFL);

    Estimate_keff_col(base_par_state.wgt, 0, 0);

    Fly_by_length(DTB);
}
