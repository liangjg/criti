//
// Created by x1314aq on 18-2-25.
//

#include "neutron_transport.h"
#include "nuclide.h"
#include "RNG.h"


void
treat_implicit_capture_fixed(particle_status_t *par_status)
{
    nuc_xs_t *cur_nuc_xs;
    double wgt_survival;

    cur_nuc_xs = par_status->nuc_xs;
    par_status->wgt *= (1 - cur_nuc_xs->abs / cur_nuc_xs->tot);

    if(par_status->wgt > WGT_CUTOFF) return;

    wgt_survival = 2 * WGT_CUTOFF;
    if(get_rand() < par_status->wgt / wgt_survival)
        par_status->wgt = wgt_survival;
    else par_status->is_killed = true;
}