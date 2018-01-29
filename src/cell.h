//
// Created by xaq on 9/8/17.
//

#ifndef CRITI_CELL_H
#define CRITI_CELL_H

#include "common.h"


typedef struct {
    int id;               /* cell id */
    int fill;             /* fill 关键字 */
    int mat;              /* mat 关键字 */
    int imp;              /* imp 关键字 */
    double tmp;           /* tmp 关键字 */
    double vol;           /* vol 关键字 */
    bool is_inner_cell;   /* inner 关键字 */
    char *rpn;            /* 面布尔表达式转换而来的RPN表达式 */
    bool simple;          /* 是否是只含有集合交运算的简单cell */

    int *surfs;         /* 当前cell的全部边界面编号，包含符号在内 */
    int surfs_sz;
} cell_t;

#define CELL_KW_NUMBER        7
#define CELL_MAX_KW_LENGTH    6

static const char cell_kw[CELL_KW_NUMBER][CELL_MAX_KW_LENGTH] = {
        "FILL",
        "MAT",
        "IMP",
        "TMP",
        "VOL",
        "INNER",
        "VOID"
};

#ifdef __cplusplus
extern "C" {
#endif

cell_t *cell_init();

bool particle_is_in_cell(const cell_t *obj, const double pos[3], const double dir[3]);

void cell_free(cell_t *obj);

#ifdef __cplusplus
}
#endif

#endif //CRITI_CELL_H
