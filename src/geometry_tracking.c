//
// Created by xaq on 10/27/17.
//

#include "neutron_transport.h"
#include "criticality.h"
#include "particle_state.h"

extern criti_t base_criti;

extern particle_state_t base_par_state;

void geometry_tracking(){
    bool particle_on_surf = true;
    double distance;

    do{
        distance = get_next_track(&particle_on_surf);

        if(base_par_state.is_killed) return;

        estimate_keff_trk();

        fly_by_length();
    } while(particle_on_surf);

    estimate_keff_col();
}
