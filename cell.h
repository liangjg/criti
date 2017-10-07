//
// Created by xaq on 9/8/17.
//

#ifndef TTYW_CELL_H
#define TTYW_CELL_H

#include "common.h"

typedef struct {
    int index;
    int fill;             /* fill 关键字 */
    int mat;              /* mat 关键字 */
    int imp;              /* imp 关键字 */
    double tmp;           /* tmp 关键字 */
    double vol;           /* vol 关键字 */
    bool is_inner_cell;   /* inner 关键字 */
    char *rpn;            /* 面布尔表达式转换而来的RPN表达式 */
} cell_t;
#endif //TTYW_CELL_H
