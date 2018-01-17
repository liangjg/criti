//
// Created by x1314aq on 18-1-17.
//

#include "neutron_transport.h"
#include "fixed_source.h"
#include "map.h"
#include "material.h"
#include "acedata.h"


extern map *base_mats;
extern map *base_nucs;

void get_exit_state_fixed(particle_state_t *par_state){
    mat_t *mat = map_get(base_mats, par_state->mat);
    nuclide_t *nuc = map_get(base_nucs, (uint64_t) mat->nuc_id[par_state->nuc]);
    nuclide_t *sab_nuc = map_get(base_nucs, (uint64_t) mat->sab_nuc_id);

    if(par_state->is_sab_col){
        treat_sab_colli_type(sab_nuc, nuc->el, nuc->inel, par_state->erg, par_state->dir, &par_state->exit_erg,
                             par_state->exit_dir);
        /* 这里似乎应该是出射能量而不是原本的能量 */
        if(par_state->erg <= EG0_CUTOFF)
            par_state->is_killed = true;
        return;
    }

    int emiss_neu_num = abs(Get_emiss_neu_num(nuc, par_state->collision_type));

    if(emiss_neu_num == 0){
        par_state->is_killed = true;
        return;
    } else if(emiss_neu_num == 100)
        emiss_neu_num = 1;

    /* special treatment for fixed source, fission reaction */
    if(emiss_neu_num == 19){
        get_fis_neu_state_fixed(par_state);
        par_state->is_killed = true;
        return;
    }

    /* ordinary scattering */
    par_state->wgt *= emiss_neu_num;

    get_ce_exit_state(par_state, par_state->collision_type, par_state->is_free_gas_col);

    /* 这里似乎应该是出射能量而不是原本的能量 */
    if(par_state->erg <= EG0_CUTOFF)
        par_state->is_killed = true;
}