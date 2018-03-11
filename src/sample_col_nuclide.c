//
// Created by xaq on 11/15/17.
//

#include "neutron_transport.h"
#include "RNG.h"
#include "material.h"
#include "nuclide.h"


void
sample_col_nuclide(particle_status_t *par_status,
                   nuc_xs_t *nuc_xs,
                   RNG_t *RNG)
{
    mat_t *mat;
    nuclide_t *nuc, *sab_nuc;
    nuc_xs_t *cur_nuc_xs;
    double sample_cutoff = par_status->macro_tot_cs * get_rand(RNG);

    mat = par_status->mat;
    double sigt_sum2 = ZERO;
    for(int i = 0; i < mat->tot_nuc_num; i++) {
        nuc = mat->nucs[i];
        cur_nuc_xs = &nuc_xs[nuc->xs];
        sigt_sum2 += cur_nuc_xs->tot * mat->nuc_atom_den[i];
        if(sigt_sum2 >= sample_cutoff) {
            par_status->nuc = nuc;
            par_status->nuc_xs = cur_nuc_xs;
            break;
        }
    }

    par_status->sab_nuc = NULL;
    if(mat->sab_nuc) {
        sab_nuc = mat->sab_nuc;
        if(sab_nuc->zaid == nuc->zaid && par_status->erg < mat->sab_nuc_esa)
            par_status->sab_nuc = sab_nuc;
    }
}