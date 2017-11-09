//
// Created by xaq on 9/2/17.
//

#include "acedata.h"

double get_total_nu(acedata_t *obj, double dEg0, int nNuc)
{
    int nKNU = GetLocOfNU(obj, nNuc);
    if(nKNU == 0)   // no neutron yield
        return 0;

    int nKNU_judge = int(obj->nucs[nNuc]->XSS[nKNU]);

    if(nKNU_judge < 0)  /// both prompt and total NU are given
        nKNU = nKNU - nKNU_judge + 1;

    int LNU = int(obj->nucs[nNuc]->XSS[nKNU]);
    ///// calculate NuTemp by evaluating a polynomial in energy /////
    if(LNU == 1) {
        int NC = int(obj->nucs[nNuc]->XSS[nKNU + 1]) - 1;
        double dNuTemp = obj->nucs[nNuc]->XSS[nKNU + NC + 2] ;
        for(int i = 1; i <= NC; i++)
            dNuTemp = dNuTemp * dEg0 + obj->nucs[nNuc]->XSS[nKNU + NC + 2 - i];
        return dNuTemp;
    }
    return get_erg_func_value(obj, nNuc, nKNU + 1, dEg0);
}


double get_delayed_nu(acedata_t *obj, double dEg0, int nNuc)
{
    int nKNU = GetLocOfDNU(obj, nNuc);
    if(nKNU == 0)
        return 0;

    return get_erg_func_value(obj, nNuc, nKNU + 1, dEg0);
}
