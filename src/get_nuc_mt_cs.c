//
// Created by xaq on 9/2/17.
//

#include "acedata.h"
#include "global_fun.h"


double
get_nuc_mt_cs(nuclide_t *nuc,
              int MT,
              int interp_pos,
              double interp_frac)
{
    if(MT >= nuc->LSIG_sz)
        return 0;

    if(nuc->LSIG[MT] <= 0)
        return 0;

    int IE_LOCA = Get_loc_of_SIG(nuc) + nuc->LSIG[MT] - 1;
    int SIG_IE = (int) (nuc->XSS[IE_LOCA]);
    if(interp_pos < SIG_IE)
        return 0;
    else
        return intplt_by_pos_fr(nuc->XSS, IE_LOCA + 2 + interp_pos - SIG_IE, interp_frac);

}