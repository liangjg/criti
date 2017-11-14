//
// Created by xaq on 11/14/17.
//

#include "geometry.h"
#include "particle_state.h"

extern map *base_univs;
extern map *base_cells;
extern map *base_surfs;
extern particle_state_t base_par_state;

void find_next_cell(){
    universe_t *univ;
    cell_t *cell;
    surface_t *surf;

    int univ_index = base_par_state.loc_univs[base_par_state.bound_level];
    univ = (universe_t *) map_get(base_univs, univ_index);

    if(univ->is_lattice){

    }
}