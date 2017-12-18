//
// Created by xaq on 10/27/17.
//

#include "criticality.h"
#include "geometry.h"


extern criti_t base_criti;
extern double base_start_wgt;
extern map *base_cells;

void sample_fission_source(particle_state_t *par_state){
    memset(par_state, 0x0, sizeof(particle_state_t));

    /* memset makes par_state->is_killed to ZERO which is false */
    //    par_state->is_killed = false;
    fission_bank_t *fission_src = (fission_bank_t *) vector_at(&base_criti.fission_src, base_criti.fission_src_cnt);

    for(int i = 0; i < 3; i++){
        par_state->pos[i] = fission_src->pos[i];
        par_state->dir[i] = fission_src->dir[i];
    }

    par_state->erg = fission_src->erg;
    par_state->wgt = base_start_wgt;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-arith"
    par_state->loc_univs.ele_size = sizeof(int);
    par_state->loc_univs.start = malloc((size_t) (8) * sizeof(int));
    par_state->loc_univs.finish = par_state->loc_univs.start;
    par_state->loc_univs.end_of_storage = par_state->loc_univs.start + 8 * sizeof(int);
    par_state->loc_univs.value_free = NULL;

    par_state->loc_cells.ele_size = sizeof(int);
    par_state->loc_cells.start = malloc((size_t) (8) * sizeof(int));
    par_state->loc_cells.finish = par_state->loc_univs.start;
    par_state->loc_cells.end_of_storage = par_state->loc_univs.start + 8 * sizeof(int);
    par_state->loc_cells.value_free = NULL;

#pragma GCC diagnostic pop
    base_criti.fission_src_cnt++;

    par_state->cell = locate_particle(par_state, par_state->pos, par_state->dir);

    if(par_state->cell == -1){
        par_state->is_killed = true;
        return;
    }

    cell_t *cell = (cell_t *) map_get(base_cells, par_state->cell);

    if(cell->imp == 0){
        par_state->wgt = 0.0;
        par_state->is_killed = true;
    }

    par_state->mat = cell->mat;
    par_state->cell_tmp = cell->tmp;
}
