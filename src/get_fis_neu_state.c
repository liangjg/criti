//
// Created by xaq on 11/15/17.
//

#include "criticality.h"
#include "acedata.h"
#include "neutron_transport.h"


void
get_fis_neu_state(particle_status_t *par_status,
                  RNG_t *RNG_slave,
                  fission_bank_t *fis_bank_slave,
                  int *fis_bank_cnt,
                  int fis_MT,
                  double fis_wgt)
{
    int i, j;
    nuclide_t *nuc = par_status->nuc;
    nuc_xs_t *cur_nuc_cs = par_status->nuc_xs;
    double erg = par_status->erg;
    int fis_neu_num = (int) (fis_wgt + get_rand_slave(RNG_slave));
    double nu_delayed = get_delayed_nu(nuc, erg);
    double beta = nu_delayed / cur_nuc_cs->nu;
    int fis_bank_cnt_local = *fis_bank_cnt;

    for(i = 0; i < fis_neu_num; i++) {
        /* sample prompt/delayed fission neutrons */
        if(get_rand_slave(RNG_slave) < beta) {
            int NPCR = Get_NPCR(nuc);
            int Loc = Get_loc_of_BDD(nuc);
            double ksi = get_rand_slave(RNG_slave);
            double prob_sum = ZERO;
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
            int law_type = get_law_type(nuc, RNG_slave, -j, erg, &LDAT);
            double exit_erg;
            double exit_mu = TWO * get_rand_slave(RNG_slave) - ONE;
            react_by_laws(nuc, RNG_slave, -1, law_type, LDAT, erg, &exit_erg, &exit_mu);

            double phi = TWO * PI * get_rand_slave(RNG_slave);
            par_status->exit_dir[0] = exit_mu;
            par_status->exit_dir[1] = sqrt(ONE - SQUARE(exit_mu)) * cos(phi);
            par_status->exit_dir[2] = sqrt(ONE - SQUARE(exit_mu)) * sin(phi);
            par_status->exit_erg = exit_erg;
        } else
            get_ce_exit_state(par_status, RNG_slave, fis_MT, false);

        for(j = 0; j < 3; j++) {
            fis_bank_slave[fis_bank_cnt_local].pos[j] = par_status->pos[j];
            fis_bank_slave[fis_bank_cnt_local].dir[j] = par_status->exit_dir[j];
        }
        fis_bank_slave[fis_bank_cnt_local].erg = par_status->exit_erg;
        fis_bank_cnt_local++;
    }
    *fis_bank_cnt = fis_bank_cnt_local;
}