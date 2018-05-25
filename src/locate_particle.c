//
//  Created by xaq on 10/27/17.
//

#include "geometry.h"


extern universe_t *root_universe;

/* 如果start_univ为NULL，则表示从root_universe开始定位 */
cell_t *
locate_particle(particle_status_t *par_status,
                universe_t *start_univ,
                const double *pos,
                const double *dir)
{
    universe_t *univ, *lat_univ;
    cell_t *cell;
    int univ_sz, cell_sz;
    double local_pos[3];
    double local_dir[3];
    int i, j;

    for(i = 0; i < 3; i++) {
        local_pos[i] = pos[i];
        local_dir[i] = dir[i];
    }

    univ = start_univ;
    /* 清空当前univs和cells，全部重新定位 */
    if(!univ) {
        univ = root_universe;
        par_status->loc_sz = 0;
    }
    univ_sz = par_status->loc_sz;
    cell_sz = par_status->loc_sz;

    /* 避免缓冲区溢出，因为par_status->loc_univs的大小为8 */
    while(univ_sz < 8) {
        par_status->loc_univs[univ_sz++] = univ;

        if(univ->lattice_type) {    /* current universe has lattice structure */
            int lat_index = find_lat_index(univ, local_pos, local_dir);
            if(lat_index < 0) {
                printf("failed to locate particle, pos = %lf %lf %lf, dir = %lf %lf %lf\n",
                       pos[0], pos[1], pos[2], dir[0], dir[1], dir[2]);
                return NULL;
            }

            par_status->loc_cells[cell_sz++] = lat_index;
            lat_univ = univ->filled_lat_univs[lat_index - 1];
            move_to_origin_lat(univ, lat_index, local_pos);
            trans_univ_coord(lat_univ, local_pos, local_dir);
            univ = lat_univ;
        } else {    /* current universe has some cells and has no lattice */
            for(i = 0; i < univ->cells_sz; i++) {
                cell = univ->cells[i];
                if(particle_is_in_cell(cell, local_pos, local_dir)) {
                    par_status->loc_cells[cell_sz++] = i;
                    if(cell->fill) {    /* current cell has a universe filled in */
                        trans_univ_coord(cell->fill, local_pos, local_dir);
                        univ = cell->fill;
                        break;
                    } 
                    /* current cell is a simple cell which has no fills */
                    for(j = 0; j < 3; j++) {
                        par_status->loc_pos[j] = local_pos[j];
                        par_status->loc_dir[j] = local_dir[j];
                    }
                    par_status->loc_sz = univ_sz;
                    return cell;
                }
            }
        }
    }

    puts("terminate locating particle because of too many levels (>8).");
    return NULL;
}
