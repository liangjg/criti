//
//  Created by xaq on 10/27/17.
//

#include "geometry.h"


extern universe_t *root_universe;

cell_t *
locate_particle(particle_status_t *par_status,
                universe_t *start_univ,
                const double *pos,
                const double *dir)
{
    universe_t *univ, *lat_univ;
    cell_t *cell, *found_cell;
    int level, univ_sz, cell_sz;
    double local_pos_temp[3];
    double local_dir_temp[3];
    int i, j;

    for(i = 0; i < 3; i++) {
        local_pos_temp[i] = pos[i];
        local_dir_temp[i] = dir[i];
    }

    univ = start_univ;
    found_cell = NULL;

    /* 清空当前univs和cells，全部重新定位 */
    if(univ == root_universe)
        par_status->loc_sz = 0;
    univ_sz = par_status->loc_sz;
    cell_sz = par_status->loc_sz;
    level = par_status->loc_sz;

    while(1) {
        if(level++ >= 8) {
            puts("terminate locating particle because of too many levels (>8).");
            return NULL;
        }

        par_status->loc_univs[univ_sz++] = univ;

        if(univ->lattice_type) {    /* current universe has lattice structure */
            int lat_index = find_lat_index(univ, local_pos_temp, local_dir_temp);
            if(lat_index < 0) {
                printf("failed to locate particle, pos = %lf %lf %lf, dir = %lf %lf %lf\n", pos[0], pos[1], pos[2],
                       dir[0], dir[1], dir[2]);
                break;
            }

            par_status->loc_cells[cell_sz++] = lat_index;
            lat_univ = univ->filled_lat_univs[lat_index - 1];
            move_to_origin_lat(univ, lat_index, local_pos_temp);
            trans_univ_coord(lat_univ, local_pos_temp, local_dir_temp);
            univ = lat_univ;
        } else {    /* current universe has some cells and has no lattice */
            for(i = 0; i < univ->cells_sz; i++) {
                cell = univ->cells[i];
                if(particle_is_in_cell(cell, local_pos_temp, local_dir_temp)) {
                    par_status->loc_cells[cell_sz++] = i;
                    if(cell->fill) {    /* current cell has a universe filled in */
                        trans_univ_coord(cell->fill, local_pos_temp, local_dir_temp);
                        univ = cell->fill;
                        break;
                    } else {    /* current cell is a simple cell which has no fills */
                        for(j = 0; j < 3; j++) {
                            par_status->loc_pos[j] = local_pos_temp[j];
                            par_status->loc_dir[j] = local_dir_temp[j];
                        }
                        found_cell = cell;
                        goto END;
                    }
                }
            }
        }
    }

END:
    if(!found_cell)
        puts("failed to locate particle.");

    par_status->loc_sz = univ_sz;
    return found_cell;
}
