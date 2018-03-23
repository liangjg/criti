//
// Created by xaq on 11/15/17.
//

#include "neutron_transport.h"
#include "nuclide.h"
#include "RNG.h"


void
treat_implicit_capture(particle_status_t *par_status,
                       RNG_t *RNG_slave)
{
    double wgt_survival;
    nuc_xs_t *cur_nuc_cs;

    cur_nuc_cs = par_status->nuc_xs;
    par_status->wgt *= (1 - (cur_nuc_cs->abs + cur_nuc_cs->fis) / cur_nuc_cs->tot);

    if(par_status->wgt > WGT_CUTOFF) return;

    wgt_survival = 2 * WGT_CUTOFF;
    if(get_rand_slave(RNG_slave) < par_status->wgt / wgt_survival)
        par_status->wgt = wgt_survival;
    else par_status->is_killed = true;
}