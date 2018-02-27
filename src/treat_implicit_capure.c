//
// Created by xaq on 11/15/17.
//

#include "neutron_transport.h"
#include "nuclide.h"
#include "RNG.h"


void treat_implicit_capture(particle_state_t *par_state, RNG_t *RNG_slave){
    double wgt_survival;
    nuc_cs_t *cur_nuc_cs;

    cur_nuc_cs = par_state->nuc_cs;
    par_state->wgt *= (1 - (cur_nuc_cs->abs + cur_nuc_cs->fis) / cur_nuc_cs->tot);

    if(par_state->wgt > WGT_CUTOFF) return;

    wgt_survival = 2 * WGT_CUTOFF;
    if(get_rand_slave(RNG_slave) < par_state->wgt / wgt_survival)
        par_state->wgt = wgt_survival;
    else par_state->is_killed = true;
}