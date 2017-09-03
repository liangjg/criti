//
// Created by xaq on 9/3/17.
//

#include "surface.h"

void reflect_par(surface_t *obj, const double pos[3], double *dir, double *loc_dir){
    double surf_norm_vec[3];
    get_surf_norm_vec(obj, pos, surf_norm_vec);

    double temp = dir[0] * surf_norm_vec[0] + dir[1] * surf_norm_vec[1] + dir[2] * surf_norm_vec[2];
    if(temp < 0.0){
        surf_norm_vec[0] = -surf_norm_vec[0];
        surf_norm_vec[1] = -surf_norm_vec[1];
        surf_norm_vec[2] = -surf_norm_vec[2];
        temp = -temp;
    }

    surf_norm_vec[0] = surf_norm_vec[0] * 2 * temp;
    surf_norm_vec[1] = surf_norm_vec[1] * 2 * temp;
    surf_norm_vec[2] = surf_norm_vec[2] * 2 * temp;

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