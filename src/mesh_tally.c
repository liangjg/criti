//
// Created by x1314aq on 18-6-3.
//

#include "mesh_tally.h"


struct pair_t {
    int index;      /* 在整个mesh中的下标 */
    double tklen;   /* 在该下标处的径迹长度 */
};

struct mesh_pair_t {
    struct pair_t *pairs;
    int sz;
};

static int
_get_mesh_index(const double *,
                mesh_tally_t *);

static void
_get_mesh_pair(const double *,
               const double *,
               double,
               mesh_tally_t *,
               struct mesh_pair_t *);

void
mtally_flux(mesh_tally_t *obj,
            particle_status_t *par_status,
            double tklen)
{
    int i, j, index;
    double cur_tklen;
    struct mesh_pair_t mesh_pair;

    _get_mesh_pair(par_status->pos, par_status->dir, tklen, obj, &mesh_pair);

    for(i = 0; i < mesh_pair.sz; i++) {
        index = mesh_pair.pairs[i].index;
        cur_tklen = mesh_pair.pairs[i].tklen;
        for(j = 0; j < obj->erg_bin_sz; j++)
            if(par_status->erg < obj->erg_bin[j])
                break;
        obj->result[index][j] += cur_tklen * par_status->wgt;
    }
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

void
_get_mesh_pair(const double *pos,
               const double *dir,
               double tklen,
               mesh_tally_t *obj,
               struct mesh_pair_t *mesh_pair)
{
    double new_pos[3];
    int index[3], new_index[3], cross_num;
    int i;

    new_pos[0] = pos[0] + dir[0] * tklen;
    new_pos[1] = pos[1] + dir[1] * tklen;
    new_pos[2] = pos[2] + dir[2] * tklen;
    cross_num = 0;

    for(i = 0; i < 3; i++) {
        if(obj->scope[i] == 1) {
            index[i] = 0;
            new_index[i] = 0;
            continue;
        }

        index[i] = (int) (floor((pos[i] - obj->bound_min[i]) / obj->delta[i]));
        new_index[i] = (int) (floor((new_pos[i] - obj->bound_min[i]) / obj->delta[i]));
        cross_num += abs(index[i] - new_index[i]) ;
    }

    if(cross_num == 0) {

    }
}