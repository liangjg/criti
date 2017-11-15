//
// Created by xaq on 11/15/17.
//

#include "neutron_transport.h"
#include "particle_state.h"
#include "nuclide.h"
#include "map.h"
#include "RNG.h"

extern particle_state_t base_par_state;
extern map *base_nucs;

void treat_implicit_capture(){
    nuclide_t *nuc;
    double wgt_survival;

    nuc = (nuclide_t *) map_get(base_nucs, base_par_state.nuc);
    base_par_state.wgt *= (1 - (nuc->abs + nuc->fis) / nuc->tot);

    wgt_survival = 2 * WGT_CUTOFF;
    if(get_rand() < base_par_state.wgt / wgt_survival)
        base_par_state.wgt = wgt_survival;
    else base_par_state.is_killed = true;
}