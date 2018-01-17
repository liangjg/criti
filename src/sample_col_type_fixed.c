//
// Created by x1314aq on 18-1-17.
//

#include "neutron_transport.h"
#include "RNG.h"
#include "material.h"
#include "map.h"
#include "acedata.h"


extern map *base_nucs;
extern map *base_mats;

int sample_col_type_fixed(particle_state_t *par_state){
    if(par_state->is_sab_col) return 0;

    mat_t *mat = map_get(base_mats, par_state->mat);
    nuclide_t *nuc = map_get(base_nucs, (uint64_t) mat->nuc_id[par_state->nuc]);

    /* fission is treated as inelastic scattering */
    nuc->inel += nuc->fis;

    while(1){
        /* elastic scattering */
        if(get_rand() * (nuc->el + nuc->inel) - nuc->el <= ZERO)
            return 2;

        /* inelastic scattering */
        double sum = ZERO;
        double ksi = get_rand() * nuc->inel;
        int Loc = Get_loc_of_MTR(nuc) - 1;
        int MT_num = Get_non_el_mt_num_with_neu(nuc);
        double ff = ONE;

        for(int i = 1; i <= MT_num; i++){
            int MT = (int) (nuc->XSS[Loc + i]);
            double cs;
            if(MT == 18 || MT == 19 || MT == 20 || MT == 21 || MT == 38){
                cs = get_nuc_mt_cs(nuc, MT, par_state->interp_N0, par_state->interp_K0);
                cs *= ff;
            } else
                cs = get_nuc_mt_cs(nuc, MT, par_state->interp_N, par_state->interp_K);

            if(cs > ZERO){
                sum += cs;
                if(ksi <= sum) return MT;
            }
        }
        if(nuc->inel == sum) break;
        nuc->inel = sum;
    }

    printf("incorrect sampling of collision type. nuc = %s.\n", nuc->id);
    par_state->is_killed = true;
    return 0;
}