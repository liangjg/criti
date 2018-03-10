//
// Created by xaq on 10/27/17.
//

#include "criticality.h"
#include "geometry.h"


extern criti_t base_criti;
extern double base_start_wgt;
extern universe_t *root_universe;

void
sample_fission_src(particle_status_t *par_status)
{
    memset(par_status, 0x0, sizeof(particle_status_t));

    fission_bank_t *fission_src = &base_criti.fission_src[base_criti.fission_src_cnt++];

    for(int i = 0; i < 3; i++) {
        par_status->pos[i] = fission_src->pos[i];
        par_status->dir[i] = fission_src->dir[i];
    }

    par_status->erg = fission_src->erg;
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
