//
// Created by xaq on 9/3/17.
//

#include "surface.h"

double calc_dist_to_surf_sub(double a1, double b1, bool is_at_surf){
    double t1 = a1 * a1 - b1;
    if(t1 < 0.)
        return -ONE;
    if(is_at_surf)
        return -TWO * a1;
    else {
        double t2 = sqrt(t1);
        double d1 = -a1 + t2;
        if(d1 <= 0.)
            return -ONE;
        double d2 = -a1 - t2;

        return d2 > 1.0E-10 ? d2 : d1;
    }
}

double calc_dist_to_surf(surface_t *obj, const double pos[3], const double dir[3], bool is_at_surf){
    double x, y, z, u, v, w;
    x = pos[0];
    y = pos[1];
    z = pos[2];
    u = dir[0];
    v = dir[1];
    w = dir[2];
    double t1, t2, t3, t4, a1, b1, uu;

    double dts = -ONE;

    switch(obj->type){
        case P:{
            t1 = obj->paras[0] * u + obj->paras[1] * v + obj->paras[2] * w;
            if(t1 > -EPSILON && t1 < EPSILON)
                return dts;
            else {
                t2 = obj->paras[0] * x + obj->paras[1] * y + obj->paras[2] * z;
                dts = (obj->paras[3] - t2) / t1;
                return dts;
            }
        }
        case PX:{
            if(u > -EPSILON && u < EPSILON)
                return dts;
            else {
                dts = (obj->paras[0] - x) / u;
                return dts;
            }
        }
        case PY:{
            if(v > -EPSILON && v < EPSILON)
                return dts;
            else {
                dts = (obj->paras[0] - y) / v;
                return dts;
            }
        }
        case PZ:{
            if(w > -EPSILON && w < EPSILON)
                return dts;
            else {
                dts = (obj->paras[0] - z) / w;
                return dts;
            }
        }
        case SO:{
            a1 = x * u + y * v + z * w;
            b1 = x * x + y * y + z * z - obj->paras[0] * obj->paras[0];
            return calc_dist_to_surf_sub(a1, b1, is_at_surf);
        }
        case S:{
            t1 = x - obj->paras[0];
            t2 = y - obj->paras[1];
            t3 = z - obj->paras[2];
            a1 = t1 * u + t2 * v + t3 * w;
            b1 = t1 * t1 + t2 * t2 + t3 * t3 - obj->paras[3] * obj->paras[3];
            return calc_dist_to_surf_sub(a1, b1, is_at_surf);
        }
        case SX:{
            t1 = x - obj->paras[0];
            a1 = t1 * u + y * v + z * w;
            b1 = t1 * t1 + y * y + z * z - obj->paras[1] * obj->paras[1];
            return calc_dist_to_surf_sub(a1, b1, is_at_surf);
        }

        case SY:{
            t2 = y - obj->paras[0];
            a1 = x * u + t2 * v + z * w;
            b1 = x * x + t2 * t2 + z * z - obj->paras[1] * obj->paras[1];
            return calc_dist_to_surf_sub(a1, b1, is_at_surf);
        }
        case SZ:{
            t3 = z - obj->paras[0];
            a1 = x * u + y * v + t3 * w;
            b1 = x * x + y * y + t3 * t3 - obj->paras[1] * obj->paras[1];
            return calc_dist_to_surf_sub(a1, b1, is_at_surf);
        }
        case CX:{
            t1 = v * v + w * w;
            if(t1 > -EPSILON && t1 < EPSILON)
                return dts;
            t1 = 1. / t1;
            a1 = (y * v + z * w) * t1;
            b1 = (y * y + z * z - obj->paras[0] * obj->paras[0]) * t1;
            return calc_dist_to_surf_sub(a1, b1, is_at_surf);
        }
        case CY:{
            t1 = u * u + w * w;
            if(t1 > -EPSILON && t1 < EPSILON)
                return dts;
            t1 = 1. / t1;
            a1 = (x * u + z * w) * t1;
            b1 = (x * x + z * z - obj->paras[0] * obj->paras[0]) * t1;
            return calc_dist_to_surf_sub(a1, b1, is_at_surf);
        }
        case CZ:{
            t1 = u * u + v * v;
            if(t1 > -EPSILON && t1 < EPSILON)
                return dts;
            t1 = 1. / t1;
            a1 = (x * u + y * v) * t1;
            b1 = (x * x + y * y - obj->paras[0] * obj->paras[0]) * t1;

            return calc_dist_to_surf_sub(a1, b1, is_at_surf);
        }
        default:{
            puts("unknown surface type.");
            exit(0);
        }
    }
}