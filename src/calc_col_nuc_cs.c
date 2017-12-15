//
// Created by xaq on 11/15/17.
//

#include "neutron_transport.h"
#include "material.h"
#include "map.h"
#include "global_fun.h"
#include "acedata.h"


extern map *base_nucs;
extern map *base_mats;
extern acedata_t base_acedata;

void calc_col_nuc_cs(particle_state_t *par_state){
    if(par_state->is_sab_col)
        return;

    mat_t *mat = (mat_t *) map_get(base_mats, par_state->mat);
    nuclide_t *nuc = (nuclide_t *) map_get(base_nucs, (uint64_t) mat->nuc_id[par_state->nuc]);
    par_state->interp_N0 = nuc->inter_pos;
    par_state->interp_K0 = nuc->inter_frac;

    if(par_state->cell_tmp > 1.0E-24 &&
       (par_state->erg < 400 * par_state->cell_tmp || nuc->atom_wgt <= 1.5)){
        par_state->is_free_gas_col = true;
        treat_free_gas_model(par_state, nuc->atom_wgt);
        get_intplt_pos_fr_double(nuc->XSS, par_state->erg_rel, 1, Get_erg_grid_num(nuc), &par_state->interp_N,
                                 &par_state->interp_K);
    } else{
        par_state->is_free_gas_col = false;
        par_state->erg_rel = par_state->erg;
        par_state->interp_N = par_state->interp_N0;
        par_state->interp_K = par_state->interp_K0;
    }

    get_nuc_abs_scatt_cs(&base_acedata, nuc, par_state->erg, par_state->interp_N0, par_state->interp_K0,
                         par_state->interp_N, par_state->interp_K);
}