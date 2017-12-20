//
// Created by xaq on 10/27/17.
//

#include "universe.h"
#include "cell.h"
#include "surface.h"
#include "particle_state.h"

#ifndef TTYW_GEOMETRY_H
#define TTYW_GEOMETRY_H

#ifdef __cplusplus
extern "C" {
#endif

int locate_particle(particle_state_t *par_state, int start_univ, const double pos[3], const double dir[3]);

void find_next_cell(particle_state_t *par_state);

double calc_dist_to_bound(particle_state_t *par_state);

void find_neighbor_cell(particle_state_t *par_state);

void build_neighbor_list();

#ifdef __cplusplus
}
#endif

#endif //TTYW_GEOMETRY_H
