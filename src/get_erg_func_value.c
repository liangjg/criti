//
// Created by xaq on 9/2/17.
//

#include "acedata.h"
#include "global_fun.h"

double get_erg_func_value(const nuclide_t *nuc, int LDAT, double erg){
    double Ti = 0;
    double erg_1 = 0, erg_2 = 0, func_val_1 = 0, func_val_2 = 0;
    int nNumOfInterpRegion = (int) (nuc->XSS[LDAT]);
    int nE_grid_base = LDAT + 2 * nNumOfInterpRegion + 1;
    int nNumOfErgGrid = (int) (nuc->XSS[nE_grid_base]);

    //use the extreme value if erg is off either end of the table.
    if(erg >= nuc->XSS[nE_grid_base + nNumOfErgGrid]){
        Ti = nuc->XSS[nE_grid_base + 2 * nNumOfErgGrid];
        return Ti;
    } else if(erg <= nuc->XSS[nE_grid_base + 1]){
        Ti = nuc->XSS[nE_grid_base + nNumOfErgGrid + 1];
        return Ti;
    }

    //binary search for the location of erg in the table.
    int min = nE_grid_base + 1;
    int max = nE_grid_base + nNumOfErgGrid;

    double fr;
    int nPos;
    get_intplt_pos_fr_double(nuc->XSS, erg, min, max, &nPos, &fr);

    erg_1 = nuc->XSS[nPos];
    erg_2 = nuc->XSS[nPos + 1];
    func_val_1 = nuc->XSS[nPos + nNumOfErgGrid];
    func_val_2 = nuc->XSS[nPos + 1 + nNumOfErgGrid];

    //find out which kind of interpolation should be used.
    if(nNumOfInterpRegion == 0){
        Ti = func_val_1 + (func_val_2 - func_val_1) * (erg - erg_1) / (erg_2 - erg_1);
        return Ti;
    }
    int n = 0;
    for(n = 1; n <= nNumOfInterpRegion; n++){
        if(nPos + 1 - nE_grid_base <= (int) (nuc->XSS[LDAT + n]))
            goto Interp;
    }
    n = nNumOfInterpRegion;

    // interpolate between table entries.
Interp:
    switch((int) (nuc->XSS[LDAT + nNumOfInterpRegion + n])){
        case 1 :{
            Ti = func_val_1;
            break;
        }
        case 2 :{
            Ti = func_val_1 + (func_val_2 - func_val_1) * (erg - erg_1) / (erg_2 - erg_1);
            break;
        }
        case 3 :{
            Ti = func_val_1 + (func_val_2 - func_val_1) * log(erg / erg_1) / log(erg_2 / erg_1);
            break;
        }
        case 4 :{
            Ti = func_val_1 * pow(func_val_2 / func_val_1, (erg - erg_1) / (erg_2 - erg_1));
            break;
        }
        case 5 :{
            Ti = func_val_1 * pow(func_val_2 / func_val_1, log(erg / erg_1) / log(erg_2 / erg_1));
            break;
        }
    }

    return Ti;
}