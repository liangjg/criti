//
// Created by xaq on 11/15/17.
//

#include "neutron_transport.h"
#include "acedata.h"


void get_exit_state(particle_state_t *par_state){
    nuclide_t *nuc = par_state->nuc;
    nuclide_t *sab_nuc = par_state->sab_nuc;

    if(sab_nuc){
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
    } else if(emiss_neu_num > 100)
        emiss_neu_num = 1;

    par_state->wgt *= emiss_neu_num;

    get_ce_exit_state(par_state, par_state->collision_type, par_state->is_free_gas_col);

    /* 这里似乎应该是出射能量而不是原本的能量 */
    if(par_state->erg <= EG0_CUTOFF)
        par_state->is_killed = true;
}