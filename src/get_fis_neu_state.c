//
// Created by xaq on 11/15/17.
//

#include "criticality.h"
#include "material.h"
#include "map.h"
#include "RNG.h"
#include "acedata.h"
#include "neutron_transport.h"


/* 全局变量，所有从核都相同 */
extern map *base_nucs;
extern map *base_mats;

/* 从核LDM中的变量 */
extern fission_bank_t fis_bank_slave[600];
extern int fis_bank_cnt;

void get_fis_neu_state(particle_state_t *par_state, int fis_MT, double fis_wgt){
    mat_t *mat = (mat_t *) map_get(base_mats, par_state->mat);
    nuclide_t *nuc = (nuclide_t *) map_get(base_nucs, (uint64_t) mat->nuc_id[par_state->nuc]);
    double erg = par_state->erg;
    int fis_neu_num = (int) (fis_wgt + get_rand());
    double nu_delayed = get_delayed_nu(nuc, erg);
    double beta = nu_delayed / nuc->nu;
    int fis_bank_cnt_local = fis_bank_cnt;

    for(int i = 0; i < fis_neu_num; i++){
        /* sample prompt/delayed fission neutrons */
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

        for(int j = 0; j < 3; j++){
            fis_bank_slave[fis_bank_cnt_local].pos[j] = par_state->pos[j];
            fis_bank_slave[fis_bank_cnt_local].dir[j] = par_state->exit_dir[j];
        }
        fis_bank_slave[fis_bank_cnt_local].erg = par_state->exit_erg;
        fis_bank_cnt_local++;
    }
    fis_bank_cnt = fis_bank_cnt_local;
}