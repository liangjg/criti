//
// Created by x1314aq on 17-12-13.
//

#include "acedata.h"
#include "map.h"
#include "IO_releated.h"
#include "cell.h"
#include "material.h"


extern map *base_cells;
extern map *base_nucs;
extern map *base_mats;
extern IOfp_t base_IOfp;
extern acedata_t base_acedata;
extern nuc_xs_t **base_nuc_xs;
extern int base_num_threads;

static int
_dppler_brdn_nuc_tmp(acedata_t *obj,
                     nuclide_t *nuc,
                     double tmp);

/***************************************************************************************************
 * 对所有nuclide进行多普勒展宽预处理
 * 如果所有cell都是相同的温度，那么一次将所有nuclide都展宽到该温度；
 * 如果某些cell有不同的温度，那么先将所有nuclide都展宽到0K温度，然后在输运过程中进一步根据cell->tmp进行进一步展宽
 ***************************************************************************************************/
void
doppler_broaden()
{
    mat_t *mat;
    nuclide_t *nuc;
    cell_t *cell;
    map_entry *cell_entry, *nuc_entry;
    int tot_nucs;
    int cnt;
    double broaden_tmp;
    bool first_time;    /* 是否所有cell都是相同的温度 */
    int i, k;

    map_iterator *nuc_iter = map_get_iter(base_nucs);
    map_iterator *cell_iter;

    cnt = 0;
    tot_nucs = 0;
    while((nuc_entry = map_iter_next(nuc_iter))) {
        nuc = nuc_entry->v.val;
        nuc->xs = tot_nucs++;
        nuc->broaden_tmp = nuc->tmp;
        if(!ISNUMBER(*nuc->id))
            continue;
        first_time = true;
        broaden_tmp = ZERO;
        cell_iter = map_get_iter(base_cells);
        while((cell_entry = map_iter_next(cell_iter))) {
            cell = cell_entry->v.val;
            mat = cell->mat;
            if(!mat)
                continue;
            for(k = 0; k < mat->tot_nuc_num; k++) {
                if(strcmp(nuc->id, ((nuclide_t *) mat->nucs[k])->id) == 0) {
                    if(first_time) {
                        broaden_tmp = cell->tmp;
                        first_time = false;
                    } else if(!EQ_ZERO(cell->tmp - broaden_tmp)) {
                        broaden_tmp = 0;
                        goto END;
                    }
                }
            }
        }
END:
        cnt += _dppler_brdn_nuc_tmp(&base_acedata, nuc, broaden_tmp);
        map_release_iter(cell_iter);
    }

    map_release_iter(nuc_iter);

    fputs("===================== Cross-section doppler broaden ====================\n", base_IOfp.mat_fp);
    fprintf(base_IOfp.mat_fp, "Doppler broaden applied to %d nuclide.\n", cnt);
    fputs("========================================================================\n", base_IOfp.mat_fp);

    /* 分配空间 */
    base_nuc_xs = malloc(base_num_threads * sizeof(void *));
    for(i = 0; i < base_num_threads; i++)
        base_nuc_xs[i] = malloc(tot_nucs * sizeof(nuc_xs_t));
}

int
_dppler_brdn_nuc_tmp(acedata_t *obj,
                     nuclide_t *nuc,
                     double tmp)
{
    int i, j;
    double a, b, f1, f2;

    calc_therm_Gfun(obj);

    if(fabs(nuc->tmp - tmp) <= 0.01 * tmp)
        return 0;   // no adjustment
    b = 500.0 * fabs(tmp - nuc->tmp) / nuc->atom_wgt;
    for(j = 1; j <= Get_erg_grid_num(nuc); j++) {
        if(nuc->XSS[j] > b)
            break;
        f1 = ONE;
        if(!EQ_ZERO(nuc->tmp)) {
            a = sqrt(nuc->atom_wgt * nuc->XSS[j] / nuc->tmp);
            if(a >= TWO)
                f1 = ONE + HALF / (a * a);
            i = (int) (a / 0.04);
            if(a < TWO)
                f1 = (obj->therm_func[i] + (a / 0.04 - i) * (obj->therm_func[i + 1] - obj->therm_func[i])) / a;
        }
        f2 = ONE;
        if(!EQ_ZERO(tmp)) {
            a = sqrt(nuc->atom_wgt * nuc->XSS[j] / tmp);
            if(a >= TWO)
                f2 = ONE + HALF / (a * a);
            i = (int) (a / .04);
            if(a < TWO)
                f2 = (obj->therm_func[i] + (a / 0.04 - i) * (obj->therm_func[i + 1] - obj->therm_func[i])) / a;
        }
        a = nuc->XSS[j + 3 * Get_erg_grid_num(nuc)] * (f2 - f1) / f1;

        nuc->XSS[j + Get_erg_grid_num(nuc)] = nuc->XSS[j + Get_erg_grid_num(nuc)] + a;
        nuc->XSS[j + 3 * Get_erg_grid_num(nuc)] = nuc->XSS[j + 3 * Get_erg_grid_num(nuc)] + a;
    }
    nuc->broaden_tmp = tmp;
    return 1;
}
