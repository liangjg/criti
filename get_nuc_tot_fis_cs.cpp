//
// Created by xaq on 9/2/17.
//

#include "acedata.h"
#include "global_fun.h"

void get_nuc_tot_fis_cs(acedata_t *obj, int nNuc, int nSabNuc, double dEg0, double dCell_T, bool bUseProbTable)
{
    enum eAdjustCsByPTOrNot { NotAdjustCsByPT = 0, AdjustCsByPT = 1};

    int nNE = GetErgGridNum(obj, nNuc);

    /////////////////////// Cal NU /////////////////////////////
    obj->nucs[nNuc]->nu = get_total_nu(obj, dEg0, nNuc);

    //////// binary search for Interpolation parameters /////////
    int min, max;
//    if(obj->p_bUseErgBinMap) {
//        GetErgBinMapPtr(obj, nNuc, min, max);
//    } else {
//        min = 1 ;
//        max = GetErgGridNum(obj, nNuc);
//    }
    min = 1;
    max = GetErgGridNum(obj, nNuc);

    get_intplt_pos_fr_double(obj->nucs[nNuc]->XSS, dEg0, min, max, &obj->nucs[nNuc]->inter_pos, &obj->nucs[nNuc]->inter_frac);

    if(bUseProbTable) {
        obj->nucs[nNuc]->prob_table_flag = NotAdjustCsByPT;
        int nL = GetLocOfLUNR(obj, nNuc);
        if(nL != 0) { // probability tables exist
            if(dEg0 > obj->nucs[nNuc]->XSS[nL + 6] && dEg0 < obj->nucs[nNuc]->XSS[nL + 5 + int(obj->nucs[nNuc]->XSS[nL])]) {
                treat_URR(obj, nNuc, dEg0);
                obj->nucs[nNuc]->prob_table_flag = AdjustCsByPT;
                return;
            }
        }
    }

    //tot_cs:
    if(nSabNuc == 0) {
        obj->nucs[nNuc]->tot = INTPLT_BY_POS_FR(obj->nucs[nNuc]->XSS, obj->nucs[nNuc]->inter_pos + nNE, obj->nucs[nNuc]->inter_frac);
        obj->nucs[nNuc]->fis = INTPLT_BY_POS_FR(obj->nucs[nNuc]->fis_XSS, obj->nucs[nNuc]->inter_pos, obj->nucs[nNuc]->inter_frac);

        dpplr_brdn_nuc_tot_cs(obj, nNuc, dCell_T, dEg0);
        return;
    }

    ////////////////  sab case ///////////////////
    interpolate_sab(obj, nNuc, nSabNuc, dEg0);
}

void dpplr_brdn_nuc_tot_cs(acedata_t *obj, int nNuc, double dCell_T, double dEg0)
{
    double a, b, a2, f;
    int bi, j;
    if(obj->nucs[nNuc]->broaden_tmp == 0.  && obj->nucs[nNuc]->broaden_tmp != dCell_T) { //th_ad:  thermal adjustment
        a2 = obj->nucs[nNuc]->atom_wgt * dEg0;
        if(a2 <= 500.0 * dCell_T) {
            if(a2 >= 4.0 * dCell_T)
                f = 0.5 * dCell_T / a2;
            else {
                a = sqrt(a2 / dCell_T);
                b = 25.*a;
                bi = int(b);
                f = (obj->therm_func[bi] + (b - bi) * (obj->therm_func[bi + 1] - obj->therm_func[bi])) / a - 1.;
            }
            j = obj->nucs[nNuc]->inter_pos + 3 * GetErgGridNum(obj, nNuc);
            obj->nucs[nNuc]->tot = obj->nucs[nNuc]->tot + f * (obj->nucs[nNuc]->XSS[j] + obj->nucs[nNuc]->inter_frac * (obj->nucs[nNuc]->XSS[j + 1] - obj->nucs[nNuc]->XSS[j]));
        }
    }
}