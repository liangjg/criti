//
// Created by xaq on 9/3/17.
//

#include "surface.h"

surface_t *surf_init(){
    surface_t *_new_surf = (surface_t *) malloc(sizeof(surface_t));
    memset(_new_surf, 0x0, sizeof(surface_t));
    return _new_surf;
}

int calc_surf_sense(surface_t *obj, const double pos[3], const double dir[3]){
    double sense;
    double x = pos[0];
    double y = pos[1];
    double z = pos[2];

    switch(obj->type){
        case P:{
            sense = obj->paras[0] * x + obj->paras[1] * y + obj->paras[2] * z - obj->paras[3];
            break;
        }
        case PX:{
            sense = x - obj->paras[0];
            break;
        }
        case PY:{
            sense = y - obj->paras[0];
            break;
        }
        case PZ:{
            sense = z - obj->paras[0];
            break;
        }
        case SO:{
            sense = SQUARE(x) + SQUARE(y) + SQUARE(z) - SQUARE(obj->paras[0]);
            break;
        }
        case S:{
            sense = SQUARE(x - obj->paras[0]) + SQUARE(y - obj->paras[1]) + SQUARE(z - obj->paras[2]) - SQUARE(obj->paras[3]);
            break;
        }
        case SX:{
            sense = SQUARE(x - obj->paras[0]) + SQUARE(y) + SQUARE(z) - SQUARE(obj->paras[1]);
            break;
        }
        case SY:{
            sense = SQUARE(x) + SQUARE(y - obj->paras[0]) + SQUARE(z) - SQUARE(obj->paras[1]);
            break;
        }
        case SZ:{
            sense = SQUARE(x) + SQUARE(y) + SQUARE(z - obj->paras[0]) - SQUARE(obj->paras[1]);
            break;
        }
        case CX:{
            sense = SQUARE(y) + SQUARE(z) - SQUARE(obj->paras[0]);
            break;
        }
        case CY:{
            sense = SQUARE(x) + SQUARE(z) - SQUARE(obj->paras[0]);
            break;
        }
        case CZ:{
            sense = SQUARE(x) + SQUARE(y) - SQUARE(obj->paras[0]);
            break;
        }
        default:{
            puts("unknown surface type.");
            exit(0);
        }
    }

    if(fabs(sense) < EPSILON){
        double norm_vec[3];
        get_surf_norm_vec(obj, pos, norm_vec);
        sense = ZERO;
        for(int i = 0; i < 3; i++)
            sense += dir[i] * norm_vec[i];
    }

    if(GT_ZERO(sense)) return 1;
    else if(LT_ZERO(sense)) return -1;
    else return 0;
}

void get_surf_norm_vec(surface_t *obj, const double pos[3], double *surf_norm_vec){
    double x = pos[0];
    double y = pos[1];
    double z = pos[2];

    switch(obj->type){
        case P: {
            surf_norm_vec[0] = obj->paras[0];
            surf_norm_vec[1] = obj->paras[1];
            surf_norm_vec[2] = obj->paras[2];
            break;
        }
        case PX: {
            surf_norm_vec[0] = ONE;
            surf_norm_vec[1] = ZERO;
            surf_norm_vec[2] = ZERO;
            break;
        }
        case PY: {
            surf_norm_vec[0] = ZERO;
            surf_norm_vec[1] = ONE;
            surf_norm_vec[2] = ZERO;
            break;
        }
        case PZ: {
            surf_norm_vec[0] = ZERO;
            surf_norm_vec[1] = ZERO;
            surf_norm_vec[2] = ONE;
            break;
        }
        case SO: {
            surf_norm_vec[0] = x;
            surf_norm_vec[1] = y;
            surf_norm_vec[2] = z;
            break;
        }
        case S: {
            surf_norm_vec[0] = x - obj->paras[0];
            surf_norm_vec[1] = y - obj->paras[1];
            surf_norm_vec[2] = z - obj->paras[2];
            break;
        }
        case SX: {
            surf_norm_vec[0] = x - obj->paras[0];
            surf_norm_vec[1] = y;
            surf_norm_vec[2] = z;
            break;
        }
        case SY: {
            surf_norm_vec[0] = x;
            surf_norm_vec[1] = y - obj->paras[0];
            surf_norm_vec[2] = z;
            break;
        }
        case SZ: {
            surf_norm_vec[0] = x;
            surf_norm_vec[1] = y;
            surf_norm_vec[2] = z - obj->paras[0];
            break;
        }
        case CX: {
            surf_norm_vec[0] = ZERO;
            surf_norm_vec[1] = y;
            surf_norm_vec[2] = z;
            break;
        }
        case CY: {
            surf_norm_vec[0] = x;
            surf_norm_vec[1] = ZERO;
            surf_norm_vec[2] = z;
            break;
        }
        case CZ: {
            surf_norm_vec[0] = x;
            surf_norm_vec[1] = y;
            surf_norm_vec[2] = ZERO;
            break;
        }
        default:{
            puts("unknown surface type.");
            exit(0);
        }
    }

    double length = ONE / sqrt(SQUARE(surf_norm_vec[0]) + SQUARE(surf_norm_vec[1]) + SQUARE(surf_norm_vec[2]));
    surf_norm_vec[0] *= length;
    surf_norm_vec[1] *= length;
    surf_norm_vec[2] *= length;
}

