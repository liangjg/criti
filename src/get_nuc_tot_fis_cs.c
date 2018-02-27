//
// Created by xaq on 9/2/17.
//

#include "acedata.h"
#include "global_fun.h"


void get_nuc_tot_fis_cs(acedata_t *obj, nuclide_t *nuc, nuclide_t *sab_nuc, nuc_cs_t *cur_nuc_cs, double erg,
                        double cell_tmp){

    int NE = Get_erg_grid_num(nuc);
    cur_nuc_cs->nu = get_total_nu(nuc, erg);

    int min, max;
    min = 1;
    max = NE;

    cur_nuc_cs->inter_pos = get_intplt_pos_fr(nuc->XSS, erg, min, max, &cur_nuc_cs->inter_frac);

    if(!sab_nuc){
        cur_nuc_cs->tot = intplt_by_pos_fr(nuc->XSS, cur_nuc_cs->inter_pos + NE, cur_nuc_cs->inter_frac);
        cur_nuc_cs->fis = intplt_by_pos_fr(nuc->fis_XSS, cur_nuc_cs->inter_pos, cur_nuc_cs->inter_frac);

        /* 多普勒展宽总截面 */
        if(nuc->broaden_tmp == ZERO && nuc->broaden_tmp != cell_tmp){ //th_ad:  thermal adjustment
            double a, b, a2, f;
            int bi, j;
            a2 = nuc->atom_wgt * erg;
            if(a2 <= 500.0 * cell_tmp){
                if(a2 >= 4.0 * cell_tmp)
                    f = HALF * cell_tmp / a2;
                else{
                    a = sqrt(a2 / cell_tmp);
                    b = 25. * a;
                    bi = (int) (b);
                    f = (obj->therm_func[bi] + (b - bi) * (obj->therm_func[bi + 1] - obj->therm_func[bi])) / a - ONE;
                }
                j = cur_nuc_cs->inter_pos + 3 * Get_erg_grid_num(nuc);
                cur_nuc_cs->tot += f * (nuc->XSS[j] + cur_nuc_cs->inter_frac * (nuc->XSS[j + 1] - nuc->XSS[j]));
            }
        }
        return;
    }

    interpolate_sab(nuc, sab_nuc, cur_nuc_cs, erg);
}