//
// Created by 叶鑫 on 2017/11/9.
//

#include "neutron_transport.h"
#include "material.h"
#include "acedata.h"


extern acedata_t base_acedata;

double
sample_free_fly_dis(particle_status_t *par_status,
                    nuc_xs_t *nuc_cs_slave,
                    RNG_t *RNG_slave,
                    bool erg_changed)
{
    mat_t *mat;
    nuclide_t *nuc, *sab_nuc;
    double nuc_atom_den;
    nuc_xs_t *cur_nuc_cs;
    int i;

    /***********************************************************************
     * 如果粒子当前的材料以及材料温度与上次相比都没有发生变化，那么直接使用上一次的计算结果
     * 即可，不用重新计算。这种情况在大规模的重复几何结构中比较常见，采取这种方法，可以提高
     * 重复几何结构的计算的效率
     ***********************************************************************/
    if(!erg_changed && !par_status->mat_changed && !par_status->cell_tmp_changed)
        goto END;

    /* vacuum material */
    if(!par_status->mat) {
        par_status->macro_tot_cs = ZERO_ERG;
        par_status->macro_nu_fis_cs = ZERO;
        goto END;
    }

    par_status->macro_tot_cs = ZERO;
    par_status->macro_nu_fis_cs = ZERO;

    mat = par_status->mat;

    /*************************************************
     * calculate total cross section of each nuclide,
     * and then, sum them up.
     *************************************************/

    for(i = 0; i < mat->tot_nuc_num; i++) {
        nuc = mat->nucs[i];
        cur_nuc_cs = &nuc_cs_slave[nuc->xs];
        sab_nuc = mat->sab_nuc;
        nuc_atom_den = mat->nuc_atom_den[i];

        if(sab_nuc && (sab_nuc->zaid != nuc->zaid || par_status->erg >= mat->sab_nuc_esa))
            sab_nuc = NULL;

        get_nuc_tot_fis_cs(&base_acedata, nuc, sab_nuc, cur_nuc_cs, par_status->erg, par_status->cell_tmp);

        par_status->macro_tot_cs += nuc_atom_den * cur_nuc_cs->tot;
        if(GT_ZERO(cur_nuc_cs->fis))
            par_status->macro_nu_fis_cs += nuc_atom_den * cur_nuc_cs->fis * cur_nuc_cs->nu;
    }

    if(!GT_ZERO(par_status->macro_tot_cs)) {
        par_status->macro_tot_cs = ZERO_ERG;
        par_status->macro_nu_fis_cs = ZERO;
    }
END:
    return -log(get_rand_slave(RNG_slave)) / par_status->macro_tot_cs;
}