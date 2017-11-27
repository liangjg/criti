//
// Created by xaq on 11/15/17.
//

#include "neutron_transport.h"
#include "nuclide.h"
#include "map.h"
#include "RNG.h"


extern map *base_nucs;

void treat_implicit_capture(particle_state_t *par_state){
    nuclide_t *nuc;
    double wgt_survival;

    nuc = (nuclide_t *) map_get(base_nucs, par_state->nuc);
    par_state->wgt *= (1 - (nuc->abs + nuc->fis) / nuc->tot);

    wgt_survival = 2 * WGT_CUTOFF;
    if(get_rand() < par_state->wgt / wgt_survival)
        par_state->wgt = wgt_survival;
    else par_state->is_killed = true;
}