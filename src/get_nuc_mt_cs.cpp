//
// Created by xaq on 9/2/17.
//

#include "acedata.h"
#include "global_fun.h"

double get_nuc_mt_cs(acedata_t *obj, int nNuc, int MT, int nInterpPos, double dInterpFrac)
{

    if(MT >= obj->nucs[nNuc]->LSIG_sz) /// fix bug: 2013-07-13
        return 0;


    if(obj->nucs[nNuc]->LSIG[MT] <= 0)    // mt number is not found
        return 0;

    int nIE_LOCA = GetLocOfSIG(obj, nNuc) + obj->nucs[nNuc]->LSIG[MT] - 1;
    int nSIG_IE = int(obj->nucs[nNuc]->XSS[nIE_LOCA]);
    if(nInterpPos < nSIG_IE)
        return 0;
    else
        return INTPLT_BY_POS_FR(obj->nucs[nNuc]->XSS, nIE_LOCA + 2 + nInterpPos - nSIG_IE, dInterpFrac);

}