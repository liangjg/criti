//
// Created by xaq on 11/18/17.
//

#include "geometry.h"


extern map *base_univs;

void find_neighbor_cell(particle_state_t *par_state){
    double loc_pos[3], loc_dir[3];
    bool found = false;
    int neighbor_cell_index = -1;
    int level = par_state->bound_level;
    int univ_index = *(int *)vector_at(&par_state->loc_univs, level);
    int bound_surf = par_state->surf;
    universe_t *univ = (universe_t *) map_get(base_univs, univ_index);

    for(int i = 0; i < 3; i++){
        loc_pos[i] = par_state->pos[i];
        loc_dir[i] = par_state->dir[i];
    }

    if(univ->is_lattice){
        int lat_index = offset_neighbor_lat(univ, *(int *)vector_at(&par_state->loc_cells, level), par_state->surf, loc_pos);
        if(lat_index >= 0){
            *(int *)vector_at(&par_state->loc_cells, level) = lat_index;
            int filled_univ = univ->filled_lat_univs[lat_index - 1];
            trans_univ_coord((universe_t *) map_get(base_univs, filled_univ), loc_pos, loc_dir);
            neighbor_cell_index = locate_particle(par_state, loc_pos, loc_dir);
        }
    } else{
        int current_cell = *(int *)vector_at(&par_state->loc_cells, level);
        map *val = (map *) map_get(univ->neighbor_lists, current_cell);
        cell_t *neighbor_cell = (cell_t *) map_get(val, bound_surf);
        if(particle_is_in_cell(neighbor_cell, loc_pos, loc_dir))
            found = true;

        if(found){
            *(int *)vector_at(&par_state->loc_cells, level) = neighbor_cell->id;
            int filled_univ = neighbor_cell->fill;
            if(filled_univ > 0){    /* neighbor_cell is a complex cell with universe filled */
                universe_t *filled_universe = (universe_t *)map_get(base_univs, filled_univ);
                trans_univ_coord(filled_universe, loc_pos, loc_dir);
                neighbor_cell_index = locate_particle(par_state, loc_pos, loc_dir);
            }
        }
    }

    if(!found)
        neighbor_cell_index = locate_particle(par_state, loc_pos, loc_dir);

    par_state->cell = neighbor_cell_index;
}