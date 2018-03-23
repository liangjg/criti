//
// Created by xaq on 2018/1/29.
//

#include "cell.h"
#include "surface.h"


/* -------------------------- private prototypes ---------------------------- */
static inline bool
_has_same_sign(int a,
               int b);

static bool
_simple_par_in_cell(const cell_t *obj,
                    const double *pos,
                    const double *dir);

static bool
_complex_par_in_cell(const cell_t *obj,
                     const double *pos,
                     const double *dir);

/* ----------------------------- API implementation ------------------------- */

/* ***************************************************************************
 * 输入：obj: 当前要判断的cell
 *      pos[3], dir[3]: 粒子坐标和方向
 * 输出: 粒子是否在当前cell
 * ***************************************************************************/
bool
particle_is_in_cell(const cell_t *obj,
                    const double pos[3],
                    const double dir[3])
{
    if(obj->simple) return _simple_par_in_cell(obj, pos, dir);
    else return _complex_par_in_cell(obj, pos, dir);
}

/* ------------------------ private API implementation ---------------------- */
inline bool
_has_same_sign(int a,
               int b)
{
    return ((a ^ b) & 0x80000000) == 0;
}

bool
_simple_par_in_cell(const cell_t *obj,
                    const double pos[3],
                    const double dir[3])
{
    int i;
    surface_t *surf;
    int surf_sense;
    int surfs_sz = obj->surfs_sz;
    bool in_cell = true;
    int surf_index = 0;

    for(i = 0; i < surfs_sz; i++) {
        surf_index = obj->surfs[i];
        surf = obj->surfs_addr[i];
        surf_sense = calc_surf_sense(surf, pos, dir);
        in_cell = _has_same_sign(surf_index, surf_sense);
        if(!in_cell) break;
    }
    return in_cell;
}

bool
_complex_par_in_cell(const cell_t *obj,
                     const double *pos,
                     const double *dir)
{
    bool in_cell;
    bool st[16];
    int i_stack;
    int surf_index;
    surface_t *surf;
    int surf_sense;
    int index;
    char *c;

    c = obj->rpn;
    index = 0;
    i_stack = -1;

    while(*c != '\0') {
        if(ISNUMBER(*c)) {
            i_stack++;
            surf_index = 0;
            do {
                surf_index *= 10;
                surf_index += *c - '0';
                c++;
            } while(ISNUMBER(*c));
            surf = obj->surfs_addr[index++];
            surf_sense = calc_surf_sense(surf, pos, dir);
            in_cell = _has_same_sign(surf_index, surf_sense);
            st[i_stack] = in_cell;
        } else if(*c == '-') {
            i_stack++;
            surf_index = 0;
            c++;
            while(ISNUMBER(*c)) {
                surf_index *= 10;
                surf_index += *c - '0';
                c++;
            }
            surf = obj->surfs_addr[index++];
            surf_index = (~surf_index) + 1;
            surf_sense = calc_surf_sense(surf, pos, dir);
            in_cell = _has_same_sign(surf_index, surf_sense);
            st[i_stack] = in_cell;
        } else if(*c == '&') {
            st[i_stack - 1] = st[i_stack] && st[i_stack - 1];
            i_stack--;
            c++;
        } else if(*c == ':') {
            st[i_stack - 1] = st[i_stack] || st[i_stack - 1];
            i_stack--;
            c++;
        } else if(*c == '!') {
            st[i_stack] = !st[i_stack];
            c++;
        } else c++;
    }

    /* i_stack等于-1只可能是因为RPN表达式为空；如果RPN表达式非空的话，最后的结果就是st[0] */
    in_cell = (i_stack == -1) ? true : st[0];

    return in_cell;
}
