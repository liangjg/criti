//
// Created by 叶鑫 on 2018/3/26.
//

#include "neutron_transport.h"
#include "acedata.h"


int
sample_col_type_fixed(particle_status_t *par_status,
                      RNG_t *RNG)
{
    if(par_status->sab_nuc) return 0;

    nuclide_t *nuc = par_status->nuc;
    nuc_xs_t *cur_nuc_xs = par_status->nuc_xs;

    /* fission is treated as inelastic scattering */
    cur_nuc_xs->inel += cur_nuc_xs->fis;

    while(1){
        /* elastic scattering */
        if(get_rand_slave(RNG) * (cur_nuc_xs->el + cur_nuc_xs->inel) - cur_nuc_xs->el <= ZERO)
            return 2;

        /* inelastic scattering */
        double sum = ZERO;
        double ksi = get_rand_slave(RNG) * cur_nuc_xs->inel;
        int Loc = Get_loc_of_MTR(nuc) - 1;
        int MT_num = Get_non_el_mt_num_with_neu(nuc);
        double ff = ONE;

        if(cur_nuc_xs->ptable){
            double cf = nuc->fis_XSS[par_status->interp_N0] + par_status->interp_K0 * (nuc->fis_XSS[par_status->interp_N0 + 1] - nuc->fis_XSS[par_status->interp_N0]);
            if(!EQ_ZERO(cf))
                ff = cur_nuc_xs->fis / cf;
        }

        for(int i = 1; i <= MT_num; i++){
            int MT = (int) (nuc->XSS[Loc + i]);
            double cs;
            if(MT == 18 || MT == 19 || MT == 20 || MT == 21 || MT == 38){
                cs = get_nuc_mt_cs(nuc, MT, par_status->interp_N0, par_status->interp_K0);
                cs *= ff;
            } else
                cs = get_nuc_mt_cs(nuc, MT, par_status->interp_N, par_status->interp_K);

            if(cs > ZERO){
                sum += cs;
                if(ksi <= sum) return MT;
            }
        }
        if(cur_nuc_xs->inel == sum) break;
        cur_nuc_xs->inel = sum;
    }

    printf("incorrect sampling of collision type. nuc = %s.\n", nuc->id);
    par_status->is_killed = true;
    return 0;
}