//
// Created by xaq on 11/15/17.
//

#include "neutron_transport.h"
#include "nuclide.h"
#include "RNG.h"


void treat_implicit_capture(particle_state_t *par_state){
    nuclide_t *nuc;
    double wgt_survival;

    nuc = par_state->nuc;
    par_state->wgt *= (ONE - (nuc->abs + nuc->fis) / nuc->tot);

    if(par_state->wgt > WGT_CUTOFF) return;

    wgt_survival = TWO * WGT_CUTOFF;
    if(get_rand() < par_state->wgt / wgt_survival)
        par_state->wgt = wgt_survival;
    else par_state->is_killed = true;
}