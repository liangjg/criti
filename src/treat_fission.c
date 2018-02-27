//
// Created by xaq on 11/15/17.
//

#include "neutron_transport.h"
#include "acedata.h"
#include "criticality.h"


extern criti_t base_criti;

/* 从核LDM中的变量 */
extern double keff_final;
extern double keff_wgt_sum_slave[3];
extern nuc_cs_t *nuc_cs_slave;

static int fis_MT[5] = {18, 19, 20, 21, 38};

void treat_fission(particle_state_t *par_state){
    nuclide_t *nuc;
    nuc_cs_t *cur_nuc_cs;
    double fis_sub_cs[5];
    double fis_R;
    int i;

    nuc = par_state->nuc;
    cur_nuc_cs = &nuc_cs_slave[nuc->cs];

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
            get_fis_neu_state(par_state, fis_MT[0], fis_R);
        }
    } else{
        for(i = 1; i < 5; i++){
            if(fis_sub_cs[i] > 0){
                fis_R = par_state->wgt * cur_nuc_cs->nu * fis_sub_cs[i] / cur_nuc_cs->tot / keff_final;
                get_fis_neu_state(par_state, fis_MT[i], fis_R);
            }
        }
    }
}