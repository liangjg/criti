//
// Created by xaq on 11/15/17.
//

#include "neutron_transport.h"
#include "acedata.h"


static int fis_MT[5] = {18, 19, 20, 21, 38};

void treat_fission(particle_state_t *par_state, RNG_t *RNG_slave, fission_bank_t *fis_bank_slave, int *fis_bank_cnt,
                   double *keff_wgt_sum_slave, double keff_final){
    nuclide_t *nuc;
    nuc_cs_t *cur_nuc_cs;
    double fis_sub_cs[5];
    double fis_R;
    int i;

    nuc = par_state->nuc;
    cur_nuc_cs = par_state->nuc_cs;

    if(cur_nuc_cs->fis <= ZERO) return;

    if(nuc->LSIG[18] > 0)  /* 总裂变截面MT=18存在 */
        fis_sub_cs[0] = get_nuc_mt_cs(nuc, 18, par_state->interp_N0, par_state->interp_K0);
    else
        for(i = 1; i < 5; i++)
            fis_sub_cs[i] = get_nuc_mt_cs(nuc, fis_MT[i], par_state->interp_N0, par_state->interp_K0);

    Estimate_keff_abs(par_state->wgt, cur_nuc_cs->nu, cur_nuc_cs->fis, cur_nuc_cs->tot);

    if(nuc->LSIG[18] > 0){
        if(fis_sub_cs[0] > 0){
            fis_R = par_state->wgt * cur_nuc_cs->nu * fis_sub_cs[0] / cur_nuc_cs->tot / keff_final;
            get_fis_neu_state(par_state, RNG_slave, fis_bank_slave, fis_bank_cnt, fis_MT[0], fis_R);
        }
    } else{
        for(i = 1; i < 5; i++){
            if(fis_sub_cs[i] > 0){
                fis_R = par_state->wgt * cur_nuc_cs->nu * fis_sub_cs[i] / cur_nuc_cs->tot / keff_final;
                get_fis_neu_state(par_state, RNG_slave, fis_bank_slave, fis_bank_cnt, fis_MT[i], fis_R);
            }
        }
    }
}