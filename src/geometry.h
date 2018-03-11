//
// Created by xaq on 10/27/17.
//

#ifndef CRITI_GEOMETRY_H
#define CRITI_GEOMETRY_H

#include "universe.h"
#include "cell.h"
#include "surface.h"
#include "particle_status.h"


BEGIN_DECL
cell_t *
locate_particle(particle_status_t *par_status,
                universe_t *start_univ,
                const double *pos,
                const double *dir);

void
find_next_cell(particle_status_t *par_status);

double
calc_dist_to_bound(particle_status_t *par_status);

void
find_neighbor_cell(particle_status_t *par_status);

void
build_neighbor_list();

/* 对几何信息进行预处理 */
int
preprocess_geometry();
END_DECL

#endif //CRITI_GEOMETRY_H
