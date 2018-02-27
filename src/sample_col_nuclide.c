//
// Created by xaq on 11/15/17.
//

#include "neutron_transport.h"
#include "RNG.h"
#include "material.h"
#include "nuclide.h"


/* 从核LDM上的全局变量 */
extern RNG_t RNG_slave;
extern nuc_cs_t *nuc_cs_slave;

void sample_col_nuclide(particle_state_t *par_state){
    mat_t *mat;
    nuclide_t *nuc, *sab_nuc;
    nuc_cs_t *cur_nuc_cs;
    double sigt_sum2;
    double sample_cutoff;
    int i;

    sample_cutoff = par_state->macro_tot_cs * get_rand_slave(&RNG_slave);
    mat = par_state->mat;
    sigt_sum2 = ZERO;
    for(i = 0; i < mat->tot_nuc_num; i++){
        nuc = mat->nucs[i];
        cur_nuc_cs = &nuc_cs_slave[nuc->cs];
        sigt_sum2 += cur_nuc_cs->tot * mat->nuc_atom_den[i];
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