//
// Created by xaq on 2018/1/29.
//

#include "surface.h"


void
reflect_par(surface_t *obj,
            const double pos[3],
            double *dir,
            double *loc_dir)
{
    double surf_norm_vec[3];
    obj->funcs->get_surf_norm_vec(obj->paras, pos, surf_norm_vec);

    double temp = dir[0] * surf_norm_vec[0] + dir[1] * surf_norm_vec[1] + dir[2] * surf_norm_vec[2];
    if(LT_ZERO(temp)) {
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
    temp = ONE / dir_length;
    dir[0] *= temp;
    dir[1] *= temp;
    dir[2] *= temp;

    double loc_dir_lenth = LENGTH_VECTOR(loc_dir);
    temp = ONE / loc_dir_lenth;
    loc_dir[0] *= temp;
    loc_dir[1] *= temp;
    loc_dir[2] *= temp;
}