//
// Created by xaq on 10/7/17.
//

#include "cell.h"
#include "surface.h"

/* -------------------------- private prototypes ---------------------------- */
static inline bool _has_same_sign(int a, int b);

static bool _simple_par_in_cell(const cell_t *obj, const double *pos, const double *dir, map *surfs);

static bool _complex_par_in_cell(const cell_t *obj, const double *pos, const double *dir, map *surfs);

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
    return _new_cell;
}

/* ---------------------------------------------------------
 * 输入：obj: 当前要判断的cell
 *      pos[3], dir[3]: 粒子坐标和方向
 *      surfs: 所有输入的面组成的一个map
 * 输出: 粒子是否在当前cell
 * ---------------------------------------------------------
 * */
bool particle_is_in_cell(const cell_t *obj, const double pos[3], const double dir[3], map *surfs){
    if(obj->simple) return _simple_par_in_cell(obj, pos, dir, surfs);
    else return _complex_par_in_cell(obj, pos, dir, surfs);
}

/* ------------------------ private API implementation ---------------------- */
inline bool _has_same_sign(int a, int b){
    return ((a ^ b) & 0x80000000) == 0;
}

bool _simple_par_in_cell(const cell_t *obj, const double pos[3], const double dir[3], map *surfs){
    bool in_cell = true;
    char *c = obj->rpn;
    int surf_index = 0;
    surface_t *surf;
    int surf_sense;

    while(*c != '\0'){
        if(ISNUMBER(*c)){
            surf_index = 0;
            do{
                surf_index *= 10;
                surf_index += *c - '0';
                c++;
            } while(ISNUMBER(*c));
            surf = (surface_t *) map_get(surfs, surf_index);
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
            surf = (surface_t *) map_get(surfs, surf_index);
            surf_index = (~surf_index) + 1;
            surf_sense = calc_surf_sense(surf, pos, dir);
            in_cell = _has_same_sign(surf_index, surf_sense);
            if(!in_cell) break;
        } else c++;
    }
    return in_cell;
}

bool _complex_par_in_cell(const cell_t *obj, const double *pos, const double *dir, map *surfs){
    bool in_cell;
//    bitmap *bm;
    bool st[16];
    int i_stack;
    int surf_index;
    surface_t *surf;
    int surf_sense;
    char *c;

    c = obj->rpn;
    i_stack = -1;
//    bm = bitmap_init(strlen(c));

    while(*c != '\0'){
        if(ISNUMBER(*c)){
            i_stack++;
            surf_index = 0;
            do{
                surf_index *= 10;
                surf_index += *c - '0';
                c++;
            } while(ISNUMBER(*c));
            surf = (surface_t *) map_get(surfs, surf_index);
            surf_sense = calc_surf_sense(surf, pos, dir);
            in_cell = _has_same_sign(surf_index, surf_sense);
//            if(in_cell) bitmap_set(bm, i_stack);
            if(in_cell) st[i_stack] = true;
        } else if(*c == '-'){
            i_stack++;
            surf_index = 0;
            c++;
            while(ISNUMBER(*c)){
                surf_index *= 10;
                surf_index += *c - '0';
                c++;
            }
            surf = (surface_t *) map_get(surfs, surf_index);
            surf_index = (~surf_index) + 1;
            surf_sense = calc_surf_sense(surf, pos, dir);
            in_cell = _has_same_sign(surf_index, surf_sense);
//            if(in_cell) bitmap_set(bm, i_stack);
            if(in_cell) st[i_stack] = true;
        } else if(*c == '&'){
//            if(!bitmap_test(bm, i_stack) || !bitmap_test(bm, i_stack - 1))
//                bitmap_clear(bm, --i_stack);
            st[i_stack - 1] = st[i_stack] && st[i_stack - 1];
            i_stack--;
            c++;
        } else if(*c == ':'){
//            if(bitmap_test(bm, i_stack) || bitmap_test(bm, i_stack - 1))
//                bitmap_set(bm, --i_stack);
            st[i_stack - 1] = st[i_stack] || st[i_stack - 1];
            i_stack--;
            c++;
        } else if(*c == '!'){
//            bitmap_reverse(bm, i_stack);
            st[i_stack] = ~st[i_stack];
            c++;
        } else c++;
    }

    /* i_stack remains -1 only if there is no region specified */
    /* The only one remained on the top of stack indicates whether the particle is in the cell */
//    in_cell = (i_stack == -1 ? true : bitmap_test(bm, 0));
//    bitmap_free(bm);
    in_cell = (i_stack == -1) ? true : st[0];

    return in_cell;
}