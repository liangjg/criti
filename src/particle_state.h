//
// Created by xaq on 10/27/17.
//

#include "common.h"

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
    double macro_tot_xs;
    double macro_mu_fis_xs;

} particle_state_t;

#ifdef __cplusplus
extern "C"{
#endif

//void Fly_by_length(double length);
#define Fly_by_length(_length)    \
    do{    \
        base_par_state.pos[0] += base_par_state.dir[0] * (_length);  \
        base_par_state.pos[1] += base_par_state.dir[1] * (_length);  \
        base_par_state.pos[2] += base_par_state.dir[2] * (_length);  \
        base_par_state.loc_pos[0] += base_par_state.loc_dir[0] * (_length);  \
        base_par_state.loc_pos[1] += base_par_state.loc_dir[1] * (_length);  \
        base_par_state.loc_pos[2] += base_par_state.loc_dir[2] * (_length);  \
} while(0)

#ifdef __cplusplus
}
#endif

#endif //TTYW_PARTICLE_STATE_H
