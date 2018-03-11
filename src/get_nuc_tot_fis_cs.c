//
// Created by xaq on 9/2/17.
//

#include "acedata.h"
#include "global_fun.h"


void
get_nuc_tot_fis_cs(acedata_t *obj,
                   nuclide_t *nuc,
                   nuclide_t *sab_nuc,
                   nuc_xs_t *cur_nuc_xs,
                   double erg,
                   double cell_tmp,
                   RNG_t *RNG)
{
    int NE = Get_erg_grid_num(nuc);

    /////////////////////// Cal NU /////////////////////////////
    cur_nuc_xs->nu = get_total_nu(nuc, erg);

    //////// binary search for Interpolation parameters /////////
    int min, max;
    min = 1;
    max = NE;

    cur_nuc_xs->inter_pos = get_intplt_pos_fr(nuc->XSS, erg, min, max, &cur_nuc_xs->inter_frac);

    int ptable = Get_loc_of_LUNR(nuc);
    cur_nuc_xs->ptable = 0;
    if(ptable) {
        if(erg > nuc->XSS[ptable + 6] && erg < nuc->XSS[ptable + 5 + (int) (nuc->XSS[ptable])]) {
            cur_nuc_xs->ptable = ptable;
            treat_URR(nuc, cur_nuc_xs, RNG, erg);
            return;
        }
    }

    //tot_cs:
    if(!sab_nuc) {
        cur_nuc_xs->tot = intplt_by_pos_fr(nuc->XSS, cur_nuc_xs->inter_pos + NE, cur_nuc_xs->inter_frac);
        cur_nuc_xs->fis = intplt_by_pos_fr(nuc->fis_XSS, cur_nuc_xs->inter_pos, cur_nuc_xs->inter_frac);

        /* 多普勒展宽总截面 */
        if(nuc->broaden_tmp == ZERO && nuc->broaden_tmp != cell_tmp) {
            double a, b, a2, f;
            int bi, j;
            a2 = nuc->atom_wgt * erg;
            if(a2 <= 500.0 * cell_tmp) {
                if(a2 >= 4.0 * cell_tmp)
                    f = HALF * cell_tmp / a2;
                else {
                    a = sqrt(a2 / cell_tmp);
                    b = 25.0 * a;
                    bi = (int) (b);
                    f = (obj->therm_func[bi] + (b - bi) * (obj->therm_func[bi + 1] - obj->therm_func[bi])) / a - ONE;
                }
                j = cur_nuc_xs->inter_pos + 3 * Get_erg_grid_num(nuc);
                cur_nuc_xs->tot += f * (nuc->XSS[j] + cur_nuc_xs->inter_frac * (nuc->XSS[j + 1] - nuc->XSS[j]));
            }
        }
        return;
    }

    ////////////////  sab case ///////////////////
    interpolate_sab(nuc, sab_nuc, cur_nuc_xs, erg);
}