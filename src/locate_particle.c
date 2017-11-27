//
//  locate_particle.c
//  ttyw
//
//  Created by 叶鑫 on 2017/10/27.
//

#include "geometry.h"


extern map *base_univs;
extern map *base_cells;
extern map *base_surfs;

int locate_particle(particle_state_t *par_state, const double pos[3], const double dir[3]){
    int univ_index = 0;
    int found_cell = -1;
    double local_pos_temp[3];
    double local_dir_temp[3];

    for(int i = 0; i < 3; i++){
        local_pos_temp[i] = pos[i];
        local_dir_temp[i] = dir[i];
    }

    int level = 0;

    universe_t *univ;
    cell_t *cell;

    while(true){
        if(++level > 99){
            puts("terminate locating particle because of too many levels (>99).");
            break;
        }

        univ = (universe_t *) map_get(base_univs, univ_index);

        if(univ->is_lattice){
            /* TODO: initiate par_state->loc_univs */
            par_state->loc_univs[par_state->loc_univs_sz++] = univ_index;

            int lat_index = find_lat_index(univ, local_pos_temp, local_dir_temp);
            if(lat_index < 0) break;

            /* TODO: initiate par_state->loc_cells */
            par_state->loc_cells[par_state->loc_cells_sz++] = lat_index;

            int lat_univ = univ->fill_lat_universe[lat_index];

            move_to_origin_lat(univ, lat_index, local_pos_temp);
            trans_univ_coord((universe_t *) map_get(base_univs, lat_univ), local_pos_temp, local_dir_temp);

            univ_index = lat_univ;

        } else{
            /* TODO: */
            par_state->loc_univs[par_state->loc_univs_sz++] = univ_index;
            for(size_t i = 0; i < vector_size(&univ->fill_cells); i++){
                int cell_index = *(int *)vector_at(&univ->fill_cells, i);

                cell = (cell_t *)map_get(base_cells, cell_index);

                if(particle_is_in_cell(cell, local_pos_temp, local_dir_temp)){
                    par_state->loc_cells[par_state->loc_cells_sz++] = cell_index;

                    if(cell->fill < 0){
                        for(int j = 0; j < 3; j++){
                            par_state->loc_pos[j] = local_pos_temp[j];
                            par_state->loc_dir[j] = local_dir_temp[j];
                        }
                        found_cell = cell_index;
                        break;
                    }
                    else{
                        int filled_univ = cell->fill;
                        trans_univ_coord(map_get(base_univs, filled_univ), local_pos_temp, local_dir_temp);
                        univ_index = filled_univ;
                    }
                }
            }
        }
    }

    if(found_cell == -1)
        puts("failed to locate particle.");

    return found_cell;
}
