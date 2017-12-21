//
// Created by xaq on 11/18/17.
//

#include "geometry.h"


extern map *base_univs;

void find_neighbor_cell(particle_state_t *par_state){
    double loc_pos[3], loc_dir[3];
    int lat_index, filled_univ, cell_index;
    bool found = false;
    int neighbor_cell_index = -1;
    int level = par_state->bound_level;
    int univ_index = *(int *)vector_at(&par_state->loc_univs, level);
    int bound_surf = par_state->surf;
    universe_t *univ = (universe_t *) map_get(base_univs, univ_index);

    vector_resize(&par_state->loc_univs, level + 1);
    vector_resize(&par_state->loc_cells, level + 1);

    for(int i = 0; i < 3; i++){
        loc_pos[i] = par_state->loc_pos[i];
        loc_dir[i] = par_state->loc_dir[i];
    }

    if(univ->is_lattice){
        lat_index = offset_neighbor_lat(univ, *(int *)vector_at(&par_state->loc_cells, level), par_state->lat_bound_surf, loc_pos);
        if(lat_index >= 0){
            *(int *)vector_at(&par_state->loc_cells, level) = lat_index;
            filled_univ = univ->filled_lat_univs[lat_index - 1];
            trans_univ_coord((universe_t *) map_get(base_univs, filled_univ), loc_pos, loc_dir);
            neighbor_cell_index = locate_particle(par_state, filled_univ, loc_pos, loc_dir);
        }
    } else{
        cell_index = *(int *)vector_at(&univ->cells, *(int *)vector_at(&par_state->loc_cells, level));
        map *val = (map *) map_get(univ->neighbor_lists, cell_index);
        cell_t *neighbor_cell = (cell_t *) map_get(val, bound_surf);
        if(neighbor_cell && particle_is_in_cell(neighbor_cell, loc_pos, loc_dir))
            found = true;

        if(found){
            /* 不得已而为之，因为loc_cells存储的是当前universe中的第几个，而不是直接存储的cell_index */
            for(int i = 0; i < vector_size(&univ->cells); i++)
                if(neighbor_cell->id == *(int *)vector_at(&univ->cells, i)){
                    *(int *)vector_at(&par_state->loc_cells, level) = i;
                    break;
                }

            neighbor_cell_index = neighbor_cell->id;
            filled_univ = neighbor_cell->fill;
            if(filled_univ > 0){    /* neighbor_cell is a complex cell with universe filled */
                universe_t *filled_universe = (universe_t *)map_get(base_univs, filled_univ);
                trans_univ_coord(filled_universe, loc_pos, loc_dir);
                neighbor_cell_index = locate_particle(par_state, filled_univ, loc_pos, loc_dir);
            }
        }
    }

    if(!found)
        neighbor_cell_index = locate_particle(par_state, 0, par_state->pos, par_state->dir);

    par_state->cell = neighbor_cell_index;
}