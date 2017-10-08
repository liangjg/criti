//
// Created by xaq on 10/7/17.
//

#include "universe.h"

universe_t *univ_init() {
    universe_t *_new_univ = new universe_t;
    _new_univ->contain_cell_num = 0;
    _new_univ->is_lattice = false;
    _new_univ->is_moved = false;
    _new_univ->is_rotated = false;
    _new_univ->cos_sita = 0.0;
    _new_univ->sin_sita = 0.0;
    _new_univ->sita = 0.0;
    _new_univ->fill_cells = nullptr;
    _new_univ->fill_lat_univese = nullptr;
    _new_univ->filled_lat_num = 0;
    _new_univ->height = 0.0;
    _new_univ->lattice_type = 0;
    return _new_univ;
}