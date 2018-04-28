//
// Created by xaq on 10/27/17.
//

#ifndef CRITI_PARTICLE_STATE_H
#define CRITI_PARTICLE_STATE_H

#include "common.h"


typedef struct particle_status_t {
    /* basic state */
    bool is_killed;
    double wgt;                 /* 粒子当前的权重 */
    double pos[3];              /* 粒子在universe 0中的位置坐标 */
    double dir[3];              /* 粒子在universe 0中的方向坐标 */
    double exit_dir[3];
    double erg;                 /* 粒子当前的能量 */
    double exit_erg;
    double loc_pos[3];          /* 粒子在当前universe中的坐标 */
    double loc_dir[3];          /* 粒子在当前universe中的方向 */

    /* geometry state */
    void *loc_univs[8];         /* 粒子在定位到当前cell的过程中，进入的每一层universe */
    int loc_cells[8];           /* 定位到的cell是在loc_univs[i]中的第几个cell；注意，这里是第几个，而不是直接存储的相应cell，存储的是univ->cells这个vector的下标 */
    int loc_sz;                 /* 以上两个loc的实际size */
    void *cell;                 /* 最终定位到的粒子所在的底层cell */
    int surf;                   /* 粒子如果要穿面的话，穿出的面的序号；注意，这里是带符号的，有正负的 */
    int bound_index;            /* 当前要穿出的面在cell->surfs这个vector中的下标 */
    int bound_level;            /* 到当前粒子距离最短的面是在第几层universe中(loc_univs中的下标) */
    int lat_bound_surf;         /* 1:-x ;  2:+x ;  3:-y;  4:+y */

    /* collision state */
    void *mat;                  /* 当前cell的材料 */
    void *nuc;                  /* 当前发生碰撞的核素 */
    void *nuc_xs;               /* 当前发生碰撞的核素的截面 */
    void *sab_nuc;              /* 和当前碰撞核素相应的热化核素 */
    double cell_tmp;            /* 当前cell的温度 */
    bool cell_tmp_changed;      /* 粒子穿面之后，所处的材料有没有发生变化 */
    bool mat_changed;           /* 粒子穿面之后，所处的材料温度有没有发生变化 */

    /* cross sections */
    int interp_N0;              /* Eg0 interpolation position */
    int interp_N;               /* Erg interpolation position */
    double interp_K0;           /* Eg0 interpolation fraction */
    double interp_K;            /* Erg interpolation fraction */
    double macro_tot_cs;
    double macro_nu_fis_cs;

    /* free-gas state */
    bool is_free_gas_col;       /* free gas collision                  */
    double erg_rel;             /* relative energy in free gas model   */
    double dir_vel[3];          /* relative velocity in free gas model */
    double vel_tgt[3];          /* Target velocity in free gas model   */
} particle_status_t;

BEGIN_DECL
#define Fly_by_length(_length)    \
    do{    \
        par_status->pos[0] += par_status->dir[0] * (_length);  \
        par_status->pos[1] += par_status->dir[1] * (_length);  \
        par_status->pos[2] += par_status->dir[2] * (_length);  \
        par_status->loc_pos[0] += par_status->loc_dir[0] * (_length);  \
        par_status->loc_pos[1] += par_status->loc_dir[1] * (_length);  \
        par_status->loc_pos[2] += par_status->loc_dir[2] * (_length);  \
    } while(0)
END_DECL

#endif //CRITI_PARTICLE_STATE_H
