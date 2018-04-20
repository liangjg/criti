//
// Created by xaq on 2018/1/29.
//

#include "common.h"


static inline void
_normalize(double *surf_norm_vec)
{
    double length = ONE / sqrt(SQUARE(surf_norm_vec[0]) + SQUARE(surf_norm_vec[1]) + SQUARE(surf_norm_vec[2]));
    surf_norm_vec[0] *= length;
    surf_norm_vec[1] *= length;
    surf_norm_vec[2] *= length;
}

void
get_surf_norm_vec_P(const double *paras,
                    const double *pos,
                    double *surf_norm_vec)
{
    surf_norm_vec[0] = paras[0];
    surf_norm_vec[1] = paras[1];
    surf_norm_vec[2] = paras[2];

    _normalize(surf_norm_vec);
}

void
get_surf_norm_vec_PX(const double *paras,
                     const double *pos,
                     double *surf_norm_vec)
{
    surf_norm_vec[0] = ONE;
    surf_norm_vec[1] = ZERO;
    surf_norm_vec[2] = ZERO;

    _normalize(surf_norm_vec);
}

void
get_surf_norm_vec_PY(const double *paras,
                     const double *pos,
                     double *surf_norm_vec)
{
    surf_norm_vec[0] = ZERO;
    surf_norm_vec[1] = ONE;
    surf_norm_vec[2] = ZERO;

    _normalize(surf_norm_vec);
}

void
get_surf_norm_vec_PZ(const double *paras,
                     const double *pos,
                     double *surf_norm_vec)
{
    surf_norm_vec[0] = ZERO;
    surf_norm_vec[1] = ZERO;
    surf_norm_vec[2] = ONE;

    _normalize(surf_norm_vec);
}

void
get_surf_norm_vec_SO(const double *paras,
                     const double *pos,
                     double *surf_norm_vec)
{
    double x = pos[0];
    double y = pos[1];
    double z = pos[2];

    surf_norm_vec[0] = x;
    surf_norm_vec[1] = y;
    surf_norm_vec[2] = z;

    _normalize(surf_norm_vec);
}

void
get_surf_norm_vec_S(const double *paras,
                    const double *pos,
                    double *surf_norm_vec)
{
    double x = pos[0];
    double y = pos[1];
    double z = pos[2];

    surf_norm_vec[0] = x - paras[0];
    surf_norm_vec[1] = y - paras[1];
    surf_norm_vec[2] = z - paras[2];

    _normalize(surf_norm_vec);
}

void
get_surf_norm_vec_SX(const double *paras,
                     const double *pos,
                     double *surf_norm_vec)
{
    double x = pos[0];
    double y = pos[1];
    double z = pos[2];

    surf_norm_vec[0] = x - paras[0];
    surf_norm_vec[1] = y;
    surf_norm_vec[2] = z;

    _normalize(surf_norm_vec);
}

void
get_surf_norm_vec_SY(const double *paras,
                     const double *pos,
                     double *surf_norm_vec)
{
    double x = pos[0];
    double y = pos[1];
    double z = pos[2];

    surf_norm_vec[0] = x;
    surf_norm_vec[1] = y - paras[0];
    surf_norm_vec[2] = z;

    _normalize(surf_norm_vec);
}

void
get_surf_norm_vec_SZ(const double *paras,
                     const double *pos,
                     double *surf_norm_vec)
{
    double x = pos[0];
    double y = pos[1];
    double z = pos[2];

    surf_norm_vec[0] = x;
    surf_norm_vec[1] = y;
    surf_norm_vec[2] = z - paras[0];

    _normalize(surf_norm_vec);
}

void
get_surf_norm_vec_C_X(const double *paras,
                      const double *pos,
                      double *surf_norm_vec)
{
    double x = pos[0];
    double y = pos[1];
    double z = pos[2];

    surf_norm_vec[0] = ZERO;
    surf_norm_vec[1] = y - paras[0];
    surf_norm_vec[2] = z - paras[1];

    _normalize(surf_norm_vec);
}

void
get_surf_norm_vec_C_Y(const double *paras,
                      const double *pos,
                      double *surf_norm_vec)
{
    double x = pos[0];
    double y = pos[1];
    double z = pos[2];

    surf_norm_vec[0] = x - paras[0];
    surf_norm_vec[1] = ZERO;
    surf_norm_vec[2] = z - paras[1];

    _normalize(surf_norm_vec);
}

void
get_surf_norm_vec_C_Z(const double *paras,
                      const double *pos,
                      double *surf_norm_vec)
{
    double x = pos[0];
    double y = pos[1];
    double z = pos[2];

    surf_norm_vec[0] = x - paras[0];
    surf_norm_vec[1] = y - paras[1];
    surf_norm_vec[2] = ZERO;

    _normalize(surf_norm_vec);
}

void
get_surf_norm_vec_CX(const double *paras,
                     const double *pos,
                     double *surf_norm_vec)
{
    double x = pos[0];
    double y = pos[1];
    double z = pos[2];

    surf_norm_vec[0] = ZERO;
    surf_norm_vec[1] = y;
    surf_norm_vec[2] = z;

    _normalize(surf_norm_vec);
}

void
get_surf_norm_vec_CY(const double *paras,
                     const double *pos,
                     double *surf_norm_vec)
{
    double x = pos[0];
    double y = pos[1];
    double z = pos[2];

    surf_norm_vec[0] = x;
    surf_norm_vec[1] = ZERO;
    surf_norm_vec[2] = z;

    _normalize(surf_norm_vec);
}

void
get_surf_norm_vec_CZ(const double *paras,
                     const double *pos,
                     double *surf_norm_vec)
{
    double x = pos[0];
    double y = pos[1];
    double z = pos[2];

    surf_norm_vec[0] = x;
    surf_norm_vec[1] = y;
    surf_norm_vec[2] = ZERO;

    _normalize(surf_norm_vec);
}