//
// Created by x1314aq on 18-1-17.
//

#include "fixed_source.h"
#include "RNG.h"
#include "acedata.h"
#include "neutron_transport.h"


extern fixed_src_t base_fixed_src;

void get_fis_neu_state_fixed(particle_state_t *par_state){
    int fis_MT = par_state->collision_type;
    nuclide_t *nuc = par_state->nuc;
    double erg = par_state->erg;
    double nu_delayed = get_delayed_nu(nuc, erg);
    double beta = nu_delayed / nuc->nu;
    int fis_neu_num = (int) (nuc->nu + get_rand());

    for(int i = 0; i < fis_neu_num; i++){
        if(get_rand() < beta){
            int NPCR = Get_NPCR(nuc);
            int Loc = Get_loc_of_BDD(nuc);
            double ksi = get_rand();
            double prob_sum = ZERO;
            int j = 1;
            for(j = 1; j < NPCR; j++){
                int NR = (int) (nuc->XSS[Loc + 1]);
                int NE = (int) (nuc->XSS[Loc + 2 + 2 * NR]);
                double yield = get_erg_func_value(nuc, Loc + 1, erg);
                prob_sum += yield;

                if(ksi < prob_sum) break;
                Loc += 2 + 2 * NR + 2 * NE + 1;
            }

            j = MIN(j, NPCR);
            int LDAT;
            int law_type = get_law_type(nuc, -j, erg, &LDAT);
            double exit_erg;
            double exit_mu = TWO * get_rand() - ONE;
            react_by_laws(nuc, -1, law_type, LDAT, erg, &exit_erg, &exit_mu);

            double phi = TWO * PI * get_rand();
            par_state->exit_dir[0] = exit_mu;
            par_state->exit_dir[1] = sqrt(ONE - SQUARE(exit_mu)) * cos(phi);
            par_state->exit_dir[2] = sqrt(ONE - SQUARE(exit_mu)) * sin(phi);
            par_state->exit_erg = exit_erg;
        } else
            get_ce_exit_state(par_state, fis_MT, false);

        fixed_src_bank_t *fixed_src_bank;
        fixed_src_bank = &base_fixed_src.fixed_bank[base_fixed_src.fixed_bank_cnt++];

        for(int j = 0; j < 3; j++){
            fixed_src_bank->pos[j] = par_state->pos[j];
            fixed_src_bank->dir[j] = par_state->exit_dir[j];
        }
        fixed_src_bank->erg = par_state->exit_erg;
        fixed_src_bank->wgt = par_state->wgt;
    }
}