//
// Created by xaq on 9/2/17.
//

#include "acedata.h"
#include "global_fun.h"


extern nuc_cs_t *nuc_cs_slave;

void get_nuc_abs_scatt_cs(acedata_t *obj, nuclide_t *nuc, double erg, int interp_pos0, double interp_frac0,
                          int interp_pos, double interp_frac){
    /* URR case: NucInterpCs[CurrentNUCLIDE][0,1,2,3,4] have been calculated in subroutine "TreatURR" */
//    if(obj->nucs[nNuc]->prob_table_flag == 1)
//        return;

    ///////// Norma case: interpolate SIG (a,el,inel) //////////
    int NE = Get_erg_grid_num(nuc);
    nuc_cs_t *cur_nuc_cs = &nuc_cs_slave[nuc->cs];
    cur_nuc_cs->abs = intplt_by_pos_fr(nuc->XSS, interp_pos0 + 2 * NE, interp_frac0);  // absorption, E0
    cur_nuc_cs->el = intplt_by_pos_fr(nuc->XSS, interp_pos + 3 * NE, interp_frac); // elastic, Er
    cur_nuc_cs->inel = intplt_by_pos_fr(nuc->inel_XSS, interp_pos, interp_frac);    // inelastic, Er

    /* 多普勒展宽吸收截面和散射截面 */
    if(nuc->atom_wgt * erg <= 500.0 * nuc->broaden_tmp){ // thermal adjustment
        double a2, a, b;
        int bi;
        a2 = nuc->atom_wgt * erg / nuc->broaden_tmp;
        if(a2 >= 4.0)
            cur_nuc_cs->el = cur_nuc_cs->el * a2 / (a2 + 0.5);
        else{
            a = sqrt(a2);
            b = 25.0 * a;
            bi = (int) (b);
            cur_nuc_cs->el = cur_nuc_cs->el * a / (obj->therm_func[bi] + (b - bi) * (obj->therm_func[bi + 1] - obj->therm_func[bi]));
        }
    }
}