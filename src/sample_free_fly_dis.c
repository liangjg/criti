//
// Created by 叶鑫 on 2017/11/9.
//

#include "neutron_transport.h"
#include "RNG.h"
#include "material.h"
#include "nuclide.h"
#include "acedata.h"
#include "map.h"


extern map *base_mats;
extern map *base_nucs;
extern acedata_t base_acedata;

double sample_free_fly_dis(particle_state_t *par_state){
    mat_t *mat;
    nuclide_t *nuc, *sab_nuc;
    double nuc_atom_den;

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

    for(int i = 0; i < mat->tot_nuc_num; i++){
        nuc = (nuclide_t *) map_get(base_nucs, (uint64_t) mat->nuc_id[i]);
        sab_nuc = (nuclide_t *) map_get(base_nucs, (uint64_t) mat->sab_nuc_id);
        if(par_state->erg >= mat->sab_nuc_esa || nuc->zaid != sab_nuc->zaid) sab_nuc = NULL;
        nuc_atom_den = mat->nuc_atom_den[i];

        nuc->inter_pos = -1;
        get_nuc_tot_fis_cs(&base_acedata, nuc, sab_nuc, par_state->erg, par_state->cell_tmp);

        par_state->macro_tot_cs += nuc_atom_den * nuc->tot;
        if(GT_ZERO(nuc->fis))
            par_state->macro_nu_fis_cs += nuc_atom_den * nuc->fis * nuc->nu;
    }

    if(!GT_ZERO(par_state->macro_tot_cs)){
        par_state->macro_tot_cs = ZERO_ERG;
        par_state->macro_nu_fis_cs = ZERO;
    }
END:
    return -log(get_rand()) / par_state->macro_tot_cs;
}