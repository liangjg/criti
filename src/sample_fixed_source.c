//
// Created by 叶鑫 on 2018/1/16.
//

#include "fixed_source.h"
#include "geometry.h"


extern fixed_src_t base_fixed_src;
extern double base_start_wgt;
extern map *base_cells;

void sample_fixed_source(particle_state_t *par_state){
    memset(par_state, 0x0, sizeof(particle_state_t));

    fixed_src_bank_t *fixed_src;
    fixed_src = (fixed_src_bank_t *) vector_at(&base_fixed_src.fixed_src, base_fixed_src.fixed_src_cnt);

    for(int i = 0; i < 3; i++){
        par_state->pos[i] = fixed_src->pos[i];
        par_state->dir[i] = fixed_src->dir[i];
    }
    par_state->erg = fixed_src->erg;
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
    base_fixed_src.fixed_src_cnt++;

    par_state->cell = locate_particle(par_state, 0, par_state->pos, par_state->dir);

    if(par_state->cell == -1){
        par_state->is_killed = true;
        return;
    }

    cell_t *cell = (cell_t *) map_get(base_cells, par_state->cell);

    if(cell->imp == 0){
        par_state->wgt = ZERO;
        par_state->is_killed = true;
    }

    par_state->mat = cell->mat;
    par_state->cell_tmp = cell->tmp;
}