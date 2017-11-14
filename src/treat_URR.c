//
// Created by xaq on 9/2/17.
//

#include "acedata.h"
#include "global_fun.h"
#include "RNG.h"

void treat_URR(nuclide_t *nuc, double erg)  // Treat Unresolved Resonance Range
{
    double dEl_factor, dFis_factor, dC_factor, dInterp_value, dSmooth_total, sInel_balance, dAbs_balance;
    int nIdx, nIdx_s, nIdx_e, nNum_e;

    int nNum_erg_grid = GetErgGridNum(nuc);
    int nLoc_lunr_table = GetLocOfLUNR(nuc);
    int nNum_prob_table_incident_erg = (int) (nuc->XSS[nLoc_lunr_table]);
    int nLoc_incident_erg = nLoc_lunr_table + 6;
    int nLength_table = (int) (nuc->XSS[nLoc_lunr_table + 1]);
    int nIncident_erg_idx = nLoc_incident_erg + 1;

    for(;;){
        if(erg < nuc->XSS[nIncident_erg_idx])
            break;
        nIncident_erg_idx = nIncident_erg_idx + 1;
    }

    //// interpolate between prob tables
    double dRatio =
            (erg - nuc->XSS[nIncident_erg_idx - 1]) / (nuc->XSS[nIncident_erg_idx] - nuc->XSS[nIncident_erg_idx - 1]);
    int nErg_idx = nIncident_erg_idx - nLoc_incident_erg;
    int nLoc_prob_table = nLoc_incident_erg + nNum_prob_table_incident_erg + (nErg_idx - 1) * nLength_table * 6; //
    int nj1 = nLoc_prob_table;
    int nj2 = nLoc_prob_table + nLength_table * 6;

    double dRand_num = get_rand();
    for(;;){
        if(nuc->XSS[nj1] >= dRand_num)
            break;
        nj1 = nj1 + 1;
    }
    for(;;){
        if(nuc->XSS[nj2] >= dRand_num)
            break;
        nj2 = nj2 + 1;
    }
    if(nj1 - nLoc_prob_table + 1 > nLength_table || nj2 - (nLoc_prob_table + nLength_table * 6) + 1 > nLength_table){
        printf("prob table not normed in nuc %s.\n", nuc->id);
        base_warnings++;
        return;
    }

    ///Interpolation parameter between tables  =2 lin-lin; =5 log-log
    int nDistribution_type = (int) (nuc->XSS[nLoc_lunr_table + 2]);
    if(nDistribution_type != 5 || dRatio <= 0. || dRatio >= 1.){ //// lin-lin interpolation.
        nuc->abs = nuc->XSS[nj1 + nLength_table * 4] + dRatio * (nuc->XSS[nj2 + nLength_table * 4] - nuc->XSS[nj1 +
                                                                                                              nLength_table *
                                                                                                              4]); //absorption (capture only)
        nuc->el = nuc->XSS[nj1 + nLength_table * 2] +
                  dRatio * (nuc->XSS[nj2 + nLength_table * 2] - nuc->XSS[nj1 + nLength_table * 2]); //elastic
        nuc->fis = nuc->XSS[nj1 + nLength_table * 3] +
                   dRatio * (nuc->XSS[nj2 + nLength_table * 3] - nuc->XSS[nj1 + nLength_table * 3]); //fission (18 only)
        // heating no. + nLength_table*5
    } else{ // log-log interpolation.
        //xs data maybe negative, resulting Nan(Not a Number), add check when reading
        double g = log(erg / nuc->XSS[nIncident_erg_idx - 1]) /
                   log(nuc->XSS[nIncident_erg_idx] / nuc->XSS[nIncident_erg_idx - 1]);
        int i = nLength_table * 4;
        nuc->abs = 0;
        if(nuc->XSS[nj1 + i] != 0)
            nuc->abs = exp(log(nuc->XSS[nj1 + i]) + g * log(nuc->XSS[nj2 + i] / nuc->XSS[nj1 + i]));
        i = nLength_table * 2;
        nuc->el = exp(log(nuc->XSS[nj1 + i]) + g * log(nuc->XSS[nj2 + i] / nuc->XSS[nj1 + i]));
        i = nLength_table * 3;
        nuc->fis = 0;
        if(nuc->XSS[nj1 + i] != 0)
            nuc->fis = exp(log(nuc->XSS[nj1 + i]) + g * log(nuc->XSS[nj2 + i] / nuc->XSS[nj1 + i]));
    }

    // calculate cross   sections
    int nFactor_flag = (int) (nuc->XSS[nLoc_lunr_table + 5]); // Factors flag
    int nInel_competition_flag = (int) (nuc->XSS[nLoc_lunr_table + 3]); // Inelastic competition flag
    int nOther_abs_flag = (int) (nuc->XSS[nLoc_lunr_table + 4]); // Other absorption flag
    if(nFactor_flag != 0 || nInel_competition_flag == 0){
        int nInterp_pos = nuc->inter_pos;
        dEl_factor = nuc->XSS[nInterp_pos + 3 * nNum_erg_grid] + nuc->inter_frac *
                                                                 (nuc->XSS[nInterp_pos + 3 * nNum_erg_grid + 1] -
                                                                  nuc->XSS[nInterp_pos + 3 * nNum_erg_grid]);
        dFis_factor = nuc->fis_XSS[nInterp_pos] +
                      nuc->inter_frac * (nuc->fis_XSS[nInterp_pos + 1] - nuc->fis_XSS[nInterp_pos]);
        dFis_factor = 0;
        int i = GetLocOfFIS(nuc);
        if(i != 0){
            int nInterp_i = nuc->inter_pos - (int) (nuc->XSS[i]) + 1;
            if(nInterp_i >= 1){
                nInterp_i = nInterp_i + i + 1;
                dFis_factor = nuc->XSS[nInterp_i] + nuc->inter_frac * (nuc->XSS[nInterp_i + 1] - nuc->XSS[nInterp_i]);
            }
        }
    }

    /////// get other absorption factor.
    if(nFactor_flag != 0 || nOther_abs_flag == 0.){
        dC_factor = 0;
        int i = GetLocOfMTR(nuc) - 1; // MTR block
        for(nIdx = 1; nIdx <= GetNonElMtNum(nuc); nIdx++){
            if((int) (nuc->XSS[i + nIdx]) == 102){
                nIdx_s = GetLocOfSIG(nuc) +
                         (int) (nuc->XSS[GetLocOfLSIG(nuc) + nIdx - 1]); // JXS(7)+LOCA : Location of NE
                nIdx_e = (int) (nuc->XSS[nIdx_s - 1]);  //  int(obj->nucs[nuc]->XSS[iq-1])  :IE
                nNum_e = (int) (nuc->XSS[nIdx_s]);
                int ic = nuc->inter_pos + 1 - nIdx_e;
                if(ic >= 1 && ic <= nNum_e){
                    if(ic == nNum_e)
                        dC_factor = nuc->XSS[nIdx_s + nNum_e];
                    else
                        dC_factor = nuc->XSS[nIdx_s + ic] +
                                    nuc->inter_frac * (nuc->XSS[nIdx_s + ic + 1] - nuc->XSS[nIdx_s + ic]);
                }
                break;
            }
        }


        if(nFactor_flag != 0){
            nuc->el = nuc->el * dEl_factor;    // elastic
            nuc->fis = nuc->fis * dFis_factor;  // fission   (18 or 19/20/21/38)
            nuc->abs = nuc->abs * dC_factor;  // absorption  (102 only)
        }
    }

    sInel_balance = 0;
    dAbs_balance = 0;


    if(nInel_competition_flag == 0 || nOther_abs_flag == 0){
        int ii = nuc->inter_pos;
        dInterp_value = nuc->XSS[ii + 2 * nNum_erg_grid] +
                        nuc->inter_frac * (nuc->XSS[ii + 2 * nNum_erg_grid + 1] - nuc->XSS[ii + 2 * nNum_erg_grid]);
    }

    if(nInel_competition_flag > 0){
        // ILF>0.  calculate inelastic
        int i = GetLocOfMTR(nuc) - 1;
        for(nIdx = 1; nIdx <= GetNonElMtNum(nuc); ++nIdx){
            if((int) (nuc->XSS[i + nIdx]) == nInel_competition_flag)
                break;
        }
        if(nIdx > GetNonElMtNum(nuc)){
            puts("total inelastic MT is not in table.");
            base_warnings++;
        }

        nIdx_s = GetLocOfSIG(nuc) + (int) (nuc->XSS[GetLocOfLSIG(nuc) + nIdx - 1]);
        nIdx_e = (int) (nuc->XSS[nIdx_s - 1]);
        nNum_e = (int) (nuc->XSS[nIdx_s]);
        int idx_c = nuc->inter_pos + 1 - nIdx_e;
        if(idx_c >= 1 && idx_c <= nNum_e){
            if(idx_c == nNum_e)
                sInel_balance = nuc->XSS[nIdx_s + nNum_e];
            else
                sInel_balance = nuc->XSS[nIdx_s + idx_c] +
                                nuc->inter_frac * (nuc->XSS[nIdx_s + idx_c + 1] - nuc->XSS[nIdx_s + idx_c]);
        }
    }
        // ILF=0
    else if(nInel_competition_flag == 0){

        int nIdx_c = nuc->inter_pos;
        dSmooth_total = nuc->XSS[nIdx_c + nNum_erg_grid] +
                        nuc->inter_frac * (nuc->XSS[nIdx_c + nNum_erg_grid + 1] - nuc->XSS[nIdx_c + nNum_erg_grid]);
        sInel_balance = dSmooth_total - dEl_factor - dFis_factor - dInterp_value;
    }
    //obj->nucs[nuc]->p_ONucCs.p_dInel=r1;  // 2010-11-04


    // check flag for other absorption to include in abs and total.
    // obtain other absorption by smooth (abs-102).
    if(nOther_abs_flag > 0){  // jxs(23,iex)+4 contains the mt of other absorption.
        int i = GetLocOfMTR(nuc) - 1;
        for(nIdx = 1; nIdx <= GetNonElMtNum(nuc); ++nIdx){
            if((int) (nuc->XSS[i + nIdx]) == nOther_abs_flag)
                break;
        }
        if(nIdx > GetNonElMtNum(nuc)){
            puts("other absorption mt not in table.");
            base_warnings++;
        }

        nIdx_s = GetLocOfSIG(nuc) + (int) (nuc->XSS[GetLocOfLSIG(nuc) + nIdx - 1]);
        nIdx_e = (int) (nuc->XSS[nIdx_s - 1]);
        nNum_e = (int) (nuc->XSS[nIdx_s]);
        int idx_c = nuc->inter_pos + 1 - (int) (nuc->XSS[nIdx_s - 1]);
        if(idx_c >= 1 && idx_c <= nNum_e){
            if(idx_c == nNum_e)
                dAbs_balance = nuc->XSS[nIdx_s + nNum_e];
            else
                dAbs_balance = nuc->XSS[nIdx_s + idx_c] +
                               nuc->inter_frac * (nuc->XSS[nIdx_s + idx_c + 1] - nuc->XSS[nIdx_s + idx_c]);
        }
    } else if(nOther_abs_flag == 0)
        dAbs_balance = dInterp_value - dC_factor;

    // increment cross sections for inelastic and other absorption.
    nuc->abs = nuc->abs + dAbs_balance;    // absorption=capture +  other absorption
    if(nuc->abs < 0){
        int NE = GetErgGridNum(nuc);
        nuc->abs = INTPLT_BY_POS_FR(nuc->XSS, nuc->inter_pos + 2 * NE, nuc->inter_frac); // absorption, E0
    }

    nuc->inel = sInel_balance;
    nuc->tot = nuc->abs + nuc->el + nuc->inel + nuc->fis;

    // Finally, get  total,absorption,elastic,fission
}