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
    universe_t *univ;
    cell_t *cell;
    int level, univ_index, cell_index, found_cell;
    int filled_univ, lat_univ;
    double local_pos_temp[3];
    double local_dir_temp[3];

    for(int i = 0; i < 3; i++){
        local_pos_temp[i] = pos[i];
        local_dir_temp[i] = dir[i];
    }

    univ_index = 0;
    found_cell = -1;
    level = 0;

    /* 清空当前univs和cells，全部重新定位 */
    vector_clear(&par_state->loc_univs);
    vector_clear(&par_state->loc_cells);

    while(1){
        if(++level > 99){
            puts("terminate locating particle because of too many levels (>99).");
            return -1;
        }

        univ = (universe_t *) map_get(base_univs, univ_index);
        vector_push_back(&par_state->loc_univs, &univ_index);

        if(univ->is_lattice){    /* current universe has lattice structure */
            int lat_index = find_lat_index(univ, local_pos_temp, local_dir_temp);
            if(lat_index < 0){
                printf("failed to locate particle, pos = %lf %lf %lf, dir = %lf %lf %lf", pos[0], pos[1], pos[2],
                       dir[0], dir[1], dir[2]);
                break;
            }

            vector_push_back(&par_state->loc_cells, &lat_index);
            lat_univ = univ->filled_lat_univs[lat_index];
            move_to_origin_lat(univ, lat_index, local_pos_temp);
            trans_univ_coord((universe_t *) map_get(base_univs, lat_univ), local_pos_temp, local_dir_temp);
            univ_index = lat_univ;

        } else{    /* current universe has some cells and has no lattice */
            for(size_t i = 0; i < vector_size(&univ->cells); i++){
                cell_index = *(int *) vector_at(&univ->cells, i);
                cell = (cell_t *) map_get(base_cells, cell_index);

                if(particle_is_in_cell(cell, local_pos_temp, local_dir_temp)){
                    vector_push_back(&par_state->loc_cells, &cell_index);
                    if(cell->fill < 0){    /* current cell is a simple cell which has no fills */
                        for(int j = 0; j < 3; j++){
                            par_state->loc_pos[j] = local_pos_temp[j];
                            par_state->loc_dir[j] = local_dir_temp[j];
                        }
                        found_cell = cell_index;
                        break;
                    } else{    /* current cell has a universe filled in */
                        filled_univ = cell->fill;
                        trans_univ_coord((universe_t *) map_get(base_univs, filled_univ), local_pos_temp,
                                         local_dir_temp);
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
