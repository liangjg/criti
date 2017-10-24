//
// Created by xaq on 9/2/17.
//

#include "acedata.h"
#include "global_fun.h"
#include "RNG.h"

void interpolate_xss_table(acedata_t *obj, double dErg, int nNuc, int LDAT, int &nPos, double &dFrac,
                           int &nNumOfInterpRegion, int &nNumOfErgGrid)
{
    //// obtain number of interpolation regions and incoming energies
    nNumOfInterpRegion = int(obj->nucs[nNuc]->XSS[LDAT]);
    int nE_grid_base = LDAT + 2 * nNumOfInterpRegion + 1;
    nNumOfErgGrid = int(obj->nucs[nNuc]->XSS[nE_grid_base]);
    dFrac = 0;

    // obtain energy bin and interpolation fraction
    int nInterp_1 = nE_grid_base + 1;
    int nInterp_2 = nE_grid_base + nNumOfErgGrid;

    if(dErg <= obj->nucs[nNuc]->XSS[nInterp_1]) {
        dFrac = 0;
        nPos = 1;
        return;
    }
    if(dErg >= obj->nucs[nNuc]->XSS[nInterp_2]) {
        dFrac = 1;
        nPos = nNumOfErgGrid - 1;
        return;
    }


    get_intplt_pos_fr_double(obj->nucs[nNuc]->XSS, dErg, nInterp_1, nInterp_2, &nPos, &dFrac);
    nPos = nPos - nE_grid_base;

    if(nNumOfInterpRegion == 0)
        return ;


    for(int n = 1; n <= nNumOfInterpRegion; ++n) {
        if(nInterp_2 - nE_grid_base <= int(obj->nucs[nNuc]->XSS[LDAT + n]) && int(obj->nucs[nNuc]->XSS[LDAT + nNumOfInterpRegion + n]) == 1) {
            dFrac = 0 ;
            return;
        }
    }

    if(int(obj->nucs[nNuc]->XSS[LDAT + nNumOfInterpRegion + nNumOfInterpRegion]) == 1) {
        dFrac = 0 ;
        return;
    }
}


double get_erg_func_value(acedata_t *obj, int nNuc, int LDAT, double dErg)
{
    double Ti = 0;
    double dErg_1 = 0, dErg_2 = 0, dFunc_val_1 = 0, dFunc_val_2 = 0;
    int nNumOfInterpRegion = int(obj->nucs[nNuc]->XSS[LDAT]);
    int nE_grid_base = LDAT + 2 * nNumOfInterpRegion + 1;
    int nNumOfErgGrid = int(obj->nucs[nNuc]->XSS[nE_grid_base]);

    //use the extreme value if erg is off either end of the table.
    if(dErg >= obj->nucs[nNuc]->XSS[nE_grid_base + nNumOfErgGrid]) {
        Ti = obj->nucs[nNuc]->XSS[nE_grid_base + 2 * nNumOfErgGrid];
        return Ti;
    }

    else if(dErg <= obj->nucs[nNuc]->XSS[nE_grid_base + 1]) {
        Ti = obj->nucs[nNuc]->XSS[nE_grid_base + nNumOfErgGrid + 1];
        return Ti;
    }

    //binary search for the location of erg in the table.
    int min = nE_grid_base + 1;
    int max = nE_grid_base + nNumOfErgGrid;

    double fr;
    int nPos;
    get_intplt_pos_fr_double(obj->nucs[nNuc]->XSS, dErg, min, max, &nPos, &fr);

    dErg_1 = obj->nucs[nNuc]->XSS[nPos];
    dErg_2 = obj->nucs[nNuc]->XSS[nPos + 1];
    dFunc_val_1 = obj->nucs[nNuc]->XSS[nPos + nNumOfErgGrid];
    dFunc_val_2 = obj->nucs[nNuc]->XSS[nPos + 1 + nNumOfErgGrid];

    //find out which kind of interpolation should be used.
    if(nNumOfInterpRegion == 0) {
        Ti = dFunc_val_1 + (dFunc_val_2 - dFunc_val_1) * (dErg - dErg_1) / (dErg_2 - dErg_1);
        return Ti;
    }
    int n = 0;
    for(n = 1; n <= nNumOfInterpRegion; n++) {
        if(nPos + 1 - nE_grid_base <= int(obj->nucs[nNuc]->XSS[LDAT + n]))
            goto Interp;
    }
    n = nNumOfInterpRegion;

    // interpolate between table entries.
    Interp:
    switch(int(obj->nucs[nNuc]->XSS[LDAT + nNumOfInterpRegion + n])) {
        case 1 : {
            Ti = dFunc_val_1;
            break;
        }
        case 2 : {
            Ti = dFunc_val_1 + (dFunc_val_2 - dFunc_val_1) * (dErg - dErg_1) / (dErg_2 - dErg_1);
            break;
        }
        case 3 : {
            Ti = dFunc_val_1 + (dFunc_val_2 - dFunc_val_1) * log(dErg / dErg_1) / log(dErg_2 / dErg_1);
            break;
        }
        case 4 : {
            Ti = dFunc_val_1 * pow(dFunc_val_2 / dFunc_val_1, (dErg - dErg_1) / (dErg_2 - dErg_1));
            break;
        }
        case 5 : {
            Ti = dFunc_val_1 * pow(dFunc_val_2 / dFunc_val_1, log(dErg / dErg_1) / log(dErg_2 / dErg_1));
            break;
        }
    }

    return Ti;
}

double sample_maxwell(acedata_t *obj, double T)
{
    //===============================================================================
    // MAXWELL_SPECTRUM samples an energy from the Maxwell fission distribution based
    // on a direct sampling scheme. The probability distribution function for a
    // Maxwellian is given as p(x) = 2/(T*sqrt(pi))*sqrt(x/T)*exp(-x/T). This PDF can
    // be sampled using rule C64 in the Monte Carlo Sampler LA-9721-MS.
    //===============================================================================

    // T: tabulated function of incoming E
    double ksi1, ksi2, ksi3;
    double c ;       // cosine of pi/2*r3

    ksi1 = get_rand();
    ksi2 = get_rand();
    ksi3 = get_rand();
    c = cos(PI / TWO * ksi3);

    return -T * (log(ksi1) + log(ksi2) * c * c);
}

double sample_watt(acedata_t *obj, double a, double b)
{
    //===============================================================================
    // WATT_SPECTRUM samples the outgoing energy from a Watt energy-dependent fission
    // spectrum. Although fitted parameters exist for many nuclides, generally the
    // continuous tabular distributions (LAW 4) should be used in lieu of the Watt
    // spectrum. This direct sampling scheme is an unpublished scheme based on the
    // original Watt spectrum derivation (See F. Brown's MC lectures).
    //===============================================================================

    double w  = sample_maxwell(obj, a);
    double ksi = get_rand();
    return w + a * a * b / 4. + (TWO * ksi - ONE) * sqrt(a * a * b * w);
}