//
// Created by xaq on 11/15/17.
//

#include "neutron_transport.h"
#include "material.h"
#include "RNG.h"
#include "map.h"
#include "acedata.h"


extern map *base_mats;
extern map *base_nucs;

void get_exit_state(particle_state_t *par_state){
    mat_t *mat = (mat_t *) map_get(base_mats, par_state->mat);
    nuclide_t *nuc = (nuclide_t *) map_get(base_nucs, (uint64_t) mat->nuc_id[par_state->nuc]);

    if(par_state->is_sab_col){
        treat_sab_colli_type(nuc, nuc->el, nuc->inel, par_state->erg, par_state->dir, &par_state->exit_erg,
                             par_state->exit_dir);
        /* 这里似乎应该是出射能量而不是原本的能量 */
        if(par_state->erg <= EG0_CUTOFF)
            par_state->is_killed = true;
        return;
    }

    int emiss_neu_num = abs(GetEmissNeuNum(nuc, par_state->collision_type));

    if(emiss_neu_num == 0){
        par_state->is_killed = true;
        return;
    } else if(emiss_neu_num > 100)
        emiss_neu_num = 1;

    par_state->wgt *= emiss_neu_num;

    get_ce_exit_state(par_state, par_state->collision_type);

    /* 这里似乎应该是出射能量而不是原本的能量 */
    if(par_state->erg <= EG0_CUTOFF)
        par_state->is_killed = true;
}