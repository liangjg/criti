//
// Created by xaq on 9/3/17.
//

#ifndef CRITI_SURFACE_H
#define CRITI_SURFACE_H

#include "common.h"


typedef enum SURF_T {
    P = 1,        //任意类型的平面，Ax+By+Cz=D
    PX = 2,       //垂直于X轴的平面，x=x0
    PY = 3,       //垂直于y轴的平面，y=y0
    PZ = 4,       //垂直于z轴的平面，z=z0
    SO = 5,       //球心在原点的球面，x^2+y^2+z^2=R^2
    S = 6,        //球心在任意处的球面，(x-x0)^2+(y-y0)^2+(z-z0)^2=R^2
    SX = 7,       //球心在x轴的球面，(x-x0)^2+y^2+z^2=R^2
    SY = 8,       //球心在y轴的球面，x^2+(y-y0)^2+z^2=R^2
    SZ = 9,       //球心在z轴的球面，x^2+y^2+(z-z0)^2=R^2
    C_X = 10,     //轴线平行于x轴的圆柱面，(y-y0)^2+(z-z0)^2=R^2
    C_Y = 11,     //轴线平行于y轴的圆柱面，(x-x0)^2+(z-z0)^2=R^2
    C_Z = 12,     //轴线平行于z轴的圆柱面，(x-x0)^2+(y-y0)^2=R^2
    CX = 13,      //轴线在x轴的圆柱面，y^2+z^2=R^2
    CY = 14,      //轴线在y轴的圆柱面，x^2+z^2=R^2
    CZ = 15       //轴线在z轴的圆柱面，x^2+y^2=R^2
} SURF_T;

typedef struct surface_t {
    int id;              /* surface id */
    SURF_T type;         /* surface type */
    double paras[4];     /* type parameters */
    int bc;              /* boundary condition of this surface */
} surface_t;

BEGIN_DECL
#define LENGTH_VECTOR(vec)    (sqrt(SQUARE((vec)[0]) + SQUARE((vec)[1]) + SQUARE((vec)[2])))

surface_t *
surf_init();

int
calc_surf_sense(surface_t *obj,
                const double pos[3],
                const double dir[3]);

void
get_surf_norm_vec(surface_t *obj,
                  const double pos[3],
                  double *surf_norm_vec);

double
calc_dist_to_surf(surface_t *obj,
                  const double pos[3],
                  const double dir[3],
                  bool is_at_surf);

void
reflect_par(surface_t *obj,
            const double pos[3],
            double *dir,
            double *loc_dir);

void
surf_free(surface_t *obj);

END_DECL

#endif //CRITI_SURFACE_H
