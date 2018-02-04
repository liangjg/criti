//
// Created by xaq on 11/15/17.
//

#include "neutron_transport.h"
#include "RNG.h"
#include "material.h"
#include "nuclide.h"


void sample_col_nuclide(particle_state_t *par_state){
    mat_t *mat;
    nuclide_t *nuc, *sab_nuc;
    double sample_cutoff = par_state->macro_tot_cs * get_rand();

    mat = par_state->mat;
    double sigt_sum2 = ZERO;
    for(int i = 0; i < mat->tot_nuc_num; i++){
        nuc = mat->nucs[i];
        sigt_sum2 += nuc->tot * mat->nuc_atom_den[i];
        if(sigt_sum2 >= sample_cutoff){
            par_state->nuc = nuc;
            break;
        }
    }

    par_state->sab_nuc = NULL;
    if(mat->sab_nuc){
        sab_nuc = mat->sab_nuc;
        if(sab_nuc->zaid == nuc->zaid && par_state->erg < mat->sab_nuc_esa)
            par_state->sab_nuc = sab_nuc;
    }
}