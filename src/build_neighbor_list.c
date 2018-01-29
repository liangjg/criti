//
// Created by xaq on 11/17/17.
//

#include "geometry.h"


extern map *base_cells;
extern map *base_univs;

/* hash function prototype */
static void _val_free(void *val);
static uint64_t _int_hash_func(const void *key);
static void _vector_free(void *val);

void build_neighbor_list(){
    register universe_t *univ;
    cell_t *cell1, *cell2;
    map_entry *entry;
    int contained_cells;
    int contained_surfs1, contained_surfs2;
    map_type *type1, *type2;
    int cell1_index, cell2_index;
    int surf_index1, surf_index2;
    register map *local_base_cells = base_cells;

    type1 = (map_type *)malloc(sizeof(map_type));
    type2 = (map_type *)malloc(sizeof(map_type));

    type1->hash_func = _int_hash_func;
    type1->key_compare = NULL;
    type1->value_dup = NULL;
    type1->value_free = _val_free;

    type2->hash_func = _int_hash_func;
    type2->key_compare = NULL;
    type2->value_dup = NULL;
    type2->value_free = _vector_free;

    map_iterator *univ_iter = map_get_iter(base_univs);

    while((entry = map_iter_next(univ_iter))){
        univ = entry->v.val;
        if(univ->is_lattice)
            continue;
        univ->neighbor_lists = map_create(type1);
        contained_cells = univ->cells_sz;
        for(size_t j = 0; j < contained_cells; j++){
            cell1_index = univ->cells[j];
            cell1 = map_get(local_base_cells, cell1_index);
            contained_surfs1 = cell1->surfs_sz;
            map *val = map_create(type2);
            map_put(univ->neighbor_lists, cell1_index, val);
            for(size_t k = 0; k < contained_surfs1; k++){
                vector *neighbor_cells = vector_init(4, sizeof(cell_t *));
                surf_index1 = cell1->surfs[k];
                for(size_t m = 0; m < contained_cells; m++){
                    if(j == m) continue;
                    cell2_index = univ->cells[m];
                    cell2 = map_get(local_base_cells, cell2_index);
                    contained_surfs2 = cell2->surfs_sz;
                    for(size_t n = 0; n < contained_surfs2; n++){
                        surf_index2 = cell2->surfs[n];
                        if(surf_index1 + surf_index2 == 0){
                            vector_push_back(neighbor_cells, &cell2);
                            break;
                        }
                    }
                }
                map_put(val, surf_index1, neighbor_cells);
            }
        }
    }

    map_release_iter(univ_iter);
}

static void _val_free(void *val){
    map_free((map *) val);
    free(val);
}

static uint64_t _int_hash_func(const void *key){
    return _default_int_hash_func(*(uint32_t *) key);
}

static void _vector_free(void *val){
    vector_free((vector *) val);
    free(val);
}