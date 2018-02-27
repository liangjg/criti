//
// Created by xaq on 9/2/17.
//

#include "acedata.h"


double get_total_nu(nuclide_t *nuc, double erg){
    int KNU = Get_loc_of_NU(nuc);
    if(KNU == 0)   // no neutron yield
        return ZERO;

    int KNU_judge = (int) (nuc->XSS[KNU]);

    if(KNU_judge < 0)  /// both prompt and total NU are given
        KNU = KNU - KNU_judge + 1;

    int LNU = (int) (nuc->XSS[KNU]);
    ///// calculate NuTemp by evaluating a polynomial in energy /////
    if(LNU == 1){
        int i;
        int NC = (int) (nuc->XSS[KNU + 1]) - 1;
        double nu = nuc->XSS[KNU + NC + 2];
        for(i = 1; i <= NC; i++)
            nu = nu * erg + nuc->XSS[KNU + NC + 2 - i];
        return nu;
    }
    return get_erg_func_value(nuc, KNU + 1, erg);
}