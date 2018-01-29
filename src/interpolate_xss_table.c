//
// Created by xaq on 11/14/17.
//

#include "acedata.h"
#include "global_fun.h"


void interpolate_xss_table(const nuclide_t *nuc, const double incident_erg, const int LDAT, int *pos, double *frac,
                           int *num_of_interp_region, int *num_of_erg_grid){
    //// obtain number of interpolation regions and incoming energies
    *num_of_interp_region = (int)(nuc->XSS[LDAT]);
    int E_grid_base = LDAT + 2 * (*num_of_interp_region) + 1;
    *num_of_erg_grid = (int)(nuc->XSS[E_grid_base]);
    *frac = 0;

    // obtain energy bin and interpolation fraction
    int interp_1 = E_grid_base + 1;
    int interp_2 = E_grid_base + *num_of_erg_grid;

    if(incident_erg <= nuc->XSS[interp_1]){
        *frac = 0;
        *pos = 1;
        return;
    }
    if(incident_erg >= nuc->XSS[interp_2]){
        *frac = 1;
        *pos = *num_of_erg_grid - 1;
        return;
    }


    *pos = get_intplt_pos_fr(nuc->XSS, incident_erg, interp_1, interp_2, frac);
    *pos = *pos - E_grid_base;

    if(*num_of_interp_region == 0)
        return;


    for(int n = 1; n <= *num_of_interp_region; ++n){
        if(interp_2 - E_grid_base <= (int)(nuc->XSS[LDAT + n]) &&
           (int)(nuc->XSS[LDAT + *num_of_interp_region + n]) == 1){
            *frac = 0;
            return;
        }
    }

    if((int)(nuc->XSS[LDAT + *num_of_interp_region + *num_of_interp_region]) == 1){
        *frac = 0;
        return;
    }
}