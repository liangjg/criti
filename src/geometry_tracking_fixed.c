//
// Created by x1314aq on 18-1-17.
//

#include "fixed_source.h"
#include "geometry.h"
#include "neutron_transport.h"


extern fixed_src_t base_fixed_src;

void geometry_tracking_fixed(particle_status_t *par_status){
    double FFL;    /* free fly length */
    double DTB;    /* distance to boundary */
    double distance;
    int iter_cnt = 0;
    bool par_on_surf = false;
    par_status->surf = 0;    /* particle is not on surface at hte beginning */

    do{
        if(iter_cnt++ > MAX_ITER){
            par_status->is_killed = true;
            puts("too many times of surface crossing.");
            base_warnings++;
            return;
        }

        if(par_on_surf)
            find_next_cell(par_status);

        if(par_status->is_killed) return;

        DTB = calc_dist_to_bound(par_status);
        if(LT_ZERO(DTB)){
            puts("failed to calculate distance to boundary.");
            par_status->is_killed = true;
            DTB = ZERO;
        }

        FFL = sample_free_fly_dis(par_status, NULL, NULL, !par_on_surf);

        if(FFL >= DTB){
            par_on_surf = true;
            distance = DTB;
        } else{
            par_on_surf = false;
            distance = FFL;
        }

        Fly_by_length(distance);
    } while(par_on_surf);
}