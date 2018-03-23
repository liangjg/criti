//
// Created by xaq on 11/15/17.
//

#include "neutron_transport.h"
#include "acedata.h"


int
sample_col_type(particle_status_t *par_status,
                RNG_t *RNG_slave)
{
    if(par_status->sab_nuc) return 0;

    int i;
    nuclide_t *nuc = par_status->nuc;
    nuc_xs_t *cur_nuc_cs = par_status->nuc_xs;
    while(1) {
        if(get_rand_slave(RNG_slave) * (cur_nuc_cs->el + cur_nuc_cs->inel) - cur_nuc_cs->el <= ZERO)
            return 2;

        double sum = ZERO;
        double ksi = get_rand_slave(RNG_slave) * cur_nuc_cs->inel;
        int Loc = Get_loc_of_MTR(nuc) - 1;
        int MT_num = Get_non_el_mt_num_with_neu(nuc);
        for(i = 1; i <= MT_num; i++) {
            int MT = (int) (nuc->XSS[Loc + i]);
            if(MT == 18 || MT == 19 || MT == 20 || MT == 21 || MT == 38)
                continue;
            double cs = get_nuc_mt_cs(nuc, MT, par_status->interp_N, par_status->interp_K);
            if(cs > ZERO) {
                sum += cs;
                if(ksi <= sum) return MT;
            }
        }
        if(cur_nuc_cs->inel == sum) break;
        cur_nuc_cs->inel = sum;
    }

    printf("incorrect sampling of collision type. nuc = %s.\n", nuc->id);
    par_status->is_killed = true;
    return 0;
}