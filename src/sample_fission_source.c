//
// Created by xaq on 10/27/17.
//

#include "criticality.h"
#include "geometry.h"

/* 全局变量，所有从核都相同 */
extern double base_start_wgt;
extern map *base_cells;

/* 从核LDM中的变量 */
extern int fis_src_cnt;
extern fission_bank_t fis_src_slave[400];

void sample_fission_source(particle_state_t *par_state){
    memset(par_state, 0x0, sizeof(particle_state_t));

    /* memset makes par_state->is_killed to ZERO which is false */

    for(int i = 0; i < 3; i++){
        par_state->pos[i] = fis_src_slave[fis_src_cnt].pos[i];
        par_state->dir[i] = fis_src_slave[fis_src_cnt].dir[i];
    }

    par_state->erg = fis_src_slave[fis_src_cnt].erg;
    par_state->wgt = base_start_wgt;
    fis_src_cnt++;

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
