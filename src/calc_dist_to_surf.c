//
// Created by xaq on 2018/1/29.
//

#include "surface.h"


static double _do_calc_dist(double a1, double b1, bool is_at_surf);

double calc_dist_to_surf(surface_t *obj, const double pos[3], const double dir[3], bool is_at_surf){
    double x, y, z, u, v, w;
    x = pos[0];
    y = pos[1];
    z = pos[2];
    u = dir[0];
    v = dir[1];
    w = dir[2];
    double t1, t2, t3, a1, b1;

    double dts = -ONE;

    switch(obj->type){
        case P:{
            t1 = obj->paras[0] * u + obj->paras[1] * v + obj->paras[2] * w;
            if(EQ_ZERO(t1))
                return dts;
            else {
                t2 = obj->paras[0] * x + obj->paras[1] * y + obj->paras[2] * z;
                dts = (obj->paras[3] - t2) / t1;
                return dts;
            }
        }
        case PX:{
            if(EQ_ZERO(u))
                return dts;
            else {
                dts = (obj->paras[0] - x) / u;
                return dts;
            }
        }
        case PY:{
            if(EQ_ZERO(v))
                return dts;
            else {
                dts = (obj->paras[0] - y) / v;
                return dts;
            }
        }
        case PZ:{
            if(EQ_ZERO(w))
                return dts;
            else {
                dts = (obj->paras[0] - z) / w;
                return dts;
            }
        }
        case SO:{
            a1 = x * u + y * v + z * w;
            b1 = x * x + y * y + z * z - obj->paras[0] * obj->paras[0];
            return _do_calc_dist(a1, b1, is_at_surf);
        }
        case S:{
            t1 = x - obj->paras[0];
            t2 = y - obj->paras[1];
            t3 = z - obj->paras[2];
            a1 = t1 * u + t2 * v + t3 * w;
            b1 = t1 * t1 + t2 * t2 + t3 * t3 - obj->paras[3] * obj->paras[3];
            return _do_calc_dist(a1, b1, is_at_surf);
        }
        case SX:{
            t1 = x - obj->paras[0];
            a1 = t1 * u + y * v + z * w;
            b1 = t1 * t1 + y * y + z * z - obj->paras[1] * obj->paras[1];
            return _do_calc_dist(a1, b1, is_at_surf);
        }

        case SY:{
            t2 = y - obj->paras[0];
            a1 = x * u + t2 * v + z * w;
            b1 = x * x + t2 * t2 + z * z - obj->paras[1] * obj->paras[1];
            return _do_calc_dist(a1, b1, is_at_surf);
        }
        case SZ:{
            t3 = z - obj->paras[0];
            a1 = x * u + y * v + t3 * w;
            b1 = x * x + y * y + t3 * t3 - obj->paras[1] * obj->paras[1];
            return _do_calc_dist(a1, b1, is_at_surf);
        }
        case C_X: {
            t1 = SQUARE(v) + SQUARE(w);
            if(EQ_ZERO(t1))
                return dts;
            t1 = ONE / t1;
            t2 = y - obj->paras[0];
            t3 = z - obj->paras[1];
            a1 = (t2 * v + t3 * w) * t1;
            b1 = (t2 * t2 + t3 * t3 - obj->paras[2] * obj->paras[2]) * t1;
            return _do_calc_dist(a1, b1, is_at_surf);
        }
        case C_Y: {
            t1 = SQUARE(u) + SQUARE(w);
            if(EQ_ZERO(t1))
                return dts;
            t1 = ONE / t1;
            t2 = x - obj->paras[0];
            t3 = z - obj->paras[1];
            a1 = (t2 * u + t3 * w) * t1;
            b1 = (t2 * t2 + t3 * t3 - obj->paras[2] * obj->paras[2]) * t1;
            return _do_calc_dist(a1, b1, is_at_surf);
        }
        case C_Z: {
            t1 = SQUARE(u) + SQUARE(v);
            if(EQ_ZERO(t1))
                return dts;
            t1 = ONE / t1;
            t2 = x - obj->paras[0];
            t3 = y - obj->paras[1];
            a1 = (t2 * u + t3 * v) * t1;
            b1 = (t2 * t2 + t3 * t3 - obj->paras[2] * obj->paras[2]) * t1;
            return _do_calc_dist(a1, b1, is_at_surf);
        }
        case CX:{
            t1 = SQUARE(v) + SQUARE(w);
            if(EQ_ZERO(t1))
                return dts;
            t1 = ONE / t1;
            a1 = (y * v + z * w) * t1;
            b1 = (y * y + z * z - obj->paras[0] * obj->paras[0]) * t1;
            return _do_calc_dist(a1, b1, is_at_surf);
        }
        case CY:{
            t1 = SQUARE(u) + SQUARE(w);
            if(EQ_ZERO(t1))
                return dts;
            t1 = ONE / t1;
            a1 = (x * u + z * w) * t1;
            b1 = (x * x + z * z - obj->paras[0] * obj->paras[0]) * t1;
            return _do_calc_dist(a1, b1, is_at_surf);
        }
        case CZ:{
            t1 = SQUARE(u) + SQUARE(v);
            if(EQ_ZERO(t1))
                return dts;
            t1 = ONE / t1;
            a1 = (x * u + y * v) * t1;
            b1 = (x * x + y * y - obj->paras[0] * obj->paras[0]) * t1;

            return _do_calc_dist(a1, b1, is_at_surf);
        }
    }
}

/* 解形如 t^2+2*a1*t+b1=0 的二次方程 */
double _do_calc_dist(double a1, double b1, bool is_at_surf){
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