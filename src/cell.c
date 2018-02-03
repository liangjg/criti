//
// Created by xaq on 10/7/17.
//

#include "cell.h"


cell_t *cell_init(){
    cell_t *_new_cell = (cell_t *)malloc(sizeof(cell_t));
    _new_cell->mat = 0;
    _new_cell->imp = 1;
    _new_cell->tmp = 2.53E-08;
    _new_cell->vol = 1.0;
    _new_cell->is_inner_cell = false;
    _new_cell->rpn = NULL;
    _new_cell->surfs = NULL;
    _new_cell->surfs_addr = NULL;
    _new_cell->surfs_sz = 0;

    _new_cell->fill = NULL;
    _new_cell->parent = NULL;

    return _new_cell;
}

void cell_free(cell_t *obj){
    free(obj->rpn);
    free(obj->surfs);
    free(obj->surfs_addr);
    free(obj);
}