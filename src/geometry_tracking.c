//
// Created by xaq on 10/27/17.
//

#include "neutron_transport.h"
#include "criticality.h"
#include "geometry.h"



void
geometry_tracking(particle_status_t *par_status,
                  double *keff_wgt_sum,
                  nuc_xs_t *nuc_xs,
                  RNG_t *RNG)
{
    double FFL;
    double DTB;
    double distance;
    int iter_cnt = 0;
    bool par_on_surf = false;
    par_status->surf = 0;

    do {
        if(iter_cnt++ > MAX_ITER) {
            par_status->is_killed = true;
            puts("too many times of surface crossing.");
            base_warnings++;
            return;
        }

        if(par_on_surf)
            find_next_cell(par_status);

        if(par_status->is_killed) return;

        DTB = calc_dist_to_bound(par_status);
        if(LT_ZERO(DTB)) {
            puts("failed to calculate distance to boundary.");
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

        Fly_by_length(distance);
    } while(par_on_surf);

    keff_wgt_sum[0] += par_status->wgt * par_status->macro_nu_fis_cs / par_status->macro_tot_cs;
}
