//
// Created by xaq on 10/7/17.
//

#include "cell.h"
#include "surface.h"
#include "map.h"
#include "vector.h"


extern map *base_surfs;

/* -------------------------- private prototypes ---------------------------- */
static inline bool _has_same_sign(int a, int b);

static bool _simple_par_in_cell(const cell_t *obj, const double *pos, const double *dir);

static bool _complex_par_in_cell(const cell_t *obj, const double *pos, const double *dir);

/* ----------------------------- API implementation ------------------------- */
cell_t *cell_init(){
    cell_t *_new_cell = (cell_t *)malloc(sizeof(cell_t));
    _new_cell->fill = -1;
    _new_cell->mat = 0;
    _new_cell->imp = 1;
    _new_cell->tmp = 2.53E-08;
    _new_cell->vol = 1.0;
    _new_cell->is_inner_cell = false;
    _new_cell->rpn = NULL;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-arith"
    /* initialize vector surfs */
    _new_cell->surfs.ele_size = sizeof(int);
    _new_cell->surfs.start = malloc((size_t)(8) * sizeof(int));    /* 8 elements by default */
    _new_cell->surfs.finish = _new_cell->surfs.start;
    _new_cell->surfs.end_of_storage = _new_cell->surfs.start + 8 * sizeof(int);
    _new_cell->surfs.value_free = NULL;
#pragma GCC diagnostic pop
    return _new_cell;
}

/* ---------------------------------------------------------
 * 输入：obj: 当前要判断的cell
 *      pos[3], dir[3]: 粒子坐标和方向
 *      surfs: 所有输入的面组成的一个map
 * 输出: 粒子是否在当前cell
 * ---------------------------------------------------------
 * */
bool particle_is_in_cell(const cell_t *obj, const double pos[3], const double dir[3]){
    if(obj->simple) return _simple_par_in_cell(obj, pos, dir);
    else return _complex_par_in_cell(obj, pos, dir);
}

void cell_free(cell_t *obj){
    free(obj->rpn);
    vector_free(&obj->surfs);
    free(obj);
}

/* ------------------------ private API implementation ---------------------- */
inline bool _has_same_sign(int a, int b){
    return ((a ^ b) & 0x80000000) == 0;
}

bool _simple_par_in_cell(const cell_t *obj, const double pos[3], const double dir[3]){
    bool in_cell = true;
    register char *c = obj->rpn;
    int surf_index = 0;
    surface_t *surf;
    int surf_sense;
    register map *local_base_surfs = base_surfs;

    while(*c != '\0'){
        if(ISNUMBER(*c)){
            surf_index = 0;
            do{
                surf_index *= 10;
                surf_index += *c - '0';
                c++;
            } while(ISNUMBER(*c));
            surf = (surface_t *) map_get(local_base_surfs, surf_index);
            surf_sense = calc_surf_sense(surf, pos, dir);
            in_cell = _has_same_sign(surf_index, surf_sense);
            if(!in_cell) break;
        } else if(*c == '-'){
            surf_index = 0;
            c++;
            while(ISNUMBER(*c)){
                surf_index *= 10;
                surf_index += *c - '0';
                c++;
            }
            surf = (surface_t *) map_get(local_base_surfs, surf_index);
            surf_index = (~surf_index) + 1;
            surf_sense = calc_surf_sense(surf, pos, dir);
            in_cell = _has_same_sign(surf_index, surf_sense);
            if(!in_cell) break;
        } else c++;
    }
    return in_cell;
}

bool _complex_par_in_cell(const cell_t *obj, const double *pos, const double *dir){
    bool in_cell;
    bool st[16];
    int i_stack;
    int surf_index;
    surface_t *surf;
    int surf_sense;
    register char *c;
    register map *local_base_surfs = base_surfs;

    c = obj->rpn;
    i_stack = -1;

    while(*c != '\0'){
        if(ISNUMBER(*c)){
            i_stack++;
            surf_index = 0;
            do{
                surf_index *= 10;
                surf_index += *c - '0';
                c++;
            } while(ISNUMBER(*c));
            surf = (surface_t *) map_get(local_base_surfs, surf_index);
            surf_sense = calc_surf_sense(surf, pos, dir);
            in_cell = _has_same_sign(surf_index, surf_sense);
            st[i_stack] = in_cell;
        } else if(*c == '-'){
            i_stack++;
            surf_index = 0;
            c++;
            while(ISNUMBER(*c)){
                surf_index *= 10;
                surf_index += *c - '0';
                c++;
            }
            surf = (surface_t *) map_get(local_base_surfs, surf_index);
            surf_index = (~surf_index) + 1;
            surf_sense = calc_surf_sense(surf, pos, dir);
            in_cell = _has_same_sign(surf_index, surf_sense);
            st[i_stack] = in_cell;
        } else if(*c == '&'){
            st[i_stack - 1] = st[i_stack] && st[i_stack - 1];
            i_stack--;
            c++;
        } else if(*c == ':'){
            st[i_stack - 1] = st[i_stack] || st[i_stack - 1];
            i_stack--;
            c++;
        } else if(*c == '!'){
            st[i_stack] = !st[i_stack];
            c++;
        } else c++;
    }

    /* i_stack等于-1只可能是因为RPN表达式为空；如果RPN表达式非空的话，最后的结果就是st[0] */
    in_cell = (i_stack == -1) ? true : st[0];

    return in_cell;
}