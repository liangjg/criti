//
// Created by xaq on 11/15/17.
//

#include "neutron_transport.h"
#include "nuclide.h"
#include "particle_state.h"
#include "map.h"
#include "global_fun.h"
#include "acedata.h"

extern particle_state_t base_par_state;
extern map *base_nucs;
extern acedata_t base_acedata;

void calc_col_nuc_cs(){
    if(base_par_state.is_sab_col)
        return;

    nuclide_t *nuc = (nuclide_t *) map_get(base_nucs, base_par_state.nuc);
    base_par_state.interp_N0 = nuc->inter_pos;
    base_par_state.interp_K0 = nuc->inter_frac;

    if(base_par_state.cell_tmp > 1.0E-24 &&
       (base_par_state.erg < 400 * base_par_state.cell_tmp || nuc->atom_wgt <= 1.5)){
        base_par_state.is_free_gas_col = true;
        treat_free_gas_model(nuc->atom_wgt);
        get_intplt_pos_fr_double(nuc->XSS, base_par_state.erg_rel, 1, Get_erg_grid_num(nuc), &base_par_state.interp_N,
                                 &base_par_state.interp_K);
    } else{
        base_par_state.is_free_gas_col = false;
        base_par_state.erg_rel = base_par_state.erg;
        base_par_state.interp_N = base_par_state.interp_N0;
        base_par_state.interp_K = base_par_state.interp_K0;
    }

    get_nuc_abs_scatt_cs(&base_acedata, nuc, base_par_state.erg, base_par_state.interp_N0, base_par_state.interp_K0,
                         base_par_state.interp_N, base_par_state.interp_K);
};