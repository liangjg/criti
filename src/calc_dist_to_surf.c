//
// Created by xaq on 2018/1/29.
//

#include "common.h"


static double
_do_calc_dist(double a1,
              double b1,
              bool is_at_surf);

double
calc_dist_to_surf_P(const double *paras,
                    const double *pos,
                    const double *dir,
                    bool is_at_surf)
{
    double x, y, z, u, v, w;
    x = pos[0];
    y = pos[1];
    z = pos[2];
    u = dir[0];
    v = dir[1];
    w = dir[2];
    double t1, t2, t3, a1, b1;

    double dts = -ONE;

    t1 = paras[0] * u + paras[1] * v + paras[2] * w;
    if(EQ_ZERO(t1))
        return dts;
    else {
        t2 = paras[0] * x + paras[1] * y + paras[2] * z;
        dts = (paras[3] - t2) / t1;
        return dts;
    }
}

double
calc_dist_to_surf_PX(const double *paras,
                     const double *pos,
                     const double *dir,
                     bool is_at_surf)
{
    double x, y, z, u, v, w;
    x = pos[0];
    y = pos[1];
    z = pos[2];
    u = dir[0];
    v = dir[1];
    w = dir[2];
    double t1, t2, t3, a1, b1;

    double dts = -ONE;

    if(EQ_ZERO(u))
        return dts;
    else {
        dts = (paras[0] - x) / u;
        return dts;
    }
}

double
calc_dist_to_surf_PY(const double *paras,
                     const double *pos,
                     const double *dir,
                     bool is_at_surf)
{
    double x, y, z, u, v, w;
    x = pos[0];
    y = pos[1];
    z = pos[2];
    u = dir[0];
    v = dir[1];
    w = dir[2];
    double t1, t2, t3, a1, b1;

    double dts = -ONE;

    if(EQ_ZERO(v))
        return dts;
    else {
        dts = (paras[0] - y) / v;
        return dts;
    }
}

double
calc_dist_to_surf_PZ(const double *paras,
                     const double *pos,
                     const double *dir,
                     bool is_at_surf)
{
    double x, y, z, u, v, w;
    x = pos[0];
    y = pos[1];
    z = pos[2];
    u = dir[0];
    v = dir[1];
    w = dir[2];
    double t1, t2, t3, a1, b1;

    double dts = -ONE;

    if(EQ_ZERO(v))
        return dts;
    else {
        dts = (paras[0] - z) / w;
        return dts;
    }
}

double
calc_dist_to_surf_SO(const double *paras,
                     const double *pos,
                     const double *dir,
                     bool is_at_surf)
{
    double x, y, z, u, v, w;
    x = pos[0];
    y = pos[1];
    z = pos[2];
    u = dir[0];
    v = dir[1];
    w = dir[2];
    double t1, t2, t3, a1, b1;

    double dts = -ONE;

    a1 = x * u + y * v + z * w;
    b1 = x * x + y * y + z * z - paras[0] * paras[0];
    return _do_calc_dist(a1, b1, is_at_surf);
}

double
calc_dist_to_surf_S(const double *paras,
                    const double *pos,
                    const double *dir,
                    bool is_at_surf)
{
    double x, y, z, u, v, w;
    x = pos[0];
    y = pos[1];
    z = pos[2];
    u = dir[0];
    v = dir[1];
    w = dir[2];
    double t1, t2, t3, a1, b1;

    double dts = -ONE;

    t1 = x - paras[0];
    t2 = y - paras[1];
    t3 = z - paras[2];
    a1 = t1 * u + t2 * v + t3 * w;
    b1 = t1 * t1 + t2 * t2 + t3 * t3 - paras[3] * paras[3];
    return _do_calc_dist(a1, b1, is_at_surf);
}

double
calc_dist_to_surf_SX(const double *paras,
                     const double *pos,
                     const double *dir,
                     bool is_at_surf)
{
    double x, y, z, u, v, w;
    x = pos[0];
    y = pos[1];
    z = pos[2];
    u = dir[0];
    v = dir[1];
    w = dir[2];
    double t1, t2, t3, a1, b1;

    double dts = -ONE;

    t1 = x - paras[0];
    a1 = t1 * u + y * v + z * w;
    b1 = t1 * t1 + y * y + z * z - paras[1] * paras[1];
    return _do_calc_dist(a1, b1, is_at_surf);
}

double
calc_dist_to_surf_SY(const double *paras,
                     const double *pos,
                     const double *dir,
                     bool is_at_surf)
{
    double x, y, z, u, v, w;
    x = pos[0];
    y = pos[1];
    z = pos[2];
    u = dir[0];
    v = dir[1];
    w = dir[2];
    double t1, t2, t3, a1, b1;

    double dts = -ONE;

    t2 = y - paras[0];
    a1 = x * u + t2 * v + z * w;
    b1 = x * x + t2 * t2 + z * z - paras[1] * paras[1];
    return _do_calc_dist(a1, b1, is_at_surf);
}

double
calc_dist_to_surf_SZ(const double *paras,
                     const double *pos,
                     const double *dir,
                     bool is_at_surf)
{
    double x, y, z, u, v, w;
    x = pos[0];
    y = pos[1];
    z = pos[2];
    u = dir[0];
    v = dir[1];
    w = dir[2];
    double t1, t2, t3, a1, b1;

    double dts = -ONE;

    t3 = z - paras[0];
    a1 = x * u + y * v + t3 * w;
    b1 = x * x + y * y + t3 * t3 - paras[1] * paras[1];
    return _do_calc_dist(a1, b1, is_at_surf);
}

