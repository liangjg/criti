//
// Created by 叶鑫 on 2018/1/16.
//

#include "fixed_source.h"
#include "geometry.h"


extern fixed_src_t base_fixed_src;
extern universe_t *root_universe;

void sample_fission_source_fixed(particle_state_t *par_state){
    memset(par_state, 0x0, sizeof(particle_state_t));

    fixed_src_bank_t *fixed_src;
    /* *************************************************************************************************
     * 这里fixed_src_bank_cnt - 1，是因为，fixed_src_bank_cnt表示的是存储的粒子数目，但是实际下标是从0开始的
     * 例如，当fixed_src_bank_cnt = 1时，实际的粒子信息存储在fixed_src_bank[0]这个位置上
     * *************************************************************************************************/
    fixed_src = (fixed_src_bank_t *) vector_at(&base_fixed_src.fixed_src_bank, base_fixed_src.fixed_src_bank_cnt - 1);

    for(int i = 0; i < 3; i++){
        par_state->pos[i] = fixed_src->pos[i];
        par_state->dir[i] = fixed_src->dir[i];
    }
    par_state->erg = fixed_src->erg;
    par_state->wgt = fixed_src->wgt;

    base_fixed_src.fixed_src_bank_cnt--;

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