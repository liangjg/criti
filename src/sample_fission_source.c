//
// Created by xaq on 10/27/17.
//

#include "criticality.h"
#include "geometry.h"


/* 全局变量，所有从核都相同 */
extern double base_start_wgt;
extern universe_t *root_universe;

void
sample_fission_source(particle_status_t *par_status,
                      int fis_src_cnt,
                      fission_bank_t *fis_src_slave)
{
    int i;

    memset(par_status, 0x0, sizeof(particle_status_t));
    for(i = 0; i < 3; i++) {
        par_status->pos[i] = fis_src_slave[fis_src_cnt].pos[i];
        par_status->dir[i] = fis_src_slave[fis_src_cnt].dir[i];
    }

    par_status->erg = fis_src_slave[fis_src_cnt].erg;
    par_status->wgt = base_start_wgt;
    par_status->cell = locate_particle(par_status, root_universe, par_status->pos, par_status->dir);

    if(!par_status->cell) {
        par_status->is_killed = true;
        return;
    }

    cell_t *cell = par_status->cell;
    if(cell->imp == 0) {
        par_status->wgt = ZERO;
        par_status->is_killed = true;
    }

    par_status->mat = cell->mat;
    par_status->cell_tmp = cell->tmp;
}
