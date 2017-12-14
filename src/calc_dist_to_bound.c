//
// Created by xaq on 11/14/17.
//

#include "geometry.h"


extern map *base_cells;
extern map *base_univs;
extern map *base_surfs;

#define OVERLAP_ERR    1.0E-08
#define ZERO_DIST      1.0E-10

double calc_dist_to_bound(particle_state_t *par_state){
    universe_t *univ;
    cell_t *cell;
    surface_t *surf;
    double loc_pos[3];
    double loc_dir[3];
    int loc_univ;
    int signed_surf_index, surf_index, lat_index, lat_bound_surf;
    int univ_cell_index, cell_index;
    double dist_min, distance;
    dist_min = 1.0E+22;
    int at_surf = abs(par_state->surf);
    par_state->surf = 0;
    par_state->lat_bound_surf = -1;

    cell = (cell_t *) map_get(base_cells, par_state->cell);
    if(cell->is_inner_cell){
        for(int i = 0; i < vector_size(&par_state->loc_univs); i++){
            loc_univ = *(int *)vector_at(&par_state->loc_univs, i);
            univ = (universe_t *) map_find(base_univs, loc_univ);
            trans_univ_dir(univ, par_state->loc_dir);
        }

        for(size_t i = 0; i < vector_size(&cell->surfs); i++){
            signed_surf_index = *(int *)vector_at(&cell->surfs, i);
            surf_index = abs(signed_surf_index);
            surf = (surface_t *)map_get(base_surfs, surf_index);
            distance = calc_dist_to_surf(surf, par_state->loc_pos, par_state->loc_dir, surf_index == at_surf);

            if(distance > ZERO_DIST && dist_min - distance > OVERLAP_ERR){
                dist_min = distance;
                par_state->surf = signed_surf_index;
                par_state->bound_index = i;
            }
        }
        par_state->lat_bound_surf = 0;
        par_state->bound_level = vector_size(&par_state->loc_cells) - 1;
    } else{
        for(int i = 0; i < 3; i++){
            loc_pos[i] = par_state->pos[i];
            loc_dir[i] = par_state->dir[i];
        }

        for(size_t i = 0; i < vector_size(&par_state->loc_cells); i++){
            loc_univ = *(int *)vector_at(&par_state->loc_univs, i);
            univ = (universe_t *)map_get(base_univs, loc_univ);
            trans_univ_coord(univ, loc_pos, loc_dir);

            if(univ->is_lattice){
                lat_index = *(int *)vector_at(&par_state->loc_cells, i);
                move_to_origin_lat(univ, lat_index, loc_pos);
                distance = calc_dist_to_lat(univ, loc_pos, loc_dir, &lat_bound_surf);
                if(distance < ZERO_DIST)
                    break;

                if(dist_min - distance > OVERLAP_ERR){
                    dist_min = distance;
                    par_state->bound_level = i;
                    par_state->lat_bound_surf = lat_bound_surf;
                    par_state->surf = 0;
                    for(int j = 0; j < 3; j++){
                        par_state->loc_pos[j] = loc_pos[j];
                        par_state->loc_dir[j] = loc_dir[j];
                    }
                }
                continue;
            }

            univ_cell_index = *(int *)vector_at(&par_state->loc_cells, i);
            cell_index = *(int *)vector_at(&univ->cells, univ_cell_index);
            cell = (cell_t *)map_get(base_cells, cell_index);

            for(size_t j = 0; j < vector_size(&cell->surfs); j++){
                signed_surf_index = *(int *)vector_at(&cell->surfs, j);
                surf_index = abs(signed_surf_index);
                surf = (surface_t *)map_get(base_surfs, surf_index);
                distance = calc_dist_to_surf(surf, loc_pos, loc_dir, surf_index == at_surf);

                if(distance > ZERO_DIST && dist_min - distance > OVERLAP_ERR){
                    dist_min = distance;
                    par_state->surf = signed_surf_index;
                    par_state->lat_bound_surf = 0;
                    par_state->bound_level = i;
                    par_state->bound_index = j;
                    for(int k = 0; k < 3; k++){
                        par_state->loc_pos[k] = loc_pos[k];
                        par_state->loc_dir[k] = loc_dir[k];
                    }
                }
            }
        }
    }

    if(par_state->surf == 0 && par_state->lat_bound_surf == -1)
        dist_min = -ONE;
    return dist_min;
}