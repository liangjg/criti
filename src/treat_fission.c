//
// Created by xaq on 11/15/17.
//

#include "neutron_transport.h"
#include "acedata.h"
#include "criticality.h"


static int fis_MT[5] = {18, 19, 20, 21, 38};

void
treat_fission(particle_status_t *par_status,
              pth_arg_t *arg)
{
    nuclide_t *nuc;
    nuc_xs_t *cur_nuc_xs;
    double fis_sub_cs[5];
    bank_t *cur_fis_bank;
    RNG_t *RNG;

    nuc = par_status->nuc;
    cur_nuc_xs = par_status->nuc_xs;
    cur_fis_bank = &arg->fis_bank[arg->bank_cnt];
    RNG = &arg->RNG;

    if(cur_nuc_xs->fis <= ZERO) return;

    if(nuc->LSIG[18] > 0)  /* 总裂变截面MT=18存在 */
        fis_sub_cs[0] = get_nuc_mt_cs(nuc, 18, par_status->interp_N0, par_status->interp_K0);
    else
        for(int i = 1; i < 5; i++)
            fis_sub_cs[i] = get_nuc_mt_cs(nuc, fis_MT[i], par_status->interp_N0, par_status->interp_K0);

    if(cur_nuc_xs->ptable) {
        double temp = nuc->fis_XSS[par_status->interp_N0]
                      + par_status->interp_K0
                        * (nuc->fis_XSS[par_status->interp_N0 + 1] - nuc->fis_XSS[par_status->interp_N0]);
        if(!EQ_ZERO(temp)) {
            double ff = cur_nuc_xs->fis / temp;
            for(int i = 0; i < 5; i++)
                fis_sub_cs[i] *= ff;
        }
    }

    arg->keff_wgt_sum[1] += par_status->wgt * cur_nuc_xs->nu * cur_nuc_xs->fis / cur_nuc_xs->tot;

    double fis_R;
    if(nuc->LSIG[18] > 0) {
        if(fis_sub_cs[0] > 0) {
            fis_R = par_status->wgt * cur_nuc_xs->nu * fis_sub_cs[0] / cur_nuc_xs->tot / arg->keff_final;
            arg->bank_cnt += get_fis_neu_state(par_status, cur_fis_bank, RNG, fis_MT[0], fis_R, cur_nuc_xs->nu);
        }
    } else {
        for(int i = 1; i < 5; i++) {
            if(fis_sub_cs[i] > 0) {
                fis_R = par_status->wgt * cur_nuc_xs->nu * fis_sub_cs[i] / cur_nuc_xs->tot / arg->keff_final;
                arg->bank_cnt += get_fis_neu_state(par_status, cur_fis_bank, RNG, fis_MT[i], fis_R, cur_nuc_xs->nu);
            }
        }
    }
}