//
// Created by xaq on 11/14/17.
//

#include "geometry.h"
#include "particle_state.h"

extern particle_state_t base_par_state;
extern map *base_cells;
extern map *base_univs;
extern map *base_surfs;

#define OVERLAP_ERR    1.0E-08
#define ZERO_DIST      1.0E-10

double calc_dist_to_bound(){
    universe_t *univ;
    cell_t *cell;
    surface_t *surf;
    double loc_pos[3];
    double loc_dir[3];
    double dist_min = 1.0E+22;
    int at_surf = base_par_state.bound_surf;
    base_par_state.bound_surf = 0;
    base_par_state.lat_bound_surf = -1;

    cell = (cell_t *) map_get(base_cells, base_par_state.cell);
    if(cell->is_inner_cell){
        for(int i = 0; i < 3; i++)
            loc_dir[i] = base_par_state.dir[i];
        for(int i = 0; i < base_par_state.loc_cells_sz; i++){
            int loc_univ = base_par_state.loc_univs[i];
            univ = (universe_t *) map_find(base_univs, loc_univ);
            trans_univ_dir(univ, loc_dir);
        }
        for(int i = 0; i < 3; i++)
            base_par_state.loc_dir[i] = loc_dir[i];

        char *rpn = cell->rpn;
        while(*rpn != '\0'){
            int surf_index = 0;
            if(ISNUMBER(*rpn)){
                do{
                    surf_index *= 10;
                    surf_index += *rpn - '0';
                    rpn++;
                } while(ISNUMBER(*rpn));
                surf = (surface_t *) map_get(base_univs, surf_index);
                double distance = calc_dist_to_surf(surf, base_par_state.loc_pos, base_par_state.loc_dir,
                                                    surf_index == at_surf);
                if(distance > ZERO_DIST && dist_min - distance > OVERLAP_ERR){
                    dist_min = distance;
                    base_par_state.bound_surf = surf_index;
                    base_par_state.lat_bound_surf = 0;
                    base_par_state.bound_level = base_par_state.loc_cells_sz - 1;
                }
            } else rpn++;
        }
    } else{
        for(int i = 0; i < 3; i++){
            loc_pos[i] = base_par_state.pos[i];
            loc_dir[i] = base_par_state.dir[i];
        }

        for(int i = 0; i < base_par_state.loc_cells_sz; i++){
            int loc_univ = base_par_state.loc_univs[i];
            univ = (universe_t *)map_get(base_univs, loc_univ);
            trans_univ_coord(univ, loc_pos, loc_dir);

            if(univ->is_lattice){
                int lat_index = base_par_state.loc_cells[i];
                move_to_origin_lat(univ, lat_index, loc_pos);
                int lat_bound_surf;
                double distance = calc_dist_to_lat(univ, loc_pos, loc_dir, &lat_bound_surf);
                if(distance < ZERO_DIST)
                    break;

                if(dist_min - distance > OVERLAP_ERR){
                    dist_min = distance;
                    base_par_state.bound_level = i;
                    base_par_state.lat_bound_surf = lat_bound_surf;
                    base_par_state.bound_surf = 0;
                    for(int j = 0; j < 3; j++){
                        base_par_state.loc_pos[j] = loc_pos[j];
                        base_par_state.loc_dir[j] = loc_dir[j];
                    }
                }
                continue;
            }

            int cell_index = base_par_state.loc_cells[i];
            cell = (cell_t *)map_get(base_cells, cell_index);

            char *rpn = cell->rpn;
            while(*rpn != '\0'){
                int surf_index = 0;
                if(ISNUMBER(*rpn)){
                    do{
                        surf_index *= 10;
                        surf_index += *rpn - '0';
                        rpn++;
                    } while(ISNUMBER(*rpn));
                    surf = (surface_t *) map_get(base_univs, surf_index);
                    double distance = calc_dist_to_surf(surf, base_par_state.loc_pos, base_par_state.loc_dir,
                                                        surf_index == at_surf);
                    if(distance > ZERO_DIST && dist_min - distance > OVERLAP_ERR){
                        dist_min = distance;
                        base_par_state.bound_surf = surf_index;
                        base_par_state.lat_bound_surf = 0;
                        base_par_state.bound_level = i;
                    }
                } else rpn++;
            }
        }
    }

    if(base_par_state.bound_surf == 0 && base_par_state.lat_bound_surf == -1)
        dist_min = -1.0;
    return dist_min;
}