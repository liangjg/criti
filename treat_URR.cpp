//
// Created by xaq on 9/2/17.
//

#include "acedata.h"
#include "global_fun.h"

void treat_URR(acedata_t *obj, int nNuc, double dEg0)  // Treat Unresolved Resonance Range
{
    double dEl_factor, dFis_factor, dC_factor, dInterp_value, dSmooth_total, sInel_balance, dAbs_balance;
    int nIdx, nIdx_s, nIdx_e, nNum_e;

    int nNum_erg_grid  = GetErgGridNum(obj, nNuc);
    int nLoc_lunr_table  = GetLocOfLUNR(obj, nNuc);
    int nNum_prob_table_incident_erg = int(obj->nucs[nNuc]->XSS[nLoc_lunr_table]);
    int nLoc_incident_erg = nLoc_lunr_table + 6;
    int nLength_table = int(obj->nucs[nNuc]->XSS[nLoc_lunr_table + 1]);
    int nIncident_erg_idx = nLoc_incident_erg + 1;

    for(;;) {
        if(dEg0 < obj->nucs[nNuc]->XSS[nIncident_erg_idx])
            break;
        nIncident_erg_idx = nIncident_erg_idx + 1;
    }

    //// interpolate between prob tables
    double dRatio  = (dEg0 - obj->nucs[nNuc]->XSS[nIncident_erg_idx - 1]) / (obj->nucs[nNuc]->XSS[nIncident_erg_idx] - obj->nucs[nNuc]->XSS[nIncident_erg_idx - 1]);
    int nErg_idx = nIncident_erg_idx - nLoc_incident_erg;
    int nLoc_prob_table = nLoc_incident_erg + nNum_prob_table_incident_erg + (nErg_idx - 1) * nLength_table * 6; //
    int nj1 = nLoc_prob_table;
    int nj2 = nLoc_prob_table + nLength_table * 6;

    double dRand_num = ORNG.Rand();
    for(;;) {
        if(obj->nucs[nNuc]->XSS[nj1] >= dRand_num)
            break;
        nj1 = nj1 + 1;
    }
    for(;;) {
        if(obj->nucs[nNuc]->XSS[nj2] >= dRand_num)
            break;
        nj2 = nj2 + 1;
    }
    if(nj1 - nLoc_prob_table + 1 > nLength_table || nj2 - (nLoc_prob_table + nLength_table * 6) + 1 > nLength_table) {
        printf("prob table not normed in nuc %s.\n", obj->nucs[nNuc]->id);
        warnings++;
        return;
    }


    ///Interpolation parameter between tables  =2 lin-lin; =5 log-log
    int nDistribution_type = int(obj->nucs[nNuc]->XSS[nLoc_lunr_table + 2]);
    if(nDistribution_type != 5 || dRatio <= 0. || dRatio >= 1.) { //// lin-lin interpolation.
        obj->nucs[nNuc]->abs = obj->nucs[nNuc]->XSS[nj1 + nLength_table * 4] + dRatio * (obj->nucs[nNuc]->XSS[nj2 + nLength_table * 4] - obj->nucs[nNuc]->XSS[nj1 + nLength_table * 4]) ; //absorption (capture only)
        obj->nucs[nNuc]->el  = obj->nucs[nNuc]->XSS[nj1 + nLength_table * 2] + dRatio * (obj->nucs[nNuc]->XSS[nj2 + nLength_table * 2] - obj->nucs[nNuc]->XSS[nj1 + nLength_table * 2]) ; //elastic
        obj->nucs[nNuc]->fis = obj->nucs[nNuc]->XSS[nj1 + nLength_table * 3] + dRatio * (obj->nucs[nNuc]->XSS[nj2 + nLength_table * 3] - obj->nucs[nNuc]->XSS[nj1 + nLength_table * 3]) ; //fission (18 only)
        // heating no. + nLength_table*5
    } else if(nDistribution_type == 5) { // log-log interpolation.
        //xs data maybe negative, resulting Nan(Not a Number), add check when reading
        double g = log(dEg0 / obj->nucs[nNuc]->XSS[nIncident_erg_idx - 1]) / log(obj->nucs[nNuc]->XSS[nIncident_erg_idx] / obj->nucs[nNuc]->XSS[nIncident_erg_idx - 1]);
        int i = nLength_table * 4;
        obj->nucs[nNuc]->abs = 0;
        if(obj->nucs[nNuc]->XSS[nj1 + i] != 0)
            obj->nucs[nNuc]->abs = exp(log(obj->nucs[nNuc]->XSS[nj1 + i]) + g * log(obj->nucs[nNuc]->XSS[nj2 + i] / obj->nucs[nNuc]->XSS[nj1 + i]));
        i =  nLength_table * 2;
        obj->nucs[nNuc]->el = exp(log(obj->nucs[nNuc]->XSS[nj1 + i]) + g * log(obj->nucs[nNuc]->XSS[nj2 + i] / obj->nucs[nNuc]->XSS[nj1 + i]));
        i = nLength_table * 3;
        obj->nucs[nNuc]->fis = 0;
        if(obj->nucs[nNuc]->XSS[nj1 + i] != 0)
            obj->nucs[nNuc]->fis = exp(log(obj->nucs[nNuc]->XSS[nj1 + i]) + g * log(obj->nucs[nNuc]->XSS[nj2 + i] / obj->nucs[nNuc]->XSS[nj1 + i]));
    } else {
        printf("unknown interpolation type in pTable of nuc %s.\n", obj->nucs[nNuc]->id);
        warnings++;
    }

    // calculate cross   sections
    int nFactor_flag = int(obj->nucs[nNuc]->XSS[nLoc_lunr_table + 5]); // Factors flag
    int nInel_competition_flag = int(obj->nucs[nNuc]->XSS[nLoc_lunr_table + 3]); // Inelastic competition flag
    int nOther_abs_flag = int(obj->nucs[nNuc]->XSS[nLoc_lunr_table + 4]); // Other absorption flag
    if(nFactor_flag != 0 || nInel_competition_flag == 0) {
        int nInterp_pos = obj->nucs[nNuc]->inter_pos;
        dEl_factor = obj->nucs[nNuc]->XSS[nInterp_pos + 3 * nNum_erg_grid] + obj->nucs[nNuc]->inter_frac * (obj->nucs[nNuc]->XSS[nInterp_pos + 3 * nNum_erg_grid + 1] - obj->nucs[nNuc]->XSS[nInterp_pos + 3 * nNum_erg_grid]);
        dFis_factor = obj->nucs[nNuc]->fis_XSS[nInterp_pos] + obj->nucs[nNuc]->inter_frac * (obj->nucs[nNuc]->fis_XSS[nInterp_pos + 1] - obj->nucs[nNuc]->fis_XSS[nInterp_pos]);
        dFis_factor = 0;
        int i = GetLocOfFIS(obj, nNuc);
        if(i != 0) {
            int nInterp_i = obj->nucs[nNuc]->inter_pos - int(obj->nucs[nNuc]->XSS[i]) + 1;
            if(nInterp_i >= 1) {
                nInterp_i = nInterp_i + i + 1;
                dFis_factor = obj->nucs[nNuc]->XSS[nInterp_i] + obj->nucs[nNuc]->inter_frac * (obj->nucs[nNuc]->XSS[nInterp_i + 1] - obj->nucs[nNuc]->XSS[nInterp_i]);
            }
        }
    }

    /////// get other absorption factor.
    if(nFactor_flag != 0  || nOther_abs_flag == 0.) {
        dC_factor = 0;
        int i = GetLocOfMTR(obj, nNuc) - 1; // MTR block
        for(nIdx = 1; nIdx <= GetNonElMtNum(obj, nNuc); nIdx++) {
            if(int(obj->nucs[nNuc]->XSS[i + nIdx]) == 102) {
                nIdx_s = GetLocOfSIG(obj, nNuc) + int(obj->nucs[nNuc]->XSS[GetLocOfLSIG(obj, nNuc) + nIdx - 1]); // JXS(7)+LOCA : Location of NE
                nIdx_e = int(obj->nucs[nNuc]->XSS[nIdx_s - 1]);  //  int(obj->nucs[nuc]->XSS[iq-1])  :IE
                nNum_e = int(obj->nucs[nNuc]->XSS[nIdx_s]);
                int ic = obj->nucs[nNuc]->inter_pos + 1 - nIdx_e;
                if(ic >= 1 && ic <= nNum_e) {
                    if(ic == nNum_e)
                        dC_factor = obj->nucs[nNuc]->XSS[nIdx_s + nNum_e];
                    else
                        dC_factor = obj->nucs[nNuc]->XSS[nIdx_s + ic] + obj->nucs[nNuc]->inter_frac * (obj->nucs[nNuc]->XSS[nIdx_s + ic + 1] - obj->nucs[nNuc]->XSS[nIdx_s + ic]);
                }
                break;
            }
        }


        if(nFactor_flag != 0) {
            obj->nucs[nNuc]->el = obj->nucs[nNuc]->el * dEl_factor;    // elastic
            obj->nucs[nNuc]->fis = obj->nucs[nNuc]->fis * dFis_factor;  // fission   (18 or 19/20/21/38)
            obj->nucs[nNuc]->abs = obj->nucs[nNuc]->abs * dC_factor;  // absorption  (102 only)
        }
    }

    sInel_balance = 0;
    dAbs_balance = 0;


    if(nInel_competition_flag == 0 || nOther_abs_flag == 0) {
        int ii = obj->nucs[nNuc]->inter_pos;
        dInterp_value = obj->nucs[nNuc]->XSS[ii + 2 * nNum_erg_grid] + obj->nucs[nNuc]->inter_frac * (obj->nucs[nNuc]->XSS[ii + 2 * nNum_erg_grid + 1] - obj->nucs[nNuc]->XSS[ii + 2 * nNum_erg_grid]);
    }

    if(nInel_competition_flag > 0) {
        // ILF>0.  calculate inelastic
        int i = GetLocOfMTR(obj, nNuc) - 1 ;
        for(nIdx = 1; nIdx <= GetNonElMtNum(obj, nNuc); ++nIdx) {
            if(int(obj->nucs[nNuc]->XSS[i + nIdx]) == nInel_competition_flag)
                break;
        }
        if(nIdx > GetNonElMtNum(obj, nNuc)) {
            puts("total inelastic MT is not in table.");
            warnings++;
        }

        nIdx_s = GetLocOfSIG(obj, nNuc) + int(obj->nucs[nNuc]->XSS[GetLocOfLSIG(obj, nNuc) + nIdx - 1]);
        nIdx_e = int(obj->nucs[nNuc]->XSS[nIdx_s - 1]);
        nNum_e = int(obj->nucs[nNuc]->XSS[nIdx_s]);
        int idx_c = obj->nucs[nNuc]->inter_pos + 1 - nIdx_e;
        if(idx_c >= 1 && idx_c <= nNum_e) {
            if(idx_c == nNum_e)
                sInel_balance = obj->nucs[nNuc]->XSS[nIdx_s + nNum_e];
            else
                sInel_balance = obj->nucs[nNuc]->XSS[nIdx_s + idx_c] + obj->nucs[nNuc]->inter_frac * (obj->nucs[nNuc]->XSS[nIdx_s + idx_c + 1] - obj->nucs[nNuc]->XSS[nIdx_s + idx_c]);
        }
    }
        // ILF=0
    else if(nInel_competition_flag == 0) {

        int nIdx_c = obj->nucs[nNuc]->inter_pos;
        dSmooth_total = obj->nucs[nNuc]->XSS[nIdx_c + nNum_erg_grid] + obj->nucs[nNuc]->inter_frac * (obj->nucs[nNuc]->XSS[nIdx_c + nNum_erg_grid + 1] - obj->nucs[nNuc]->XSS[nIdx_c + nNum_erg_grid]);
        sInel_balance = dSmooth_total - dEl_factor - dFis_factor - dInterp_value;
    }
    //obj->nucs[nuc]->p_ONucCs.p_dInel=r1;  // 2010-11-04


    // check flag for other absorption to include in abs and total.
    // obtain other absorption by smooth (abs-102).
    if(nOther_abs_flag > 0) {  // jxs(23,iex)+4 contains the mt of other absorption.
        int i = GetLocOfMTR(obj, nNuc) - 1 ;
        for(nIdx = 1; nIdx <= GetNonElMtNum(obj, nNuc); ++nIdx) {
            if(int(obj->nucs[nNuc]->XSS[i + nIdx]) == nOther_abs_flag)
                break;
        }
        if(nIdx > GetNonElMtNum(obj, nNuc)) {
            puts("other absorption mt not in table.");
            warnings++;
        }

        nIdx_s = GetLocOfSIG(obj, nNuc) + int(obj->nucs[nNuc]->XSS[GetLocOfLSIG(obj, nNuc) + nIdx - 1]);
        nIdx_e = int(obj->nucs[nNuc]->XSS[nIdx_s - 1]);
        nNum_e = int(obj->nucs[nNuc]->XSS[nIdx_s]);
        int idx_c = obj->nucs[nNuc]->inter_pos + 1 - int(obj->nucs[nNuc]->XSS[nIdx_s - 1]);
        if(idx_c >= 1 && idx_c <= nNum_e) {
            if(idx_c == nNum_e)
                dAbs_balance = obj->nucs[nNuc]->XSS[nIdx_s + nNum_e];
            else
                dAbs_balance = obj->nucs[nNuc]->XSS[nIdx_s + idx_c] + obj->nucs[nNuc]->inter_frac * (obj->nucs[nNuc]->XSS[nIdx_s + idx_c + 1] - obj->nucs[nNuc]->XSS[nIdx_s + idx_c]);
        }
    } else if(nOther_abs_flag == 0)
        dAbs_balance = dInterp_value - dC_factor;

    // increment cross sections for inelastic and other absorption.
    obj->nucs[nNuc]->abs = obj->nucs[nNuc]->abs + dAbs_balance ;    // absorption=capture +  other absorption
    if(obj->nucs[nNuc]->abs < 0) {
        int NE = GetErgGridNum(obj, nNuc);
        obj->nucs[nNuc]->abs = INTPLT_BY_POS_FR(obj->nucs[nNuc]->XSS, obj->nucs[nNuc]->inter_pos + 2 * NE, obj->nucs[nNuc]->inter_frac); // absorption, E0
    }

    obj->nucs[nNuc]->inel = sInel_balance;
    obj->nucs[nNuc]->tot =  obj->nucs[nNuc]->abs + obj->nucs[nNuc]->el + obj->nucs[nNuc]->inel + obj->nucs[nNuc]->fis;

    // Finally, get  total,absorption,elastic,fission
}