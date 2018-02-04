//
// Created by xaq on 11/14/17.
//

#include "geometry.h"


#define REFLECTIVE    1
#define CROSSING      0

void find_next_cell(particle_state_t *par_state){
    universe_t *univ;
    cell_t *cell;
    surface_t *surf;
    void *prev_mat;
    double prev_cell_tmp;
    int level = par_state->bound_level;

    univ = par_state->loc_univs[level];
    prev_mat = par_state->mat;
    prev_cell_tmp = par_state->cell_tmp;

    if(univ->lattice_type)
        find_neighbor_cell(par_state);
    else{
        cell = univ->cells[par_state->loc_cells[level]];
        surf = cell->surfs_addr[par_state->bound_index];

        switch(surf->bc){
            case CROSSING:
                find_neighbor_cell(par_state);
                break;
            case REFLECTIVE:{
                /* TODO: 当cell为凹多边形时，可能存在假的反射面，这时应该继续输运而不进行反射 */
                reflect_par(surf, par_state->pos, par_state->dir, par_state->loc_dir);
                break;
            }
        }
    }

    if(!par_state->cell){
        par_state->is_killed = true;
        return;
    }

    cell = par_state->cell;
    if(cell->imp == 0)
        par_state->is_killed = true;
    par_state->mat = cell->mat;
    par_state->cell_tmp = cell->tmp;
    par_state->mat_changed = (prev_mat != par_state->mat);
    par_state->cell_tmp_changed = !EQ_ZERO(prev_cell_tmp - par_state->cell_tmp);
}