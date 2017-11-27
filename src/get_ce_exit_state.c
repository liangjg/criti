//
// Created by xaq on 11/15/17.
//

#include "neutron_transport.h"
#include "map.h"
#include "acedata.h"


extern map *base_nucs;

void get_ce_exit_state(particle_state_t *par_state, int MT){
    double exit_mu_lab;
    double exit_erg_lab;
    nuclide_t *nuc = (nuclide_t *)map_get(base_nucs, par_state->nuc);

    get_ce_exist_erg_mu(nuc, MT, par_state->erg, &exit_erg_lab, &exit_mu_lab);

    rotate_dir(exit_mu_lab, par_state->dir, par_state->exit_dir);
    par_state->exit_erg = exit_erg_lab;

    if(!(par_state->exit_erg >= ZERO && par_state->exit_erg < 100)){
        puts("exit energy out of range.");
        par_state->exit_erg = EG0_CUTOFF;
    }
}