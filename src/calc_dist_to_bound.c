//
// Created by xaq on 11/14/17.
//

#include "geometry.h"


#define OVERLAP_ERR    1.0E-08
#define ZERO_DIST      1.0E-10

double calc_dist_to_bound(particle_state_t *par_state){
    universe_t *univ;
    cell_t *cell;
    surface_t *surf;
    double loc_pos[3];
    double loc_dir[3];
    int signed_surf_index, surf_index, lat_index, lat_bound_surf;
    int univ_cell_index, at_surf;
    double dist_min, distance;
    int i, j, k;

    at_surf = abs(par_state->surf);
    dist_min = 1.0E+22;
    par_state->surf = 0;
    par_state->lat_bound_surf = -1;

    cell = par_state->cell;
    if(cell->is_inner_cell){
        for(i = 0; i < 3; i++)
            loc_dir[i] = par_state->dir[i];
        for(i = 0; i < par_state->loc_sz; i++){
            univ = par_state->loc_univs[i];
            trans_univ_dir(univ, loc_dir);
        }
        for(i = 0; i < 3; i++)
            par_state->loc_dir[i] = loc_dir[i];

        for(i = 0; i < cell->surfs_sz; i++){
            signed_surf_index = cell->surfs[i];
            surf_index = abs(signed_surf_index);
            surf = cell->surfs_addr[i];
            distance = calc_dist_to_surf(surf, par_state->loc_pos, par_state->loc_dir, surf_index == at_surf);

            if(distance > ZERO_DIST && dist_min - distance > OVERLAP_ERR){
                dist_min = distance;
                par_state->surf = signed_surf_index;
                par_state->bound_index = i;
            }
        }
        par_state->lat_bound_surf = 0;
        par_state->bound_level = par_state->loc_sz - 1;
    } else{
        for(i = 0; i < 3; i++){
            loc_pos[i] = par_state->pos[i];
            loc_dir[i] = par_state->dir[i];
        }

        for(i = 0; i < par_state->loc_sz; i++){
            univ = par_state->loc_univs[i];
            trans_univ_coord(univ, loc_pos, loc_dir);

            if(univ->lattice_type){
                lat_index = par_state->loc_cells[i];
                move_to_origin_lat(univ, lat_index, loc_pos);
                distance = calc_dist_to_lat(univ, loc_pos, loc_dir, &lat_bound_surf);

                if(distance > ZERO_DIST && dist_min - distance > OVERLAP_ERR){
                    dist_min = distance;
                    par_state->bound_level = i;
                    par_state->lat_bound_surf = lat_bound_surf;
                    par_state->surf = 0;
                    for(j = 0; j < 3; j++){
                        par_state->loc_pos[j] = loc_pos[j];
                        par_state->loc_dir[j] = loc_dir[j];
                    }
                }
                continue;
            }

            univ_cell_index = par_state->loc_cells[i];
            cell = univ->cells[univ_cell_index];

            for(j = 0; j < cell->surfs_sz; j++){
                signed_surf_index = cell->surfs[j];
                surf_index = abs(signed_surf_index);
                surf = cell->surfs_addr[j];
                distance = calc_dist_to_surf(surf, loc_pos, loc_dir, surf_index == at_surf);

                if(distance > ZERO_DIST && dist_min - distance > OVERLAP_ERR){
                    dist_min = distance;
                    par_state->surf = signed_surf_index;
                    par_state->lat_bound_surf = 0;
                    par_state->bound_level = i;
                    par_state->bound_index = j;
                    for(k = 0; k < 3; k++){
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