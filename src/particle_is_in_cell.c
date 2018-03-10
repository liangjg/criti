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
    bool in_cell = true;
    int surf_index = 0;
    surface_t *surf;
    int surf_sense;
    int surfs_sz = obj->surfs_sz;

    for(int i = 0; i < surfs_sz; i++) {
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
    int index = 0;
    register char *c;

    c = obj->rpn;
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

//bool _complex_par_in_cell(const cell_t *obj, const double pos[3], const double dir[3]){
//    bool st[16];
//    char symbols[16];
//    int i_stack, surfs_ptr, symbols_ptr;
//    surface_t *surf;
//    int *surfs_sense, *surfs_index;
//    int surfs_sz;
//    int symbol_cnt;
//    char *c;
//
//    surfs_sz = obj->surfs_sz;
//    surfs_sense = (int *) malloc(surfs_sz * sizeof(int));
//    surfs_index = obj->surfs;
//
//    for(int i = 0; i < surfs_sz; i++){
//        surf = obj->surfs_addr[i];
//        surfs_sense[i] = calc_surf_sense(surf, pos, dir);
//    }
//
//    c = obj->rpn;
//    symbol_cnt = 0;
//
//    while(*c != '\0'){
//        if(*c == '&' || *c == ':' || *c == '!')
//            symbols[symbol_cnt++] = *c;
//        c++;
//    }
//
//    i_stack = -1;
//    surfs_ptr = 0;
//    symbols_ptr = 0;
//
//    while(surfs_ptr < surfs_sz || symbols_ptr < symbol_cnt){
//        switch(symbols[symbols_ptr++]){
//            case '&':
//                if(surfs_ptr < surfs_sz){
//                    i_stack++;
//                    st[i_stack++] = _has_same_sign(surfs_sense[surfs_ptr], surfs_index[surfs_ptr]);
//                    surfs_ptr++;
//                    st[i_stack] = _has_same_sign(surfs_sense[surfs_ptr], surfs_index[surfs_ptr]);
//                    surfs_ptr++;
//                }
//                st[i_stack - 1] = st[i_stack] && st[i_stack - 1];
//                i_stack--;
//                break;
//            case ':':
//                if(surfs_ptr < surfs_sz){
//                    i_stack++;
//                    st[i_stack++] = _has_same_sign(surfs_sense[surfs_ptr], surfs_index[surfs_ptr]);
//                    surfs_ptr++;
//                    st[i_stack] = _has_same_sign(surfs_sense[surfs_ptr], surfs_index[surfs_ptr]);
//                    surfs_ptr++;
//                }
//                st[i_stack - 1] = st[i_stack] || st[i_stack - 1];
//                i_stack--;
//                break;
//            default:
//                st[i_stack] = !st[i_stack];
//                break;
//        }
//    }
//    free(surfs_sense);
//    return st[i_stack];
//}