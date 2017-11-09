//
// Created by 叶鑫 on 2017/11/9.
//

#include "neutron_transport.h"
#include "RNG.h"
#include "particle_state.h"
#include "cell.h"
#include "material.h"
//#include "acedata.h"
#include "nuclide.h"

extern particle_state_t base_par_state;

extern map *base_cells;

extern map *base_mats;

//extern acedata_t base_acedata;

double sample_free_fly_dis(bool is_erg_changed){
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

    /* current material */
    mat_t *mat = (mat_t *) map_get(base_mats, base_par_state.mat);

    /* current cell */
    cell_t *cell = (cell_t *) map_get(base_cells, base_par_state.cell);

//    for(int i = 0; i < mat->tot_nuc_num; i++)
//        base_acedata.nucs[i].inter_pos = -1;
    /* TODO: use energy bin map to accelerate binary search */

    /* calculate total cross section of each nuclide */
    for(int i = 0; i < mat->tot_nuc_num; i++){
        int nuclide = i;
        int sab_nuclide = i;
    }

END:
    return -log(get_rand()) / base_par_state.macro_tot_xs;
}