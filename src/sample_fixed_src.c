//
// Created by xaq on 2018/1/16.
//

#include "fixed_source.h"
#include "geometry.h"


extern fixed_src_t base_fixed_src;
extern double base_start_wgt;
extern universe_t *root_universe;

void sample_fixed_src(particle_state_t *par_state){
    memset(par_state, 0x0, sizeof(particle_state_t));

    fixed_src_bank_t *fixed_src;
    fixed_src = &base_fixed_src.fixed_src[base_fixed_src.fixed_src_cnt++];

    for(int i = 0; i < 3; i++){
        par_state->pos[i] = fixed_src->pos[i];
        par_state->dir[i] = fixed_src->dir[i];
    }
    par_state->erg = fixed_src->erg;
    par_state->wgt = base_start_wgt;
    par_state->cell = locate_particle(par_state, root_universe, par_state->pos, par_state->dir);

    if(!par_state->cell){
        par_state->is_killed = true;
        return;
    }

    cell_t *cell = par_state->cell;

    if(cell->imp == 0){
        par_state->wgt = ZERO;
        par_state->is_killed = true;
    }

    par_state->mat = cell->mat;
    par_state->cell_tmp = cell->tmp;
}