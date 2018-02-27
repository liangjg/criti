//
// Created by xaq on 9/2/17.
//

#include "acedata.h"
#include "global_fun.h"


double get_erg_func_value(const nuclide_t *nuc, int LDAT, double erg){
    double Ti = ZERO;
    double erg_1, erg_2, func_val_1, func_val_2;
    int num_of_interp_region = (int) (nuc->XSS[LDAT]);
    int E_grid_base = LDAT + 2 * num_of_interp_region + 1;
    int num_of_erg_grid = (int) (nuc->XSS[E_grid_base]);

    if(erg >= nuc->XSS[E_grid_base + num_of_erg_grid]){
        Ti = nuc->XSS[E_grid_base + 2 * num_of_erg_grid];
        return Ti;
    } else if(erg <= nuc->XSS[E_grid_base + 1]){
        Ti = nuc->XSS[E_grid_base + num_of_erg_grid + 1];
        return Ti;
    }

    int min = E_grid_base + 1;
    int max = E_grid_base + num_of_erg_grid;

    double fr;
    int pos;
    pos = get_intplt_pos_fr(nuc->XSS, erg, min, max, &fr);

    erg_1 = nuc->XSS[pos];
    erg_2 = nuc->XSS[pos + 1];
    func_val_1 = nuc->XSS[pos + num_of_erg_grid];
    func_val_2 = nuc->XSS[pos + 1 + num_of_erg_grid];

    if(num_of_interp_region == 0){
        Ti = func_val_1 + (func_val_2 - func_val_1) * (erg - erg_1) / (erg_2 - erg_1);
        return Ti;
    }
    int n;
    for(n = 1; n <= num_of_interp_region; n++){
        if(pos + 1 - E_grid_base <= (int) (nuc->XSS[LDAT + n]))
            goto Interp;
    }
    n = num_of_interp_region;

Interp:
    switch((int) (nuc->XSS[LDAT + num_of_interp_region + n])){
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