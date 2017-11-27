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
    double wgt;
    double pos[3];              /* xyz position in Universe 0 */
    double dir[3];
    double exit_dir[3];
    double erg;
    double exit_erg;
    double loc_pos[3];          /* xyz position in local Universe. It changes when new Bound is found */
    double loc_dir[3];          /* uvw direction in local Universe. */

    /* geometry state */
    vector loc_univs_v;
    vector loc_cells_v;
    int *loc_univs;             /* Universes where the Particle is located */
    int loc_univs_sz;
    int *loc_cells;             /* The i-th cell in universe where the Particle is located. ==>  Universe[LocUnivs].ContainCells[LocCells] */
    int loc_cells_sz;
    int cell;                   /* CELL in maximum-level universe where the Particle is located */
    int bound_level;            /* Boundary is in the i-th level universe                       */
    int bound_surf;             /* Boundary surface index                                       */
    int bound_index;            /* Boundary is the i-th surface of the CELL                     */
    int lat_bound_surf;         /* 1:-x ;  2:+x ;  3:-y;  4:+y                                  */

    /* collision state */
    int mat;                    /* Current cell material                                  */
    int nuc;                    /* Current collision nuclide                              */
    int sab_nuc;                /* Sab nuclide corresponding to current collision nuclide */
    int collision_type;         /* Current reaction MT number                             */
    double cell_tmp;            /* Current cell temperature                               */

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
    double erg_rel;             /* relative energ in free gas model    */
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
