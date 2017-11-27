//
// Created by xaq on 11/15/17.
//

#include "neutron_transport.h"
#include "RNG.h"
#include "nuclide.h"
#include "map.h"
#include "acedata.h"


extern map *base_nucs;

int sample_col_type(particle_state_t *par_state){
    if(par_state->is_sab_col) return 0;

    nuclide_t *nuc = (nuclide_t *)map_get(base_nucs, par_state->nuc);
    while(1){
        if(get_rand() * (nuc->el + nuc->inel) - nuc->el <= ZERO)
            return 2;

        double sum = 0;
        double ksi = get_rand() * nuc->inel;
        int Loc = Get_loc_of_MTR(nuc) - 1;
        int MT_num = Get_non_el_mt_num_with_neu(nuc);
        for(int i = 0; i < MT_num; i++){
            int MT = (int)(nuc->XSS[Loc + i]);
            if(MT == 18 || MT == 19 || MT == 20 || MT == 21 || MT == 38)
                continue;
            double cs = get_nuc_mt_cs(nuc, MT, par_state->interp_N, par_state->interp_K);
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