//
// Created by 叶鑫 on 2017/11/6.
//

#include "neutron_transport.h"
#include "particle_state.h"
#include "geometry.h"

extern particle_state_t base_par_state;

double get_next_track(bool *particle_on_surf){
    if(*particle_on_surf == true){
        find_next_cell();
        if(base_par_state.cell == -1) base_warnings++;
    }
    if(base_par_state.is_killed) return ZERO;

    double DTB = calc_dist_to_bound();
//    if(DTB)

    double FFL = sample_free_fly_dis();

    double res;

    if(FFL > DTB){
        *particle_on_surf = true;
        res = DTB;
    }
    else{
        *particle_on_surf = false;
        res = FFL;
    }
    return res;
}