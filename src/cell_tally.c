//
// Created by x1314aq on 18-5-27.
//

#include "cell_tally.h"


static bool
_match(cell_tally_t *,
       particle_status_t *,
       int);

void
ctally_flux(cell_tally_t *obj,
           particle_status_t *par_status,
           double tklen)
{
    int i;
    double res = par_status->wgt * tklen;
    for(i = 0; i < obj->cells_sz; i++) {
        if(_match(obj, par_status, i)) {
            /* 如果存在能量分箱 */
            if(obj->erg_bin_sz) {
                int j;
                int sz = obj->erg_bin_sz;
                for(j = 0; j < sz; j++) {
                    if(par_status->erg < obj->erg_bin[j]) {
                        obj->result[i][j] += res;
                        return;
                    }
                }
                obj->result[i][sz] += res;
            } else
                obj->result[i][0] += res;
            return;
        }
    }
}

void
ctally_power(cell_tally_t *obj,
             particle_status_t *par_status,
             double tklen)
{

}

void
ctally_fis_rate(cell_tally_t *obj,
                particle_status_t *par_status,
                double tklen)
{

}

void
ctally_abs_rate(cell_tally_t *obj,
                particle_status_t *par_status,
                double tklen)
{

}

void
cell_tally_free(cell_tally_t *obj)
{
    int i;

    free(obj->erg_bin);
    for(i = 0; i < obj->cells_sz; i++) {
        free(obj->cells[i]);
        free(obj->result[i]);
    }
    free(obj->cells);
    free(obj->result);
}

bool
_match(cell_tally_t *obj,
       particle_status_t *par_status,
       int index)
{
    /* 如果不存在filter */
    if(!obj->filter) {
        if(obj->vec_sz != par_status->loc_sz) return false;
        int sz = par_status->loc_sz;
        return memcmp(obj->cells[index], par_status->loc_cells, sz * sizeof(int)) == 0;
    }

    /* ***************************************************
     * 如果存在filter，那么只需要比较obj->cells[index]和
     * par_status->loc_cells的前obj->vec_sz位即可，注意要忽略
     * obj->filter中为0的位
     * ***************************************************/
    int i;
    for(i = 0; i < obj->vec_sz; i++) {
        if(obj->filter[i] == 0) continue;
        if(obj->cells[index][i] != par_status->loc_cells[i]) return false;
    }
    return true;
}
