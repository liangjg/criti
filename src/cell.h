//
// Created by xaq on 9/8/17.
//

#ifndef CRITI_CELL_H
#define CRITI_CELL_H

#include "common.h"


typedef struct cell_t {
    int id;                    /* cell id */
    int imp;                   /* imp关键字 */
    int surfs_sz;              /* 布尔表达式所包含的面的数目 */
    bool is_inner_cell;        /* inner关键字 */
    bool simple;               /* 是否是只含有集合交运算的简单cell */
    double tmp;                /* tmp关键字 */
    double vol;                /* vol关键字 */

    int *surfs;                /* 当前cell的全部边界面编号，包含符号在内 */
    void **surfs_addr;         /* 对应于每个边界面的surface实例的地址 */
    char *rpn;                 /* 面布尔表达式转换而来的RPN表达式 */
    char *expr;                /* 原始面布尔表达式 */

    void *fill;                /* fill关键字，指向当前cell所包含的universe实例的地址 */
    void *parent;              /* 指向包含当前cell的上级universe结构 */
    void *mat;                 /* mat关键字，指向当前cell的material实例的地址 */
    void ***neighbor_lists;    /* 从每个面穿出的时候的邻居栅元列表 */
    int *neighbor_lists_sz;    /* 每个面的邻居栅元列表的长度 */
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

BEGIN_DECL
cell_t *
cell_init();

bool
particle_is_in_cell(const cell_t *obj,
                    const double pos[3],
                    const double dir[3]);

void
cell_free(cell_t *obj);

END_DECL

#endif //CRITI_CELL_H
