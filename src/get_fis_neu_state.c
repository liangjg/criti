//
// Created by xaq on 11/15/17.
//

#include "criticality.h"
#include "particle_state.h"
#include "nuclide.h"
#include "map.h"
#include "RNG.h"
#include "acedata.h"
#include "neutron_transport.h"

extern particle_state_t base_par_state;
extern map *base_nucs;
extern criti_t base_criti;

void get_fis_neu_state(int fis_MT, double fis_wgt){
    nuclide_t *nuc = (nuclide_t *) map_get(base_nucs, base_par_state.nuc);
    double erg = base_par_state.erg;
    int fis_neu_num = (int) (fis_wgt + get_rand());
    double nu_delayed = get_delayed_nu(nuc, erg);
    double beta = nu_delayed / nuc->nu;

    for(int i = 0; i < fis_neu_num; i++){
        /* sample prompt/delayed fission neutrons */
        if(get_rand() < beta){
            int NPCR = GetNPCR(nuc);
            int Loc = GetLocOfBDD(nuc);
            double ksi = get_rand();
            double prob_sum = ZERO;
            int j = 1;
            for(j = 1; j < NPCR; j++){
                int NR = (int)(nuc->XSS[Loc + 1]);
                int NE = (int)(nuc->XSS[Loc + 2 + 2 * NR]);
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
            base_par_state.exit_dir[0] = exit_mu;
            base_par_state.exit_dir[1] = sqrt(ONE - SQUARE(exit_mu)) * cos(phi);
            base_par_state.exit_dir[2] = sqrt(ONE - SQUARE(exit_mu)) * sin(phi);
            base_par_state.exit_erg = exit_erg;
        } else
            get_ce_exit_state(fis_MT);

        fission_bank_t fission_bank;
        for(int j = 0; j < 3; j++){
            fission_bank.pos[j] = base_par_state.pos[j];
            fission_bank.dir[j] = base_par_state.exit_dir[j];
        }
        fission_bank.erg = base_par_state.exit_erg;
        vector_push_back(&base_criti.fission_bank, &fission_bank);
        base_criti.fission_bank_cnt++;
    }
}