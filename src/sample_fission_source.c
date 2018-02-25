//
// Created by xaq on 10/27/17.
//

#include "criticality.h"
#include "geometry.h"


extern criti_t base_criti;
extern double base_start_wgt;
extern universe_t *root_universe;

void sample_fission_source(particle_state_t *par_state){
    memset(par_state, 0x0, sizeof(particle_state_t));

    fission_bank_t *fission_src = &base_criti.fission_src[base_criti.fission_src_cnt++];

    for(int i = 0; i < 3; i++){
        par_state->pos[i] = fission_src->pos[i];
        par_state->dir[i] = fission_src->dir[i];
    }

    par_state->erg = fission_src->erg;
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
