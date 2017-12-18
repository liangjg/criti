//
// Created by xaq on 9/2/17.
//

#include "acedata.h"
#include "global_fun.h"

void get_nuc_abs_scatt_cs(acedata_t *obj, nuclide_t *nuc, double erg, int nInterpPos0, double dInterpFrac0,
                          int nInterpPos, double dInterpFrac){
    /* URR case: NucInterpCs[CurrentNUCLIDE][0,1,2,3,4] have been calculated in subroutine "TreatURR" */
//    if(obj->nucs[nNuc]->prob_table_flag == 1)
//        return;

    ///////// Norma case: interpolate SIG (a,el,inel) //////////
    int NE = GetErgGridNum(nuc);
    nuc->abs = INTPLT_BY_POS_FR(nuc->XSS, nInterpPos0 + 2 * NE, dInterpFrac0);  // absorption, E0
    nuc->el = INTPLT_BY_POS_FR(nuc->XSS, nInterpPos + 3 * NE, dInterpFrac); // elastic, Er
    nuc->inel = INTPLT_BY_POS_FR(nuc->inel_XSS, nInterpPos, dInterpFrac);    // inelastic, Er

    dppler_brdn_nuc_abs_scatt(obj, nuc, erg);
}

void dppler_brdn_nuc_abs_scatt(acedata_t *obj, nuclide_t *nuc, double erg){
    if(nuc->atom_wgt * erg <= 500.0 * nuc->broaden_tmp){ // thermal adjustment
        double a2, a, b;
        int bi;
        a2 = nuc->atom_wgt * erg / nuc->broaden_tmp;
        if(a2 >= 4.0)
            nuc->el = nuc->el * a2 / (a2 + 0.5);
        else{
            a = sqrt(a2);
            b = 25.0 * a;
            bi = (int) (b);
            nuc->el = nuc->el * a / (obj->therm_func[bi] + (b - bi) * (obj->therm_func[bi + 1] - obj->therm_func[bi]));
        }
    }
}