//
// Created by xaq on 9/2/17.
//

#include "acedata.h"


int
get_law_type(const nuclide_t *nuc,
             RNG_t *RNG,
             int MT,
             double incident_erg,
             int *LDAT)
{
    int law_type;
    double E1, E0, Pi = ZERO, ksi;
    int LNW, IDAT, NR, NE;
    int which_Pi = 1;

    int LDIS = MT > 0 ? Get_loc_of_DLW(nuc) : Get_loc_of_DNED(nuc);
    int LOCC = MT > 0 ? LDIS + nuc->LDLW[MT] : LDIS + (int) (nuc->XSS[Get_loc_of_DNEDL(nuc) - MT - 1]);

    LNW = (int) (nuc->XSS[LOCC - 1]);
    law_type = (int) (nuc->XSS[LOCC]);
    if(law_type <= 0) {
        printf("unknown Law type %d. \nNuc = %d, MT = %d, Law_type = XSS[%d] = %9.5E.\n",
               law_type, nuc->zaid, MT, LOCC, nuc->XSS[LOCC]);
        base_warnings++;
    }

    IDAT = (int) (nuc->XSS[LOCC + 1]);
    if(LNW == 0) {
        *LDAT = LDIS + IDAT - 1;
        return law_type;
    }

    NR = (int) (nuc->XSS[LOCC + 2]);
    NE = (int) (nuc->XSS[LOCC + 3 + 2 * NR]);

    if(incident_erg <= nuc->XSS[LOCC + 4 + 2 * NR]) {
        which_Pi = 1;
        Pi = nuc->XSS[LOCC + 4 + 2 * NR + NE];
    } else if(incident_erg >= nuc->XSS[LOCC + 4 + 2 * NR + NE - 1]) {
        which_Pi = NE;
        Pi = nuc->XSS[LOCC + 4 + 2 * NR + NE + NE - 1];
    } else {
        int j;
        for(j = 1; j <= NE - 1; j++) {
            E1 = nuc->XSS[LOCC + 4 + 2 * NR + j];
            if(incident_erg <= E1) {
                E0 = nuc->XSS[LOCC + 4 + 2 * NR + j - 1];
                if(get_rand(RNG) < (incident_erg - E0) / (E1 - E0)) { //  En  or  En+1
                    which_Pi = j + 1;
                    Pi = nuc->XSS[LOCC + 4 + 2 * NR + NE + j];
                } else {
                    which_Pi = j;
                    Pi = nuc->XSS[LOCC + 4 + 2 * NR + NE + j - 1];
                }
                break;
            }
        }
    }

    ksi = get_rand(RNG);
    while(ksi > Pi) {
        Pi = Pi + nuc->XSS[LDIS + LNW + 4 + 2 * NR + NE + which_Pi - 1];
        if(ksi <= Pi) {
            law_type = (int) (nuc->XSS[LDIS + LNW]);
            IDAT = (int) (nuc->XSS[LDIS + LNW + 1]);
            break;
        }
        LNW = (int) (nuc->XSS[LDIS + LNW - 1]);
        if(LNW == 0)
            break;
    }

    *LDAT = LDIS + IDAT - 1;
    return law_type;
}