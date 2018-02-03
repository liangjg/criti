//
// Created by xaq on 2018/2/3.
//

#include "geometry.h"


extern map *base_univs;
extern map *base_cells;
extern map *base_surfs;
universe_t *root_universe;

int preprocess_geometry(){
    universe_t *univ;
    cell_t *cell;
    map_entry *entry;
    map_iterator *cell_iter, *univ_iter;
    int filled_univ_index;
    int surfs_sz, filled_lat_univs_sz;
    int surf_index;

    cell_iter = map_get_iter(base_cells);
    while((entry = map_iter_next(cell_iter))){
        cell = entry->v.val;
        /* 调整cell->fill指针和univ->parent指针 */
        if(cell->fill){
            filled_univ_index = (int) cell->fill;
            univ = map_get(base_univs, filled_univ_index);
            cell->fill = univ;
            univ->parent = cell;
        }

        /* 调整cell->surfs_addr */
        surfs_sz = cell->surfs_sz;
        cell->surfs_addr = (void **) malloc(surfs_sz * sizeof(void *));
        for(int i = 0; i < surfs_sz; i++){
            surf_index = abs(cell->surfs[i]);
            cell->surfs_addr[i] = map_get(base_surfs, surf_index);
        }
    }

    univ_iter = map_get_iter(base_univs);
    while((entry = map_iter_next(univ_iter))){
        univ = entry->v.val;
        /* 调整univ->filled_lat_univs */
        if(univ->filled_lat_univs){
            filled_lat_univs_sz = univ->scope[0] * univ->scope[1];
            if(univ->lattice_type == 1) filled_lat_univs_sz *= univ->scope[2];
            for(int i = 0; i < filled_lat_univs_sz; i++){
                filled_univ_index = (int) univ->filled_lat_univs[i];
                univ->filled_lat_univs[i] = map_get(base_univs, filled_univ_index);
            }
        }
    }

    map_release_iter(cell_iter);
    map_release_iter(univ_iter);

    root_universe = map_get(base_univs, 0);
    /* 构建邻居栅元 */
    build_neighbor_list();
    return 0;
}