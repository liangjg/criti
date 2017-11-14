//
// Created by xaq on 9/2/17.
//

#include "acedata.h"
#include "global_fun.h"

double get_nuc_mt_cs(nuclide_t *nuc, int MT, int nInterpPos, double dInterpFrac){
    if(MT >= nuc->LSIG_sz) /// fix bug: 2013-07-13
        return 0;

    if(nuc->LSIG[MT] <= 0)    // mt number is not found
        return 0;

    int nIE_LOCA = GetLocOfSIG(nuc) + nuc->LSIG[MT] - 1;
    int nSIG_IE = (int) (nuc->XSS[nIE_LOCA]);
    if(nInterpPos < nSIG_IE)
        return 0;
    else
        return INTPLT_BY_POS_FR(nuc->XSS, nIE_LOCA + 2 + nInterpPos - nSIG_IE, dInterpFrac);

}