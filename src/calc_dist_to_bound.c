//
// Created by xaq on 11/14/17.
//

#include "geometry.h"


#define OVERLAP_ERR    1.0E-08
#define ZERO_DIST      1.0E-10

double
calc_dist_to_bound(particle_status_t *par_status)
{
    universe_t *univ;
    cell_t *cell;
    surface_t *surf;
    double loc_pos[3];
    double loc_dir[3];
    int signed_surf_index, surf_index, lat_index, lat_bound_surf;
    int univ_cell_index;
    double dist_min, distance;
    dist_min = 1.0E+22;
    int at_surf = abs(par_status->surf);
    par_status->surf = 0;
    par_status->lat_bound_surf = -1;

    cell = par_status->cell;
    if(cell->is_inner_cell) {
        for(int i = 0; i < 3; i++)
            loc_dir[i] = par_status->dir[i];
        for(int i = 0; i < par_status->loc_sz; i++) {
            univ = par_status->loc_univs[i];
            trans_univ_dir(univ, loc_dir);
        }
        for(int i = 0; i < 3; i++)
            par_status->loc_dir[i] = loc_dir[i];

        for(int i = 0; i < cell->surfs_sz; i++) {
            signed_surf_index = cell->surfs[i];
            surf_index = abs(signed_surf_index);
            surf = cell->surfs_addr[i];
            distance = surf->funcs->calc_dist_to_surf(surf->paras, par_status->loc_pos, par_status->loc_dir, surf_index == at_surf);

            if(distance > ZERO_DIST && dist_min - distance > OVERLAP_ERR) {
                dist_min = distance;
                par_status->surf = signed_surf_index;
                par_status->bound_index = i;
            }
        }
        par_status->lat_bound_surf = 0;
        par_status->bound_level = par_status->loc_sz - 1;
    } else {
        for(int i = 0; i < 3; i++) {
            loc_pos[i] = par_status->pos[i];
            loc_dir[i] = par_status->dir[i];
        }

        for(int i = 0; i < par_status->loc_sz; i++) {
            univ = par_status->loc_univs[i];
            trans_univ_coord(univ, loc_pos, loc_dir);

            if(univ->lattice_type) {
                lat_index = par_status->loc_cells[i];
                move_to_origin_lat(univ, lat_index, loc_pos);
                distance = calc_dist_to_lat(univ, loc_pos, loc_dir, &lat_bound_surf);

                if(distance > ZERO_DIST && dist_min - distance > OVERLAP_ERR) {
                    dist_min = distance;
                    par_status->bound_level = i;
                    par_status->lat_bound_surf = lat_bound_surf;
                    par_status->surf = 0;
                    for(int j = 0; j < 3; j++) {
                        par_status->loc_pos[j] = loc_pos[j];
                        par_status->loc_dir[j] = loc_dir[j];
                    }
                }
                continue;
            }

            univ_cell_index = par_status->loc_cells[i];
            cell = univ->cells[univ_cell_index];

            for(int j = 0; j < cell->surfs_sz; j++) {
                signed_surf_index = cell->surfs[j];
                surf_index = abs(signed_surf_index);
                surf = cell->surfs_addr[j];
                distance = surf->funcs->calc_dist_to_surf(surf->paras, loc_pos, loc_dir, surf_index == at_surf);

                if(distance > ZERO_DIST && dist_min - distance > OVERLAP_ERR) {
                    dist_min = distance;
                    par_status->surf = signed_surf_index;
                    par_status->lat_bound_surf = 0;
                    par_status->bound_level = i;
                    par_status->bound_index = j;
                    for(int k = 0; k < 3; k++) {
                        par_status->loc_pos[k] = loc_pos[k];
                        par_status->loc_dir[k] = loc_dir[k];
                    }
                }
            }
        }
    }

    if(par_status->surf == 0 && par_status->lat_bound_surf == -1)
        dist_min = -ONE;
    return dist_min;
}