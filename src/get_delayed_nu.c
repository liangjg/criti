//
// Created by xaq on 2018/1/29.
//

#include "acedata.h"


double
get_delayed_nu(nuclide_t *nuc,
               double erg)
{
    int nKNU = Get_loc_of_DNU(nuc);
    if(nKNU == 0)
        return 0;

    return get_erg_func_value(nuc, nKNU + 1, erg);
}