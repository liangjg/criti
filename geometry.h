//
// Created by xaq on 10/27/17.
//

#include "universe.h"
#include "cell.h"
#include "surface.h"

#ifndef TTYW_GEOMETRY_H
#define TTYW_GEOMETRY_H

#ifdef __cplusplus
extern "C" {
#endif

int locate_particle(const double pos[3], const double dir[3]);

#ifdef __cplusplus
}
#endif

#endif //TTYW_GEOMETRY_H
