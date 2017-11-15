//
// Created by xaq on 11/15/17.
//

#include "neutron_transport.h"
#include "particle_state.h"
#include "map.h"
#include "acedata.h"

extern particle_state_t base_par_state;
extern map *base_nucs;

void get_ce_exit_state(int fis_MT){
    double exit_mu_lab;
    double exit_erg_lab;
    nuclide_t *nuc = (nuclide_t *)map_get(base_nucs, base_par_state.nuc);

    get_ce_exist_erg_mu(nuc, fis_MT, base_par_state.erg, &exit_erg_lab, &exit_mu_lab);

    rotate_dir(exit_mu_lab, base_par_state.dir, base_par_state.exit_dir);
    base_par_state.exit_erg = exit_erg_lab;

    if(!(base_par_state.exit_erg >= ZERO && base_par_state.exit_erg < 100)){
        puts("exit energy out of range.");
        base_par_state.exit_erg = EG0_CUTOFF;
    }
}