//
// Created by xaq on 10/7/17.
//

#include "cell.h"

cell_t *cell_init() {
    cell_t *_new_cell = new cell_t;
    _new_cell->fill = -1;
    _new_cell->mat = 0;
    _new_cell->imp = 1;
    _new_cell->tmp = 2.53E-08;
    _new_cell->vol = 1.0;
    _new_cell->is_inner_cell = false;
    _new_cell->rpn = nullptr;
    return _new_cell;
}

bool particle_is_in_cell(const cell_t *obj, const double pos[3], const double dir[3]) {
    return false;
}