//
// Created by 叶鑫 on 2017/11/9.
//

#include "neutron_transport.h"
#include "RNG.h"
#include "material.h"
#include "nuclide.h"
#include "acedata.h"
#include "map.h"


/* 主核上的全局变量 */
extern map *base_mats;
extern map *base_nucs;
extern acedata_t base_acedata;

/* 从核LDM上的全局变量 */
extern RNG_t RNG_slave;

double sample_free_fly_dis(particle_state_t *par_state, bool erg_changed){
    mat_t *mat;
    nuclide_t *nuc, *sab_nuc;
    double nuc_atom_den;

    /* **********************************************************************
     * 如果粒子当前的材料以及材料温度与上次相比都没有发生变化，那么直接使用上一次的计算结果
     * 即可，不用重新计算。这种情况在大规模的重复几何结构中比较常见，采取这种方法，可以提高
     * 重复几何结构的计算的效率
     * **********************************************************************/
    if(!erg_changed && !par_state->mat_changed && !par_state->cell_tmp_changed)
        goto END;

    /* vacuum material */
    if(par_state->mat == 0){
        par_state->macro_tot_cs = ZERO_ERG;
        par_state->macro_nu_fis_cs = ZERO;
        goto END;
    }

    par_state->macro_tot_cs = ZERO;
    par_state->macro_nu_fis_cs = ZERO;

    mat = (mat_t *) map_get(base_mats, par_state->mat);

    /*************************************************
     * calculate total cross section of each nuclide,
     * and then, sum them up.
     *************************************************/

    if(erg_changed){
        map_entry *entry;
        map_iterator *nuc_iter = map_get_iter(base_nucs);

        while((entry = map_iter_next(nuc_iter))){
            nuc = entry->v.val;
            if(ISNUMBER(*nuc->id))    /* 非热化核素，特点是nuc->id以数字开头 */
                nuc->inter_pos = -1;
        }

        map_release_iter(nuc_iter);
    }

    for(int i = 0; i < mat->tot_nuc_num; i++){
        nuc = (nuclide_t *) map_get(base_nucs, (uint64_t) mat->nuc_id[i]);
        sab_nuc = (nuclide_t *) map_get(base_nucs, (uint64_t) mat->sab_nuc_id);

        if(nuc->inter_pos > 0)
            if(sab_nuc == NULL && !par_state->cell_tmp_changed)
                goto SUM_UP;

        if(par_state->erg >= mat->sab_nuc_esa || nuc->zaid != sab_nuc->zaid)
            sab_nuc = NULL;
        get_nuc_tot_fis_cs(&base_acedata, nuc, sab_nuc, par_state->erg, par_state->cell_tmp);

SUM_UP:
        nuc_atom_den = mat->nuc_atom_den[i];
        par_state->macro_tot_cs += nuc_atom_den * nuc->tot;
        if(GT_ZERO(nuc->fis))
            par_state->macro_nu_fis_cs += nuc_atom_den * nuc->fis * nuc->nu;
    }

    if(!GT_ZERO(par_state->macro_tot_cs)){
        par_state->macro_tot_cs = ZERO_ERG;
        par_state->macro_nu_fis_cs = ZERO;
    }
END:
    return -log(get_rand(&RNG_slave)) / par_state->macro_tot_cs;
}