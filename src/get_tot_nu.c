//
// Created by xaq on 9/2/17.
//

#include "acedata.h"

double get_total_nu(nuclide_t *nuc, double erg){
    int nKNU = GetLocOfNU(nuc);
    if(nKNU == 0)   // no neutron yield
        return ZERO;

    int nKNU_judge = (int) (nuc->XSS[nKNU]);

    if(nKNU_judge < 0)  /// both prompt and total NU are given
        nKNU = nKNU - nKNU_judge + 1;

    int LNU = (int) (nuc->XSS[nKNU]);
    ///// calculate NuTemp by evaluating a polynomial in energy /////
    if(LNU == 1){
        int NC = (int) (nuc->XSS[nKNU + 1]) - 1;
        double dNuTemp = nuc->XSS[nKNU + NC + 2];
        for(int i = 1; i <= NC; i++)
            dNuTemp = dNuTemp * erg + nuc->XSS[nKNU + NC + 2 - i];
        return dNuTemp;
    }
    return get_erg_func_value(nuc, nKNU + 1, erg);
}


double get_delayed_nu(nuclide_t *nuc, double erg){
    int nKNU = GetLocOfDNU(nuc);
    if(nKNU == 0)
        return 0;

    return get_erg_func_value(nuc, nKNU + 1, erg);
}
