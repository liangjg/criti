//
// Created by xaq on 11/18/17.
//

#include "geometry.h"


void find_neighbor_cell(particle_state_t *par_state){
    double loc_pos[3], loc_dir[3];
    int lat_index;
    bool found = false;
    int level = par_state->bound_level;
    cell_t *found_cell = NULL;
    universe_t *univ = par_state->loc_univs[level];
    universe_t *filled_univ = NULL;

    par_state->loc_sz = level + 1;

    for(int i = 0; i < 3; i++){
        loc_pos[i] = par_state->loc_pos[i];
        loc_dir[i] = par_state->loc_dir[i];
    }

    if(univ->lattice_type){
        lat_index = offset_neighbor_lat(univ, par_state->loc_cells[level], par_state->lat_bound_surf, loc_pos);
        if(lat_index >= 0){
            par_state->loc_cells[level] = lat_index;
            filled_univ = univ->filled_lat_univs[lat_index - 1];
            trans_univ_coord(filled_univ, loc_pos, loc_dir);
            found_cell = locate_particle(par_state, filled_univ, loc_pos, loc_dir);
            if(found_cell) found = true;
        }
    }
    else{
        cell_t *cell, *neighbor_cell;
        int bound_index = par_state->bound_index;
        cell = univ->cells[par_state->loc_cells[level]];
        neighbor_cell = NULL;
        int neighbor_sz = cell->neighbor_lists_sz[bound_index];
        for(int i = 0; i < neighbor_sz; i++){
            neighbor_cell = cell->neighbor_lists[bound_index][i];
            if(particle_is_in_cell(neighbor_cell, loc_pos, loc_dir)){
                found = true;
                break;
            }
        }

        if(found){
            /* 不得已而为之，因为loc_cells存储的是当前universe中的第几个，而不是直接存储的cell_index */
            for(int i = 0; i < univ->cells_sz; i++)
                if(neighbor_cell == univ->cells[i]){
                    par_state->loc_cells[level] = i;
                    break;
                }

            found_cell = neighbor_cell;
            filled_univ = neighbor_cell->fill;
            if(filled_univ){    /* neighbor_cell is a complex cell with universe filled */
                trans_univ_coord(filled_univ, loc_pos, loc_dir);
                found_cell = locate_particle(par_state, filled_univ, loc_pos, loc_dir);
            }
        }
    }

    if(!found)
        found_cell = locate_particle(par_state, par_state->loc_univs[0], par_state->pos, par_state->dir);

    par_state->cell = found_cell;
}