//
// Created by xaq on 9/2/17.
//

#include "acedata.h"
#include "RNG.h"


void get_ce_exist_erg_mu(acedata_t *obj, int nNuc, int nMT, double dIncidErg, double &dExitErg_Lab, double &dMu_Lab)
{
    double dMu_Cm = 0;
    double dExitErg_Cm;

    if(obj->nucs[nNuc]->LAND[nMT] >= 0)
        dMu_Cm = get_scatt_cosine(obj, nNuc, nMT, dIncidErg);

    if(nMT == 2) {
        double aw = obj->nucs[nNuc]->atom_wgt ; // atomic weight
        if(aw >= 1) {
            dExitErg_Lab = dIncidErg * (1 + (aw * aw) + 2 * aw * dMu_Cm) / ((1 + aw) * (1 + aw));
            dMu_Lab = (1 + aw * dMu_Cm) / sqrt(1 + 2 * aw * dMu_Cm + (aw * aw));
        } else {
            dExitErg_Lab = 0.5 * dIncidErg * (1 + dMu_Cm);
            dMu_Lab = sqrt(0.5 + 0.5 * dMu_Cm);
        }
    } else {
        int LawType, LDAT;

        get_law_type(obj, nNuc, nMT, dIncidErg, LawType, LDAT);

        react_by_laws(obj, nNuc, nMT, LawType, LDAT, dIncidErg, dExitErg_Cm, dMu_Cm);

        transform_cm_to_lab(obj, nNuc, nMT, dIncidErg, dExitErg_Cm, dMu_Cm, dExitErg_Lab, dMu_Lab);
    }

    /////////////////  check exit Erg and Mu in lab /////////////
    if(dExitErg_Lab == 0)  // occasionally with MT = 2
        dExitErg_Lab = ZERO_ERG ;
    else if(!(dExitErg_Lab > 0 && dExitErg_Lab < 100)) {
        printf("exit erg_lab out of range. nuc=%d, MT=%d, Erg=%9.6E\n", nNuc, nMT, dExitErg_Lab);
        base_warnings++;
        dExitErg_Lab = ZERO_ERG ;
    }

    if(!(dMu_Lab >= -1.000001 && dMu_Lab <= 1.000001)) {
        printf("exit mu_lab out of range. nuc=%d, MT=%d, Mu=%f\n", nNuc, nMT, dMu_Lab);
        base_warnings++;
        dMu_Lab = 2 * get_rand() - 1.;
    }
}