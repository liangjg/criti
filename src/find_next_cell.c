//
// Created by xaq on 11/14/17.
//

#include "geometry.h"


#define REFLECTIVE    1
#define CROSSING      0

void
find_next_cell(particle_status_t *par_status)
{
    universe_t *univ;
    cell_t *cell;
    surface_t *surf;
    void *prev_mat;
    double prev_cell_tmp;
    int level = par_status->bound_level;

    univ = par_status->loc_univs[level];
    prev_mat = par_status->mat;
    prev_cell_tmp = par_status->cell_tmp;

    if(univ->lattice_type)
        find_neighbor_cell(par_status);
    else {
        cell = univ->cells[par_status->loc_cells[level]];
        surf = cell->surfs_addr[par_status->bound_index];

        switch(surf->bc) {
            case CROSSING:find_neighbor_cell(par_status);
                break;
            case REFLECTIVE: {
                /* TODO: 当cell为凹多边形时，可能存在假的反射面，这时应该继续输运而不进行反射 */
                reflect_par(surf, par_status->pos, par_status->dir, par_status->loc_dir);
                break;
            }
        }
    }

    if(!par_status->cell) {
        par_status->is_killed = true;
        return;
    }

    cell = par_status->cell;
    if(cell->imp == 0)
        par_status->is_killed = true;
    par_status->mat = cell->mat;
    par_status->cell_tmp = cell->tmp;
    par_status->mat_changed = (prev_mat != par_status->mat);
    par_status->cell_tmp_changed = !EQ_ZERO(prev_cell_tmp - par_status->cell_tmp);
}