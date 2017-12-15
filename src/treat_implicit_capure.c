//
// Created by xaq on 11/15/17.
//

#include "neutron_transport.h"
#include "nuclide.h"
#include "material.h"
#include "map.h"
#include "RNG.h"


extern map *base_nucs;
extern map *base_mats;

void treat_implicit_capture(particle_state_t *par_state){
    mat_t *mat;
    nuclide_t *nuc;
    double wgt_survival;

    mat = (mat_t *) map_get(base_mats, par_state->mat);
    nuc = (nuclide_t *) map_get(base_nucs, (uint64_t) mat->nuc_id[par_state->nuc]);
    par_state->wgt *= (1 - (nuc->abs + nuc->fis) / nuc->tot);

    if(par_state->wgt > WGT_CUTOFF) return;

    wgt_survival = 2 * WGT_CUTOFF;
    if(get_rand() < par_state->wgt / wgt_survival)
        par_state->wgt = wgt_survival;
    else par_state->is_killed = true;
}