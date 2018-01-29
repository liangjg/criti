//
// Created by xaq on 9/2/17.
//

#include "acedata.h"
#include "RNG.h"


/// 抽样得到非弹性散射的Law
///
/// @param[in] nNuc 碰撞核素Index
/// @param[in] MT 反应mt号
/// @param[in] incident_erg 入射能量
/// @param[out] LDAT
/// @return 抽样得到的Law
int get_law_type(const nuclide_t *nuc, const int MT, const double incident_erg, int *LDAT)
{
    int law_type;
    double E1, E0, Pi = ZERO, ksi;
    int LNW, IDAT, NR, NE;
    int which_Pi = 1;

    int LDIS = MT > 0 ? Get_loc_of_DLW(nuc) : Get_loc_of_DNED(nuc);
    int LOCC = MT > 0 ? LDIS + nuc->LDLW[MT] : LDIS + (int)(nuc->XSS[Get_loc_of_DNEDL(nuc) - MT - 1]);

    LNW = (int)(nuc->XSS[LOCC - 1]);
    law_type = (int)(nuc->XSS[LOCC]);
    if(law_type <= 0) {
        printf("unknown Law type %d. \nNuc = %d, MT = %d, Law_type = XSS[%d] = %9.5E.\n",
               law_type, nuc->zaid, MT, LOCC, nuc->XSS[LOCC]);
        base_warnings++;
    }

    IDAT = (int)(nuc->XSS[LOCC + 1]);
    if(LNW == 0) {
        *LDAT = LDIS + IDAT - 1;
        return law_type;
    }

    NR = (int)(nuc->XSS[LOCC + 2]);
    NE = (int)(nuc->XSS[LOCC + 3 + 2 * NR]);

    if(incident_erg <= nuc->XSS[LOCC + 4 + 2 * NR]) {
        which_Pi = 1;
        Pi = nuc->XSS[LOCC + 4 + 2 * NR + NE];
    } else if(incident_erg >= nuc->XSS[LOCC + 4 + 2 * NR + NE - 1]) {
        which_Pi = NE;
        Pi = nuc->XSS[LOCC + 4 + 2 * NR + NE + NE - 1];
    } else {
        for(int j = 1; j <= NE - 1; j++) {
            E1 = nuc->XSS[LOCC + 4 + 2 * NR + j];
            if(incident_erg <= E1) {
                E0 = nuc->XSS[LOCC + 4 + 2 * NR + j - 1];
                if(get_rand() < (incident_erg - E0) / (E1 - E0)) { //  En  or  En+1
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

    ksi = get_rand();
    while(ksi > Pi) {
        Pi = Pi + nuc->XSS[LDIS + LNW + 4 + 2 * NR + NE + which_Pi - 1];
        if(ksi <= Pi) {
            law_type = (int)(nuc->XSS[LDIS + LNW]);
            IDAT = (int)(nuc->XSS[LDIS + LNW + 1]);
            break;
        }
        LNW = (int)(nuc->XSS[LDIS + LNW - 1]);
        if(LNW == 0)
            break;
    }

    *LDAT = LDIS + IDAT - 1;
    return law_type;
}