//
// Created by xaq on 9/2/17.
//

#include "acedata.h"

void transform_cm_to_lab(acedata_t *obj, int nNuc, int nMT, double dIncidErg, double dExitErg_Cm, double dMu_Cm,
                         double &dExitErg_Lab, double &dMu_Lab)
{
    if(GetEmissNeuNum(obj, nNuc, nMT) < 0) {
        double aw = obj->nucs[nNuc]->atom_wgt;
        dExitErg_Lab = dExitErg_Cm + (dIncidErg + 2 * dMu_Cm * (aw + 1) * sqrt(dIncidErg * dExitErg_Cm)) / (((1 + aw) * (1 + aw)));
        dMu_Lab = dMu_Cm * sqrt(dExitErg_Cm / dExitErg_Lab) + sqrt(dIncidErg / dExitErg_Lab) / (aw + 1);
    } else {
        dExitErg_Lab = dExitErg_Cm;
        dMu_Lab = dMu_Cm;
    }
}
