//
// Created by xaq on 9/2/17.
//

#include "acedata.h"
#include "global_fun.h"
#include "RNG.h"

void interpolate_xss_table(acedata_t *obj, double dErg, int nNuc, int LDAT, int &nPos, double &dFrac,
                           int &nNumOfInterpRegion, int &nNumOfErgGrid){
    //// obtain number of interpolation regions and incoming energies
    nNumOfInterpRegion = int(obj->nucs[nNuc]->XSS[LDAT]);
    int nE_grid_base = LDAT + 2 * nNumOfInterpRegion + 1;
    nNumOfErgGrid = int(obj->nucs[nNuc]->XSS[nE_grid_base]);
    dFrac = 0;

    // obtain energy bin and interpolation fraction
    int nInterp_1 = nE_grid_base + 1;
    int nInterp_2 = nE_grid_base + nNumOfErgGrid;

    if(dErg <= obj->nucs[nNuc]->XSS[nInterp_1]){
        dFrac = 0;
        nPos = 1;
        return;
    }
    if(dErg >= obj->nucs[nNuc]->XSS[nInterp_2]){
        dFrac = 1;
        nPos = nNumOfErgGrid - 1;
        return;
    }


    get_intplt_pos_fr_double(obj->nucs[nNuc]->XSS, dErg, nInterp_1, nInterp_2, &nPos, &dFrac);
    nPos = nPos - nE_grid_base;

    if(nNumOfInterpRegion == 0)
        return;


    for(int n = 1; n <= nNumOfInterpRegion; ++n){
        if(nInterp_2 - nE_grid_base <= int(obj->nucs[nNuc]->XSS[LDAT + n]) &&
           int(obj->nucs[nNuc]->XSS[LDAT + nNumOfInterpRegion + n]) == 1){
            dFrac = 0;
            return;
        }
    }

    if(int(obj->nucs[nNuc]->XSS[LDAT + nNumOfInterpRegion + nNumOfInterpRegion]) == 1){
        dFrac = 0;
        return;
    }
}


double get_erg_func_value(acedata_t *obj, int nNuc, int LDAT, double dErg){
    double Ti = 0;
    double dErg_1 = 0, dErg_2 = 0, dFunc_val_1 = 0, dFunc_val_2 = 0;
    int nNumOfInterpRegion = int(obj->nucs[nNuc]->XSS[LDAT]);
    int nE_grid_base = LDAT + 2 * nNumOfInterpRegion + 1;
    int nNumOfErgGrid = int(obj->nucs[nNuc]->XSS[nE_grid_base]);

    //use the extreme value if erg is off either end of the table.
    if(dErg >= obj->nucs[nNuc]->XSS[nE_grid_base + nNumOfErgGrid]){
        Ti = obj->nucs[nNuc]->XSS[nE_grid_base + 2 * nNumOfErgGrid];
        return Ti;
    } else if(dErg <= obj->nucs[nNuc]->XSS[nE_grid_base + 1]){
        Ti = obj->nucs[nNuc]->XSS[nE_grid_base + nNumOfErgGrid + 1];
        return Ti;
    }

    //binary search for the location of erg in the table.
    int min = nE_grid_base + 1;
    int max = nE_grid_base + nNumOfErgGrid;

    double fr;
    int nPos;
    get_intplt_pos_fr_double(obj->nucs[nNuc]->XSS, dErg, min, max, &nPos, &fr);

    dErg_1 = obj->nucs[nNuc]->XSS[nPos];
    dErg_2 = obj->nucs[nNuc]->XSS[nPos + 1];
    dFunc_val_1 = obj->nucs[nNuc]->XSS[nPos + nNumOfErgGrid];
    dFunc_val_2 = obj->nucs[nNuc]->XSS[nPos + 1 + nNumOfErgGrid];

    //find out which kind of interpolation should be used.
    if(nNumOfInterpRegion == 0){
        Ti = dFunc_val_1 + (dFunc_val_2 - dFunc_val_1) * (dErg - dErg_1) / (dErg_2 - dErg_1);
        return Ti;
    }
    int n = 0;
    for(n = 1; n <= nNumOfInterpRegion; n++){
        if(nPos + 1 - nE_grid_base <= int(obj->nucs[nNuc]->XSS[LDAT + n]))
            goto Interp;
    }
    n = nNumOfInterpRegion;

    // interpolate between table entries.
    Interp:
    switch(int(obj->nucs[nNuc]->XSS[LDAT + nNumOfInterpRegion + n])){
        case 1 :{
            Ti = dFunc_val_1;
            break;
        }
        case 2 :{
            Ti = dFunc_val_1 + (dFunc_val_2 - dFunc_val_1) * (dErg - dErg_1) / (dErg_2 - dErg_1);
            break;
        }
        case 3 :{
            Ti = dFunc_val_1 + (dFunc_val_2 - dFunc_val_1) * log(dErg / dErg_1) / log(dErg_2 / dErg_1);
            break;
        }
        case 4 :{
            Ti = dFunc_val_1 * pow(dFunc_val_2 / dFunc_val_1, (dErg - dErg_1) / (dErg_2 - dErg_1));
            break;
        }
        case 5 :{
            Ti = dFunc_val_1 * pow(dFunc_val_2 / dFunc_val_1, log(dErg / dErg_1) / log(dErg_2 / dErg_1));
            break;
        }
    }

    return Ti;
}