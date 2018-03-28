//
// Created by xaq on 11/15/17.
//

#include "neutron_transport.h"
#include "acedata.h"


void
get_exit_state(particle_status_t *par_status,
               RNG_t *RNG)
{
    nuclide_t *nuc;
    nuclide_t *sab_nuc;

    nuc = par_status->nuc;
    sab_nuc = par_status->sab_nuc;

    if(sab_nuc) {
        treat_sab_colli_type(sab_nuc, par_status, RNG);
        /* 这里似乎应该是出射能量而不是原本的能量 */
        if(par_status->erg <= EG0_CUTOFF)
            par_status->is_killed = true;
        return;
    }

    int emiss_neu_num = abs(Get_emiss_neu_num(nuc, par_status->collision_type));

    if(emiss_neu_num == 0) {
        par_status->is_killed = true;
        return;
    } else if(emiss_neu_num > 100)
        emiss_neu_num = 1;

    par_status->wgt *= emiss_neu_num;

    get_ce_exit_state(par_status, RNG, par_status->collision_type, par_status->is_free_gas_col);

    /* 这里似乎应该是出射能量而不是原本的能量 */
    if(par_status->erg <= EG0_CUTOFF)
        par_status->is_killed = true;
}