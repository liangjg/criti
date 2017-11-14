//
// Created by xaq on 11/15/17.
//

#include "neutron_transport.h"
#include "particle_state.h"
#include "RNG.h"
#include "material.h"
#include "map.h"
#include "nuclide.h"

extern particle_state_t base_par_state;
extern map *base_mats;
extern map *base_nucs;

void sample_col_nuclide(){
    mat_t *mat;
    nuclide_t *nuc, *sab_nuc;
    double sample_cutoff = base_par_state.macro_tot_cs * get_rand();

    int mat_index = base_par_state.mat;
    mat = (mat_t *)map_get(base_mats, mat_index);
    double sigt_sum2 = ZERO;
    for(int i = 0; i < mat->tot_nuc_num; i++){
        nuc = (nuclide_t *)map_get(base_nucs, (uint64_t)mat->nuc_id[i]);
        sigt_sum2 += nuc->tot * mat->nuc_atom_den[i];
        if(sigt_sum2 >= sample_cutoff){
            base_par_state.nuc = i;
            break;
        }
    }

    if(mat->tot_sab_nuc_num){
        sab_nuc = (nuclide_t *)map_get(base_nucs, (uint64_t)mat->sab_nuc_id);
        if(sab_nuc->zaid == nuc->zaid && base_par_state.erg < mat->sab_nuc_esa)
            base_par_state.is_sab_col = true;
    }
    else base_par_state.is_sab_col = false;
}