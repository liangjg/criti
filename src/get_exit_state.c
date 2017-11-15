//
// Created by xaq on 11/15/17.
//

#include "neutron_transport.h"
#include "particle_state.h"
#include "nuclide.h"
#include "RNG.h"
#include "map.h"
#include "acedata.h"

extern map *base_nucs;
extern particle_state_t base_par_state;

void get_exit_state(){
    nuclide_t *nuc = (nuclide_t *) map_get(base_nucs, base_par_state.nuc);

    if(base_par_state.is_sab_col){
        treat_sab_colli_type(nuc, nuc->el, nuc->inel, base_par_state.erg, base_par_state.dir, &base_par_state.exit_erg,
                             base_par_state.exit_dir);
        /* 这里似乎应该是出射能量而不是原本的能量 */
        if(base_par_state.erg <= EG0_CUTOFF)
            base_par_state.is_killed = true;
        return;
    }

    int emiss_neu_num = abs(GetEmissNeuNum(nuc, base_par_state.collision_type));

    if(emiss_neu_num == 0){
        base_par_state.is_killed = true;
        return;
    }
    else if(emiss_neu_num > 100)
        emiss_neu_num = 1;

    base_par_state.wgt *= emiss_neu_num;

    get_ce_exit_state(base_par_state.collision_type);

    /* 这里似乎应该是出射能量而不是原本的能量 */
    if(base_par_state.erg <= EG0_CUTOFF)
        base_par_state.is_killed = true;
}