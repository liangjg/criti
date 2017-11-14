//
// Created by 叶鑫 on 2017/11/9.
//

#include "neutron_transport.h"
#include "RNG.h"
#include "particle_state.h"
#include "cell.h"
#include "material.h"
#include "nuclide.h"
#include "acedata.h"

extern particle_state_t base_par_state;
extern map *base_cells;
extern map *base_mats;
extern map *base_nucs;

double sample_free_fly_dis(bool is_erg_changed){
    mat_t *mat;
    cell_t *cell;
    nuclide_t *nuc, *sab_nuc;

    if(!is_erg_changed)
        goto END;

    /* vacuum material */
    if(base_par_state.mat == 0){
        base_par_state.macro_tot_xs = ZERO_ERG;
        base_par_state.macro_mu_fis_xs = ZERO;
        goto END;
    }

    base_par_state.macro_tot_xs = ZERO;
    base_par_state.macro_mu_fis_xs = ZERO;

    /* current material and cell */
    mat = (mat_t *) map_get(base_mats, base_par_state.mat);
    cell = (cell_t *) map_get(base_cells, base_par_state.cell);
    base_par_state.cell_tmp = cell->tmp;

    /* calculate total cross section of each nuclide */
    for(int i = 0; i < mat->tot_nuc_num; i++){
        nuc = (nuclide_t *) map_get(base_nucs, (uint64_t) mat->nuc_id[i]);
        sab_nuc = (nuclide_t *) map_get(base_nucs, (uint64_t) mat->sab_nuc_id);

        get_nuc_tot_fis_cs(NULL, nuc, sab_nuc, base_par_state.erg, base_par_state.cell_tmp);
    }



END:
    return -log(get_rand()) / base_par_state.macro_tot_xs;
}