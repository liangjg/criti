//
// Created by x1314aq on 18-6-3.
//

#include "mesh_tally.h"


static int
_get_mesh_index(const double *,
                mesh_tally_t *);

void
mtally_flux(mesh_tally_t *obj,
            particle_status_t *par_status,
            double tklen)
{
    int index, i;
    index = _get_mesh_index(par_status->pos, obj);

    if(index == -1) return;

    for(i = 0; i < obj->erg_bin_sz; i++)
        if(par_status->erg < obj->erg_bin[i])
            break;
    obj->result[index][i] += tklen * par_status->wgt;
}

void
mtally_power(mesh_tally_t *obj,
             particle_status_t *par_status,
             double tklen)
{

}

void
mtally_fis_rate(mesh_tally_t *obj,
                particle_status_t *par_status,
                double tklen)
{

}

void
mtally_abs_rate(mesh_tally_t *obj,
                particle_status_t *par_status,
                double tklen)
{

}

void
mesh_tally_free(mesh_tally_t *obj)
{
    int mesh = 1;
    int i;

    for(i = 0; i < 3; i++)
        mesh *= obj->scope[i];

    for(i = 0; i < mesh; i++)
        free(obj->result[i]);

    free(obj->result);
    free(obj->erg_bin);
}

int
_get_mesh_index(const double *pos,
                mesh_tally_t *obj)
{
    int index[3];
    int i;

    for(i = 0; i < 3; i++) {
        if(obj->scope[i] == 1) {
            index[i] = 0;
            continue;
        }
        if(pos[i] < obj->bound_min[i] || pos[i] > obj->bound_max[i])
            return -1;

        index[i] = (int) ((pos[i] - obj->bound_min[i]) / obj->delta[i]);
    }

    return index[0] + index[1] * obj->scope[0] + index[2] * obj->scope[0] * obj->scope[1];
}