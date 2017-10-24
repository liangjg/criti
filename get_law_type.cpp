//
// Created by xaq on 9/2/17.
//

#include "acedata.h"
#include "RNG.h"

void get_law_type(acedata_t *obj, int nNuc, int nMT, double dIncidErg, int &nLawType, int &LDAT)
{
    double E1, E0, Pi = 0, ksi;
    int LNW, IDAT, NR, NE;
    int which_Pi = 1;

    int LDIS = nMT > 0 ? GetLocOfDLW(obj, nNuc) : GetLocOfDNED(obj, nNuc);
    int LOCC = nMT > 0 ? LDIS + obj->nucs[nNuc]->LDLW[nMT] : LDIS + int(obj->nucs[nNuc]->XSS[GetLocOfDNEDL(obj, nNuc) - nMT - 1]);

    LNW = int(obj->nucs[nNuc]->XSS[LOCC - 1]);
    nLawType = int(obj->nucs[nNuc]->XSS[LOCC]);
    if(nLawType <= 0) {
        printf("unknown Law type %d. \nNuc = %d, MT = %d, Law_type = XSS[%d] = %9.5E.\n",
               nLawType, obj->nucs[nNuc]->zaid, nMT, LOCC, obj->nucs[nNuc]->XSS[LOCC]);
        warnings++;
    }

    IDAT = int(obj->nucs[nNuc]->XSS[LOCC + 1]);
    if(LNW == 0) {
        LDAT = LDIS + IDAT - 1;
        return;
    }

    NR = int(obj->nucs[nNuc]->XSS[LOCC + 2]);
    NE = int(obj->nucs[nNuc]->XSS[LOCC + 3 + 2 * NR]);

    if(dIncidErg <= obj->nucs[nNuc]->XSS[LOCC + 4 + 2 * NR]) {
        which_Pi = 1;
        Pi = obj->nucs[nNuc]->XSS[LOCC + 4 + 2 * NR + NE];
    } else if(dIncidErg >= obj->nucs[nNuc]->XSS[LOCC + 4 + 2 * NR + NE - 1]) {
        which_Pi = NE;
        Pi = obj->nucs[nNuc]->XSS[LOCC + 4 + 2 * NR + NE + NE - 1];
    } else {
        for(int j = 1; j <= NE - 1; j++) {
            E1 = obj->nucs[nNuc]->XSS[LOCC + 4 + 2 * NR + j];
            if(dIncidErg <= E1) {
                E0 = obj->nucs[nNuc]->XSS[LOCC + 4 + 2 * NR + j - 1];
                if(get_rand() < (dIncidErg - E0) / (E1 - E0)) { //  En  or  En+1
                    which_Pi = j + 1;
                    Pi = obj->nucs[nNuc]->XSS[LOCC + 4 + 2 * NR + NE + j];
                } else {
                    which_Pi = j;
                    Pi = obj->nucs[nNuc]->XSS[LOCC + 4 + 2 * NR + NE + j - 1];
                }
                break;
            }
        }
    }

    ksi = get_rand();
    while(ksi > Pi) {
        Pi = Pi + obj->nucs[nNuc]->XSS[LDIS + LNW + 4 + 2 * NR + NE + which_Pi - 1];
        if(ksi <= Pi) {
            nLawType = int(obj->nucs[nNuc]->XSS[LDIS + LNW]);
            IDAT = int(obj->nucs[nNuc]->XSS[LDIS + LNW + 1]);
            break;
        }
        LNW = int(obj->nucs[nNuc]->XSS[LDIS + LNW - 1]);
        if(LNW == 0)
            break;
    }

    LDAT = LDIS + IDAT - 1;
}