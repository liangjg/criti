//
// Created by xaq on 10/27/17.
//

#include "common.h"
#include "vector.h"

#ifndef TTYW_PARTICLE_STATE_H
#define TTYW_PARTICLE_STATE_H

typedef struct{
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
    vector loc_univs;           /* 粒子在定位到当前cell的过程中，进入的每一层universe */
    vector loc_cells;           /* 定位到的cell是在loc_univs[i]中的第几个cell；注意，这里是第几个，而不是直接存储的相应cell，存储的是univ->cells这个vector的下标 */
    int cell;                   /* 最终定位到的粒子所在的底层cell */
    int surf;                   /* 粒子如果要穿面的话，穿出的面的序号；注意，这里是带符号的，有正负的 */
    int bound_index;            /* 当前要穿出的面在cell->surfs这个vector中的下标 */
    int bound_level;            /* 穿出的这个面是在第几层universe中 */
    int lat_bound_surf;         /* 1:-x ;  2:+x ;  3:-y;  4:+y */

    /* collision state */
    int mat;                    /* 当前cell的材料 */
    /* TODO: 将nuc和sab_nuc改为字符数组，即char nuc[10]和char sab_nuc[10] */
    int nuc;                    /* 当前发生碰撞的核素，在相应的mat->nuc_id中的下标 */
    int sab_nuc;                /* 和当前碰撞核素相应的热化核素 */
    int collision_type;         /* 当前反应的MT号 */
    double cell_tmp;            /* 当前cell的温度 */

    /* cross sections */
    int interp_N0;              /* Eg0 interpolation position */
    double interp_K0;           /* Eg0 interpolation fraction */
    int interp_N;               /* Erg interpolation position */
    double interp_K;            /* Erg interpolation fraction */
    double macro_tot_cs;
    double macro_nu_fis_cs;

    /* free-gas/sab state */
    bool is_sab_col;            /* sab collision                       */
    bool is_free_gas_col;       /* free gas collision                  */
    double erg_rel;             /* relative energy in free gas model   */
    double dir_vel[3];          /* relative velocity in free gas model */
    double vel_tgt[3];          /* Target velocity in free gas model   */
} particle_state_t;

#ifdef __cplusplus
extern "C"{
#endif

//void Fly_by_length(double length);
#define Fly_by_length(_length)    \
    do{    \
        par_state->pos[0] += par_state->dir[0] * (_length);  \
        par_state->pos[1] += par_state->dir[1] * (_length);  \
        par_state->pos[2] += par_state->dir[2] * (_length);  \
        par_state->loc_pos[0] += par_state->loc_dir[0] * (_length);  \
        par_state->loc_pos[1] += par_state->loc_dir[1] * (_length);  \
        par_state->loc_pos[2] += par_state->loc_dir[2] * (_length);  \
} while(0)

#ifdef __cplusplus
}
#endif

#endif //TTYW_PARTICLE_STATE_H
