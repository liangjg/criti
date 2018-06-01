//
// Created by 叶鑫 on 2017/11/9.
//

#include "neutron_transport.h"
#include "RNG.h"
#include "material.h"
#include "acedata.h"


extern acedata_t base_acedata;
extern int base_tot_nucs;

double
sample_free_fly_dis(particle_status_t *par_status,
                    nuc_xs_t *nuc_xs,
                    RNG_t *RNG,
                    bool erg_changed)
{
    mat_t *mat;
    nuclide_t *nuc, *sab_nuc;
    nuc_xs_t *cur_nuc_xs;
    double nuc_atom_den;

    /***********************************************************************
     * 如果粒子当前的材料以及材料温度与上次相比都没有发生变化，那么直接使用上一次的计算结果
     * 即可，不用重新计算。这种情况在大规模的重复几何结构中比较常见，采取这种方法，可以提高
     * 重复几何结构的计算的效率
     ***********************************************************************/
    if(!erg_changed && !par_status->mat_changed && !par_status->cell_tmp_changed)
        goto END;

    /* vacuum material */
    if(par_status->mat == NULL) {
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
    if(erg_changed)
        for(int i = 0; i < base_tot_nucs; ++i)
            nuc_xs[i].inter_pos = -1;

    for(int i = 0; i < mat->tot_nuc_num; i++) {
        nuc = mat->nucs[i];
        sab_nuc = mat->sab_nuc;
//        nuc_atom_den = mat->nuc_atom_den[i];
        cur_nuc_xs = &nuc_xs[nuc->xs];

        if(cur_nuc_xs->inter_pos > 0 && !sab_nuc && !par_status->cell_tmp_changed)
            continue;

        if(sab_nuc && (sab_nuc->zaid != nuc->zaid || par_status->erg >= mat->sab_nuc_esa))
            sab_nuc = NULL;

        get_nuc_tot_fis_cs(&base_acedata, nuc, sab_nuc, cur_nuc_xs, par_status->erg, par_status->cell_tmp, RNG);

//        par_status->macro_tot_cs += nuc_atom_den * cur_nuc_xs->tot;
//        if(GT_ZERO(cur_nuc_xs->fis))
//            par_status->macro_nu_fis_cs += nuc_atom_den * cur_nuc_xs->fis * cur_nuc_xs->nu;
    }

    for(int i = 0; i < mat->tot_nuc_num; i++) {
        nuc = mat->nucs[i];
        nuc_atom_den = mat->nuc_atom_den[i];
        cur_nuc_xs = &nuc_xs[nuc->xs];

        par_status->macro_tot_cs += nuc_atom_den * cur_nuc_xs->tot;
        if(GT_ZERO(cur_nuc_xs->fis))
            par_status->macro_nu_fis_cs += nuc_atom_den * cur_nuc_xs->fis * cur_nuc_xs->nu;
    }

    if(!GT_ZERO(par_status->macro_tot_cs)) {
        par_status->macro_tot_cs = ZERO_ERG;
        par_status->macro_nu_fis_cs = ZERO;
    }
END:
    return -log(get_rand(RNG)) / par_status->macro_tot_cs;
}
