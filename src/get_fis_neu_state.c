//
// Created by xaq on 11/15/17.
//

#include "criticality.h"
#include "acedata.h"
#include "neutron_transport.h"


int
get_fis_neu_state(particle_status_t *par_status,
                  bank_t *cur_fis_bank,
                  RNG_t *RNG,
                  int fis_MT,
                  double fis_wgt,
                  double nu)
{
    nuclide_t *nuc = par_status->nuc;
    double erg = par_status->erg;
    int fis_neu_num = (int) (fis_wgt + get_rand(RNG));
    double nu_delayed = get_delayed_nu(nuc, erg);
    double beta = nu_delayed / nu;

    for(int i = 0; i < fis_neu_num; i++) {
        /* sample prompt/delayed fission neutrons */
        if(get_rand(RNG) < beta) {
            int NPCR = Get_NPCR(nuc);
            int Loc = Get_loc_of_BDD(nuc);
            double ksi = get_rand(RNG);
            double prob_sum = ZERO;
            int j = 1;
            for(j = 1; j < NPCR; j++) {
                int NR = (int) (nuc->XSS[Loc + 1]);
                int NE = (int) (nuc->XSS[Loc + 2 + 2 * NR]);
                double yield = get_erg_func_value(nuc, Loc + 1, erg);
                prob_sum += yield;

                if(ksi < prob_sum) break;
                Loc += 2 + 2 * NR + 2 * NE + 1;
            }

            j = MIN(j, NPCR);
            int LDAT;
            int law_type = get_law_type(nuc, -j, erg, RNG, &LDAT);
            double exit_erg;
            double exit_mu = TWO * get_rand(RNG) - ONE;
            react_by_laws(nuc, RNG, -1, law_type, LDAT, erg, &exit_erg, &exit_mu);

            double phi = TWO * PI * get_rand(RNG);
            par_status->exit_dir[0] = exit_mu;
            par_status->exit_dir[1] = sqrt(ONE - SQUARE(exit_mu)) * cos(phi);
            par_status->exit_dir[2] = sqrt(ONE - SQUARE(exit_mu)) * sin(phi);
            par_status->exit_erg = exit_erg;
        } else
            get_ce_exit_state(par_status, RNG, fis_MT, false);

        for(int j = 0; j < 3; j++) {
            cur_fis_bank->pos[j] = par_status->pos[j];
            cur_fis_bank->dir[j] = par_status->exit_dir[j];
        }
        cur_fis_bank->erg = par_status->exit_erg;
        cur_fis_bank++;
    }
    return fis_neu_num;
}