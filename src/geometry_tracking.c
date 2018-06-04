//
// Created by xaq on 10/27/17.
//

#include "neutron_transport.h"
#include "geometry.h"
#include "tally.h"


extern tally_t base_tally;


void
geometry_tracking(particle_status_t *par_status,
                  double *keff_wgt_sum,
                  nuc_xs_t *nuc_xs,
                  RNG_t *RNG,
                  int act_cycle)
{
    double FFL;    /* free fly length */
    double DTB;    /* distance to boundary */
    double distance;
    int iter_cnt = 0;
    bool par_on_surf = false;
    par_status->surf = 0;    /* particle is not on surface at hte beginning */

    do {
        if(iter_cnt++ > MAX_ITER) {
            par_status->is_killed = true;
            puts("Too many times of surface crossing.");
            base_warnings++;
            return;
        }

        if(par_on_surf)
            find_next_cell(par_status);

        if(par_status->is_killed)
            return;

        DTB = calc_dist_to_bound(par_status);
        if(LT_ZERO(DTB)) {
            puts("Failed to calculate distance to boundary.");
            par_status->is_killed = true;
            DTB = ZERO;
        }

        FFL = sample_free_fly_dis(par_status, nuc_xs, RNG, !par_on_surf);

        if(FFL >= DTB) {
            par_on_surf = true;
            distance = DTB;
        } else {
            par_on_surf = false;
            distance = FFL;
        }

        keff_wgt_sum[2] += par_status->wgt * par_status->macro_nu_fis_cs * distance;

        if(act_cycle > 0) {
            int i;
            if(base_tally.cell_tally_sz)
                for(i = 0; i < base_tally.cell_tally_sz; i++)
                    base_tally.cell_tallies[i]->do_tally(base_tally.cell_tallies[i], par_status, distance);

            if(base_tally.mesh_tally_sz)
                for(i = 0; i < base_tally.mesh_tally_sz; i++)
                    base_tally.mesh_tallies[i]->do_tally(base_tally.mesh_tallies[i], par_status, distance);
        }

        Fly_by_length(distance);
    } while(par_on_surf);

    keff_wgt_sum[0] += par_status->wgt * par_status->macro_nu_fis_cs / par_status->macro_tot_cs;
}
