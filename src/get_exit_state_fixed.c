//
// Created by 叶鑫 on 2018/3/26.
//

#include "neutron_transport.h"
#include "fixed_source.h"
#include "acedata.h"


void
get_exit_state_fixed(particle_status_t *par_status,
                     RNG_t *RNG,
                     bank_t *bank,
                     int *bank_cnt)
{
    nuclide_t *nuc, *sab_nuc;
    nuc_xs_t *cur_nuc_xs;
    bank_t *cur_fixed_bank;

    nuc = par_status->nuc;
    sab_nuc = par_status->sab_nuc;
    cur_nuc_xs = par_status->nuc_xs;
    cur_fixed_bank = &bank[*bank_cnt];

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
    } else if(emiss_neu_num == 100)
        emiss_neu_num = 1;

    /* special treatment for fixed source, fission reaction */
    if(emiss_neu_num == 19) {
        *bank_cnt += get_fis_neu_state_fixed(par_status, cur_fixed_bank, RNG, cur_nuc_xs->nu);
        par_status->is_killed = true;
        return;
    }

    /* ordinary scattering */
    par_status->wgt *= emiss_neu_num;

    get_ce_exit_state(par_status, RNG, par_status->collision_type, par_status->is_free_gas_col);

    /* 这里似乎应该是出射能量而不是原本的能量 */
    if(par_status->erg <= EG0_CUTOFF)
        par_status->is_killed = true;
}