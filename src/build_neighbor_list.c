//
// Created by xaq on 11/17/17.
//

#include "geometry.h"
#include "vector.h"
#include "map.h"


extern map *base_cells;

void build_neighbor_list(){
    cell_t *cell, *neighbor_cell;
    universe_t *univ;
    map_iterator *cell_iter;
    map_entry *entry;
    int surf_index, surfs_sz, surfs_sz2, cells_sz, neighbor_sz;
    int i, j, k;
    vector *vec;

    vec = vector_init(8, sizeof(void *));
    cell_iter = map_get_iter(base_cells);
    while((entry = map_iter_next(cell_iter))){
        cell = entry->v.val;
        univ = cell->parent;
        cells_sz = univ->cells_sz;
        surfs_sz = cell->surfs_sz;
        cell->neighbor_lists = (void ***) malloc(surfs_sz * sizeof(void **));
        cell->neighbor_lists_sz = (int *) malloc(surfs_sz * sizeof(int));
        for(i = 0; i < surfs_sz; i++){
            surf_index = cell->surfs[i];
            for(j = 0; j < cells_sz; j++){
                neighbor_cell = univ->cells[j];
                surfs_sz2 = neighbor_cell->surfs_sz;
                for(k = 0; k < surfs_sz2; k++){
                    if(surf_index + neighbor_cell->surfs[k] == 0)
                        vector_push_back(vec, &neighbor_cell);
                }
            }

            neighbor_sz = vector_size(vec);
            cell->neighbor_lists_sz[i] = neighbor_sz;
            cell->neighbor_lists[i] = (void **) malloc(neighbor_sz * sizeof(void *));
            memcpy(cell->neighbor_lists[i], vec->start, sizeof(void *) * neighbor_sz);
            vector_clear(vec);
        }
    }

    map_release_iter(cell_iter);
    vector_free(vec);
    free(vec);
}