double
calc_dist_to_surf_C_X(const double *paras,
                      const double *pos,
                      const double *dir,
                      bool is_at_surf)
{
    double x, y, z, u, v, w;
    x = pos[0];
    y = pos[1];
    z = pos[2];
    u = dir[0];
    v = dir[1];
    w = dir[2];
    double t1, t2, t3, a1, b1;

    double dts = -ONE;

    t1 = SQUARE(v) + SQUARE(w);
    if(EQ_ZERO(t1))
        return dts;
    t1 = ONE / t1;
    t2 = y - paras[0];
    t3 = z - paras[1];
    a1 = (t2 * v + t3 * w) * t1;
    b1 = (t2 * t2 + t3 * t3 - paras[2] * paras[2]) * t1;
    return _do_calc_dist(a1, b1, is_at_surf);
}

double
calc_dist_to_surf_C_Y(const double *paras,
                      const double *pos,
                      const double *dir,
                      bool is_at_surf)
{
    double x, y, z, u, v, w;
    x = pos[0];
    y = pos[1];
    z = pos[2];
    u = dir[0];
    v = dir[1];
    w = dir[2];
    double t1, t2, t3, a1, b1;

    double dts = -ONE;

    t1 = SQUARE(u) + SQUARE(w);
    if(EQ_ZERO(t1))
        return dts;
    t1 = ONE / t1;
    t2 = x - paras[0];
    t3 = z - paras[1];
    a1 = (t2 * u + t3 * w) * t1;
    b1 = (t2 * t2 + t3 * t3 - paras[2] * paras[2]) * t1;
    return _do_calc_dist(a1, b1, is_at_surf);
}

double
calc_dist_to_surf_C_Z(const double *paras,
                      const double *pos,
                      const double *dir,
                      bool is_at_surf)
{
    double x, y, z, u, v, w;
    x = pos[0];
    y = pos[1];
    z = pos[2];
    u = dir[0];
    v = dir[1];
    w = dir[2];
    double t1, t2, t3, a1, b1;

    double dts = -ONE;

    t1 = SQUARE(u) + SQUARE(v);
    if(EQ_ZERO(t1))
        return dts;
    t1 = ONE / t1;
    t2 = x - paras[0];
    t3 = y - paras[1];
    a1 = (t2 * u + t3 * v) * t1;
    b1 = (t2 * t2 + t3 * t3 - paras[2] * paras[2]) * t1;
    return _do_calc_dist(a1, b1, is_at_surf);
}

double
calc_dist_to_surf_CX(const double *paras,
                     const double *pos,
                     const double *dir,
                     bool is_at_surf)
{
    double x, y, z, u, v, w;
    x = pos[0];
    y = pos[1];
    z = pos[2];
    u = dir[0];
    v = dir[1];
    w = dir[2];
    double t1, t2, t3, a1, b1;

    double dts = -ONE;

    t1 = SQUARE(v) + SQUARE(w);
    if(EQ_ZERO(t1))
        return dts;
    t1 = ONE / t1;
    a1 = (y * v + z * w) * t1;
    b1 = (y * y + z * z - paras[0] * paras[0]) * t1;
    return _do_calc_dist(a1, b1, is_at_surf);
}

double
calc_dist_to_surf_CY(const double *paras,
                     const double *pos,
                     const double *dir,
                     bool is_at_surf)
{
    double x, y, z, u, v, w;
    x = pos[0];
    y = pos[1];
    z = pos[2];
    u = dir[0];
    v = dir[1];
    w = dir[2];
    double t1, t2, t3, a1, b1;

    double dts = -ONE;

    t1 = SQUARE(u) + SQUARE(w);
    if(EQ_ZERO(t1))
        return dts;
    t1 = ONE / t1;
    a1 = (x * u + z * w) * t1;
    b1 = (x * x + z * z - paras[0] * paras[0]) * t1;
    return _do_calc_dist(a1, b1, is_at_surf);
}

double
calc_dist_to_surf_CZ(const double *paras,
                     const double *pos,
                     const double *dir,
                     bool is_at_surf)
{
    double x, y, z, u, v, w;
    x = pos[0];
    y = pos[1];
    z = pos[2];
    u = dir[0];
    v = dir[1];
    w = dir[2];
    double t1, t2, t3, a1, b1;

    double dts = -ONE;

    t1 = SQUARE(u) + SQUARE(v);
    if(EQ_ZERO(t1))
        return dts;
    t1 = ONE / t1;
    a1 = (x * u + y * v) * t1;
    b1 = (x * x + y * y - paras[0] * paras[0]) * t1;
    return _do_calc_dist(a1, b1, is_at_surf);
}

/* 解形如 t^2+2*a1*t+b1=0 的二次方程 */
double
_do_calc_dist(double a1,
              double b1,
              bool is_at_surf)
{
    double delta = SQUARE(a1) - b1;
    if(LT_ZERO(delta))     /* 说明粒子运动方向与曲面无交点 */
        return -ONE;
    if(is_at_surf)
        return -TWO * a1;  /* 这里如果delta == 0，结果应该是-a1，而不是-2 * a1 */
    else {
        double sqrt_delta = sqrt(delta);
        double d1 = -a1 + sqrt_delta;
        if(LT_ZERO(d1))    /* 说明粒子的运动方向为远离该曲面，不可能有交点 */
            return -ONE;
        double d2 = -a1 - sqrt_delta;

        return GT_ZERO(d2) ? d2 : d1;  /* 说明粒子运动方向上与该曲面有两个交点，一个穿进，一个穿出，因此最近的交点为d2 */
    }
}