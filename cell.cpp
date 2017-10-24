//
// Created by xaq on 10/7/17.
//

#include "cell.h"
#include "surface.h"
#include "stack.h"

/* -------------------------- private prototypes ---------------------------- */
static inline bool _has_same_sign(int a, int b);

/* ----------------------------- API implementation ------------------------- */
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

/* ---------------------------------------------------------
 * 输入：obj: 当前要判断的cell
 *      pos[3], dir[3]: 粒子坐标和方向
 *      surfs: 所有输入的面组成的一个map
 * 输出: 粒子是否在当前cell
 * ---------------------------------------------------------
 * */
bool particle_is_in_cell(const cell_t *obj, const double pos[3], const double dir[3], map *surfs) {
    const char *rpn = obj->rpn;
    stack *opnd = stack_init(sizeof(int));
    stack *res = stack_init(sizeof(bool));
    while(*rpn != '\0'){
        if(ISNUMBER(*rpn)){
            int res = *rpn - '0';
            rpn++;
            while(ISNUMBER(*rpn)){
                res *= 10;
                res += *rpn - '0';
                rpn++;
            }
            stack_push(opnd, &res);
        } else if(*rpn == '-'){
            int res = 0;
            rpn++;
            while(ISNUMBER(*rpn)){
                res *= 10;
                res += *rpn - '0';
                rpn++;
            }
            res = (~res) + 1;
            stack_push(opnd, &res);
        } else if(ISSPACE(*rpn)) continue;
        else {
            switch(*rpn){
                case '&':{
                    if(stack_size(res) == 0){
                        int surf1 = *(int *) stack_top(opnd);
                        stack_pop(opnd);
                        surface_t *s1 = (surface_t *) map_get(surfs, abs(surf1));
                        int surf1_sense = calc_surf_sense(s1, pos, dir);
                        bool res1 = _has_same_sign(surf1, surf1_sense);

                        int surf2 = *(int *) stack_top(opnd);
                        stack_pop(opnd);
                        if(res1){    /* res1 is true, we need to judge res2 */
                            surface_t *s2 = (surface_t *) map_get(surfs, abs(surf2));
                            int surf2_sense = calc_surf_sense(s2, pos, dir);
                            bool res2 = _has_same_sign(surf2, surf2_sense);
                            res1 = res1 && res2;
                        }
                        /* else res1 is false, the final result is false too */
                        stack_push(res, &res1);
                    }
                    else{
                        int surf1 = *(int *) stack_top(opnd);
                        stack_pop(opnd);
                        surface_t *s1 = (surface_t *) map_get(surfs, surf1);
                        int surf1_sense = calc_surf_sense(s1, pos, dir);
                        bool res1 = _has_same_sign(surf1, surf1_sense);

                        bool res2 = *(bool *)stack_top(res);

                        res1 = res1 && res2;
                        stack_push(res, &res1);
                    }

                    rpn++;
                    break;
                }
                case ':':{
                    if(stack_size(res) == 0){
                        int surf1 = *(int *) stack_top(opnd);
                        stack_pop(opnd);
                        surface_t *s1 = (surface_t *) map_get(surfs, abs(surf1));
                        int surf1_sense = calc_surf_sense(s1, pos, dir);
                        bool res1 = _has_same_sign(surf1, surf1_sense);

                        int surf2 = *(int *) stack_top(opnd);
                        stack_pop(opnd);
                        if(!res1){    /* res1 is false, we need to judge res2 */
                            surface_t *s2 = (surface_t *) map_get(surfs, abs(surf2));
                            int surf2_sense = calc_surf_sense(s2, pos, dir);
                            bool res2 = _has_same_sign(surf2, surf2_sense);
                            res1 = res1 || res2;
                        }
                        /* else res1 is true, the final result is false too */
                        stack_push(res, &res1);
                    }
                    else{
                        int surf1 = *(int *) stack_top(opnd);
                        stack_pop(opnd);
                        surface_t *s1 = (surface_t *) map_get(surfs, surf1);
                        int surf1_sense = calc_surf_sense(s1, pos, dir);
                        bool res1 = _has_same_sign(surf1, surf1_sense);

                        bool res2 = *(bool *)stack_top(res);

                        res1 = res1 || res2;
                        stack_push(res, &res1);
                    }

                    rpn++;
                    break;
                }
                case '!':{
                    if(stack_size(res) == 0){
                        int surf1 = *(int *) stack_top(opnd);
                        stack_pop(opnd);
                        surface_t *s1 = (surface_t *) map_get(surfs, abs(surf1));
                        int surf1_sense = calc_surf_sense(s1, pos, dir);
                        bool res1 = _has_same_sign(surf1, surf1_sense);

                        res1 = !res1;
                        stack_push(res, &res1);
                    }
                    else{
                        bool res1 = *(bool *)stack_top(res);

                        res1 = !res1;
                        stack_push(res, &res1);
                    }

                    rpn++;
                    break;
                }
                default:
                    puts("unknown symbol.");
                    break;
            }
        }
    }
    bool final_res = *(bool *) stack_top(res);
    stack_free(opnd);
    stack_free(res);
    return final_res;
}

/* ------------------------ private API implementation ---------------------- */
inline bool _has_same_sign(int a, int b) {
    return (a ^ b) & 0x80000000 == 0;
}