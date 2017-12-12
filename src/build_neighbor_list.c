//
// Created by xaq on 11/17/17.
//

#include "geometry.h"


extern map *base_surfs;
extern map *base_cells;
extern map *base_univs;

/* hash function prototype */
static void _val_free(void *val);
static uint64_t _int_hash_func(const void *key);

void build_neighbor_list(){
    universe_t *univ;
    cell_t *cell1, *cell2;
    map_entry *entry;
    size_t contained_cells;
    size_t contained_surfs1, contained_surfs2;
    map_type *type1, *type2;
    int cell1_index, cell2_index;
    int surf_index1, surf_index2;

    int table = base_univs->table;
    unsigned long used = base_univs->ht[table].used;
    type1 = (map_type *)malloc(sizeof(map_type));
    type2 = (map_type *)malloc(sizeof(map_type));

    type1->hash_func = _int_hash_func;
    type1->key_compare = NULL;
    type1->value_dup = NULL;
    type1->value_free = _val_free;

    type2->hash_func = _int_hash_func;
    type2->key_compare = NULL;
    type2->value_dup = NULL;
    type2->value_free = NULL;

    for(unsigned long i = 0; i < used; i++){
        entry = base_univs->ht[table].buckets[i];
        while(entry){
            univ = (universe_t *)entry->v.val;
            if(univ->is_lattice) continue;
            univ->neighbor_lists = map_create(type1);
            contained_cells = vector_size(&univ->cells);
            for(size_t j = 0; j < contained_cells; j++){
                cell1_index = *(int *)vector_at(&univ->cells, j);
                cell1 = (cell_t *)map_get(base_cells, cell1_index);
                contained_surfs1 = vector_size(&cell1->surfs);
                map *val = map_create(type2);
                map_put(univ->neighbor_lists, cell1_index, val);
                for(size_t k = 0; k < contained_surfs1; k++){
                    surf_index1 = *(int *)vector_at(&cell1->surfs, k);
                    for(size_t m = 0; m < contained_cells; m++){
                        if(j == m) continue;
                        cell2_index = *(int *)vector_at(&univ->cells, m);
                        cell2 = (cell_t *)map_get(base_cells, cell2_index);
                        contained_surfs2 = vector_size(&cell2->surfs);
                        for(size_t n = 0; n < contained_surfs2; n++){
                            surf_index2 = *(int *)vector_at(&cell2->surfs, n);
                            if(surf_index1 + surf_index2 == 0){
                                map_put(val, surf_index1, cell2);
                                break;
                            }
                        }
                    }
                }
            }
            entry = entry->next;
        }
    }
}

static void _val_free(void *val){
    map_free((map *) val);
}

static uint64_t _int_hash_func(const void *key){
    return _default_int_hash_func(*(uint32_t *) key);
}