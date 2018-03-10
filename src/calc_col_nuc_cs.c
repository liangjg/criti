//
// Created by xaq on 11/15/17.
//

#include "neutron_transport.h"
#include "global_fun.h"
#include "acedata.h"


extern acedata_t base_acedata;
extern nuc_xs_t *base_nuc_xs;

void
calc_col_nuc_cs(particle_status_t *par_status)
{
    if(par_status->sab_nuc)
        return;

    nuclide_t *nuc = par_status->nuc;
    nuc_xs_t *cur_nuc_xs = par_status->nuc_xs;
    par_status->interp_N0 = cur_nuc_xs->inter_pos;
    par_status->interp_K0 = cur_nuc_xs->inter_frac;

    if(par_status->cell_tmp > 1.0E-24 &&
        (par_status->erg < 400 * par_status->cell_tmp || nuc->atom_wgt <= 1.5)) {
        par_status->is_free_gas_col = true;
        if(nuc->ptable) return;
        treat_free_gas_model(par_status, nuc->atom_wgt);
        par_status->interp_N = get_intplt_pos_fr(nuc->XSS, par_status->erg_rel, 1, Get_erg_grid_num(nuc),
                                                &par_status->interp_K);
    } else {
        par_status->is_free_gas_col = false;
        par_status->erg_rel = par_status->erg;
        par_status->interp_N = par_status->interp_N0;
        par_status->interp_K = par_status->interp_K0;
    }

    get_nuc_abs_scatt_cs(&base_acedata, nuc, cur_nuc_xs, par_status);
}