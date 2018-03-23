//
// Created by xaq on 11/15/17.
//

#include "neutron_transport.h"
#include "material.h"


void
sample_col_nuclide(particle_status_t *par_status,
                   nuc_xs_t *nuc_cs_slave,
                   RNG_t *RNG_slave)
{
    mat_t *mat;
    nuclide_t *nuc, *sab_nuc;
    nuc_xs_t *cur_nuc_cs;
    double sigt_sum2;
    double sample_cutoff;
    int i;

    sample_cutoff = par_status->macro_tot_cs * get_rand_slave(RNG_slave);
    mat = par_status->mat;
    sigt_sum2 = ZERO;
    for(i = 0; i < mat->tot_nuc_num; i++) {
        nuc = mat->nucs[i];
        cur_nuc_cs = &nuc_cs_slave[nuc->xs];
        sigt_sum2 += cur_nuc_cs->tot * mat->nuc_atom_den[i];
        if(sigt_sum2 >= sample_cutoff) {
            par_status->nuc = nuc;
            par_status->nuc_xs = cur_nuc_cs;
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