/* 解形如 t^2+2*a1*t+b1=0 的二次方程 */
double calc_dist_to_surf_sub(double a1, double b1, bool is_at_surf){
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
            if(EQ_ZERO(t1))
                return dts;
            t1 = ONE / t1;
            a1 = (y * v + z * w) * t1;
            b1 = (y * y + z * z - obj->paras[0] * obj->paras[0]) * t1;
            return calc_dist_to_surf_sub(a1, b1, is_at_surf);
        }
        case CY:{
            t1 = u * u + w * w;
            if(EQ_ZERO(t1))
                return dts;
            t1 = ONE / t1;
            a1 = (x * u + z * w) * t1;
            b1 = (x * x + z * z - obj->paras[0] * obj->paras[0]) * t1;
            return calc_dist_to_surf_sub(a1, b1, is_at_surf);
        }
        case CZ:{
            t1 = u * u + v * v;
            if(EQ_ZERO(t1))
                return dts;
            t1 = ONE / t1;
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

void reflect_par(surface_t *obj, const double pos[3], double *dir, double *loc_dir){
    double surf_norm_vec[3];
    get_surf_norm_vec(obj, pos, surf_norm_vec);

    double temp = dir[0] * surf_norm_vec[0] + dir[1] * surf_norm_vec[1] + dir[2] * surf_norm_vec[2];
    if(LT_ZERO(temp)){
        surf_norm_vec[0] = -surf_norm_vec[0];
        surf_norm_vec[1] = -surf_norm_vec[1];
        surf_norm_vec[2] = -surf_norm_vec[2];
        temp = -temp;
    }

    surf_norm_vec[0] = surf_norm_vec[0] * TWO * temp;
    surf_norm_vec[1] = surf_norm_vec[1] * TWO * temp;
    surf_norm_vec[2] = surf_norm_vec[2] * TWO * temp;

    dir[0] -= surf_norm_vec[0];
    dir[1] -= surf_norm_vec[1];
    dir[2] -= surf_norm_vec[2];

    loc_dir[0] -= surf_norm_vec[0];
    loc_dir[1] -= surf_norm_vec[1];
    loc_dir[2] -= surf_norm_vec[2];

    double dir_length = LENGTH_VECTOR(dir);
    dir[0] /= dir_length;
    dir[1] /= dir_length;
    dir[2] /= dir_length;

    double loc_dir_lenth = LENGTH_VECTOR(loc_dir);
    loc_dir[0] /= loc_dir_lenth;
    loc_dir[1] /= loc_dir_lenth;
    loc_dir[2] /= loc_dir_lenth;
}

void surf_free(surface_t *obj){
    free(obj);
}