//
// Created by xaq on 11/17/17.
//

#include "geometry.h"
#include "particle_state.h"
#include "cell.h"
#include "surface.h"
#include "universe.h"

extern map *base_surfs;
extern map *base_cells;
extern map *base_univs;
extern particle_state_t base_par_state;

void build_neighbor_list(){
    universe_t *univ;
    cell_t *cell, *cell2;
    surface_t *surf;
    map_entry *entry, *entry2;
    int contained_cells;
    int contained_surfs;

    int table = base_univs->table;
    int used = base_univs->ht[table].used;

    for(int i = 0; i < used; i++){
        entry = base_univs->ht[table].buckets[i];
        while(entry){
            univ = (universe_t *)entry->v.val;
            if(univ->is_lattice) continue;
            contained_cells = univ->contain_cell_num;
            univ->neighbor_lists = (int **)malloc(contained_cells * sizeof(int *));
            for(int j = 0; j < contained_cells; j++){
                cell = (cell_t *)map_get(base_cells, univ->fill_cells[i]);
                contained_surfs = cell->contain_surfs;
                univ->neighbor_lists[j] = (int *)malloc(contained_surfs * sizeof(int));
                for(int k = 0; k < contained_surfs; k++){
                    surf = (surface_t *)map_get(base_surfs, abs(cell->surfs[k]));
                    for(int m = 0; m < surf->cell_sz; m++){
                        entry2 = map_find(base_cells, surf->cell[m]);
                        if(entry2 && entry2->v.val != cell){
                            cell2 = entry2->v.val;
                            for(int n = 0; n < cell2->contain_surfs; n++){
                                if(cell->surfs[k] + cell2->surfs[n] == 0){
                                    univ->neighbor_lists[j][k] = cell2->id;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            entry = entry->next;
        }
    }

}