//
// Created by xaq on 11/15/17.
//

#include "neutron_transport.h"
#include "acedata.h"


void
get_exit_state(particle_status_t *par_status)
{
    nuclide_t *nuc = par_status->nuc;
    nuclide_t *sab_nuc = par_status->sab_nuc;
    nuc_xs_t *cur_nuc_xs = par_status->nuc_xs;

    if(sab_nuc) {
        treat_sab_colli_type(sab_nuc, cur_nuc_xs->el, cur_nuc_xs->inel, par_status->erg, par_status->dir, &par_status->exit_erg,
                             par_status->exit_dir);
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

    get_ce_exit_state(par_status, par_status->collision_type, par_status->is_free_gas_col);

    /* 这里似乎应该是出射能量而不是原本的能量 */
    if(par_status->erg <= EG0_CUTOFF)
        par_status->is_killed = true;
}