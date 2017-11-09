//
// Created by xaq on 9/2/17.
//

#include "acedata.h"
#include "global_fun.h"

void dppler_brdn_nuc_abs_scatt(acedata_t *obj, int nNuc, double dEg0)
{
    if(obj->nucs[nNuc]->atom_wgt * dEg0 <= 500.*obj->nucs[nNuc]->broaden_tmp) { // thermal adjustment
        double a2, a, b;
        int bi;
        a2 = obj->nucs[nNuc]->atom_wgt * dEg0 / obj->nucs[nNuc]->broaden_tmp;
        if(a2 >= 4.0)
            obj->nucs[nNuc]->el = obj->nucs[nNuc]->el * a2 / (a2 + 0.5);
        else {
            a = sqrt(a2);
            b = 25.0 * a;
            bi = int(b);
            obj->nucs[nNuc]->el = obj->nucs[nNuc]->el * a / (obj->therm_func[bi] + (b - bi) * (obj->therm_func[bi + 1] - obj->therm_func[bi]));
        }
    }
}

void get_nuc_abs_scatt_cs(acedata_t *obj, int nNuc, double dEg0, int nInterpPos0, double dInterpFrac0, int nInterpPos, double dInterpFrac)
{
    ////// URR case: NucInterpCs[CurrentNUCLIDE][0,1,2,3,4] have been calculated in subroutine "TreatURR"/////
    if(obj->nucs[nNuc]->prob_table_flag == 1)
        return;

    ///////// Norma case: interpolate SIG (a,el,inel) //////////
    int nNE = GetErgGridNum(obj, nNuc);
    obj->nucs[nNuc]->abs = INTPLT_BY_POS_FR(obj->nucs[nNuc]->XSS, nInterpPos0 + 2 * nNE, dInterpFrac0);  // absorption, E0
    obj->nucs[nNuc]->el = INTPLT_BY_POS_FR(obj->nucs[nNuc]->XSS, nInterpPos + 3 * nNE, dInterpFrac); // elastic, Er
    obj->nucs[nNuc]->inel = INTPLT_BY_POS_FR(obj->nucs[nNuc]->inel_XSS, nInterpPos, dInterpFrac);    // inelastic, Er

    dppler_brdn_nuc_abs_scatt(obj, nNuc, dEg0);
}