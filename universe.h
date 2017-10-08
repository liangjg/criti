//
// Created by xaq on 10/7/17.
//

#ifndef TTYW_UNIVERSE_H
#define TTYW_UNIVERSE_H

#include "common.h"

typedef struct {
    int contain_cell_num;
    int *fill_cells;

    double origin[3];
    double rotation[3][3];

    bool is_moved;
    bool is_rotated;
    bool is_lattice;
    int lattice_type;
    double pitch[3];
    int scope[3];
    double sita;
    double sin_sita;
    double cos_sita;
    double height;

    int *fill_lat_univese;
    int filled_lat_num;
} universe_t;

/////////////////// hexagon lattice ///////////////////////
///             E                                       ///
///             *               / b1 = (L1, 0)          ///
///           *   *            /                        ///
///      F  *       * D       /                         ///
///         *   O   *        /------> b2=(0.5*L1, H)    ///
///      A  *       * C                                 ///
///           *   *                                     ///
///             *            <FA,AB> = sita             ///
///             B                                       ///
///  1: FA ,  2: AB ,  3: BC ,  4: CD , 5: DE ,  6: EF  ///
///////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

universe_t *univ_init();
void trans_univ_coord(universe_t *obj, const double pos[3], const double dir[3]);
void trans_univ_dir(universe_t *obj, const double dir[3]);
int find_lat_index(universe_t *obj, const double pos[3], const double dir[3]);
int find_lat_index_rect(universe_t *obj, const double pos[3], const double dir[3]);
int find_lat_index_hex(universe_t *obj, const double pos[3], const double dir[3]);
void move_to_origin_lat(universe_t *obj, int lat_index, double pos[3]);
double calc_dist_to_lat(universe_t *obj, const double pos[3], const double dir[3], int which_surf);
double calc_dist_to_lat_rect(universe_t *obj, const double pos[3], const double dir[3], int which_surf);
double calc_dist_to_lat_hex(universe_t *obj, const double pos[3], const double dir[3], int which_surf);

#ifdef __cplusplus
}
#endif

#endif //TTYW_UNIVERSE_H
