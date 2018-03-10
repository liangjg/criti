//
// Created by xaq on 11/15/17.
//

#include "neutron_transport.h"
#include "acedata.h"
#include "criticality.h"


extern criti_t base_criti;
extern nuc_xs_t *base_nuc_xs;

static int fis_MT[5] = {18, 19, 20, 21, 38};

void
treat_fission(particle_status_t *par_status)
{
    nuclide_t *nuc;
    nuc_xs_t *cur_nuc_xs;
    double fis_sub_cs[5];

    nuc = par_status->nuc;
    cur_nuc_xs = par_status->nuc_xs;

    if(cur_nuc_xs->fis <= ZERO) return;

    if(nuc->LSIG[18] > 0)  /* 总裂变截面MT=18存在 */
        fis_sub_cs[0] = get_nuc_mt_cs(nuc, 18, par_status->interp_N0, par_status->interp_K0);
    else
        for(int i = 1; i < 5; i++)
            fis_sub_cs[i] = get_nuc_mt_cs(nuc, fis_MT[i], par_status->interp_N0, par_status->interp_K0);

    if(nuc->ptable) {
        double temp = nuc->fis_XSS[par_status->interp_N0]
                      + par_status->interp_K0
                        * (nuc->fis_XSS[par_status->interp_N0 + 1] - nuc->fis_XSS[par_status->interp_N0]);
        if(!EQ_ZERO(temp)) {
            double ff = cur_nuc_xs->fis / temp;
            for(int i = 0; i < 5; i++)
                fis_sub_cs[i] *= ff;
        }
    }

    Estimate_keff_abs(par_status->wgt, cur_nuc_xs->nu, cur_nuc_xs->fis, cur_nuc_xs->tot);

    double fis_R;
    if(nuc->LSIG[18] > 0) {
        if(fis_sub_cs[0] > 0) {
            fis_R = par_status->wgt * cur_nuc_xs->nu * fis_sub_cs[0] / cur_nuc_xs->tot / base_criti.keff_final;
            get_fis_neu_state(par_status, fis_MT[0], fis_R, cur_nuc_xs->nu);
        }
    } else {
        for(int i = 1; i < 5; i++) {
            if(fis_sub_cs[i] > 0) {
                fis_R = par_status->wgt * cur_nuc_xs->nu * fis_sub_cs[i] / cur_nuc_xs->tot / base_criti.keff_final;
                get_fis_neu_state(par_status, fis_MT[i], fis_R, cur_nuc_xs->nu);
            }
        }
    }
}