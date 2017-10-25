//
// Created by xaq on 9/8/17.
//

#ifndef TTYW_CELL_H
#define TTYW_CELL_H

#include "common.h"
#include "map.h"

typedef struct {
    int fill;             /* fill 关键字 */
    int mat;              /* mat 关键字 */
    int imp;              /* imp 关键字 */
    double tmp;           /* tmp 关键字 */
    double vol;           /* vol 关键字 */
    bool is_inner_cell;   /* inner 关键字 */
    char *rpn;            /* 面布尔表达式转换而来的RPN表达式 */
} cell_t;

#define CELL_KW_NUMBER        6
#define CELL_MAX_KW_LENGTH    6

static const char cell_kw[CELL_KW_NUMBER][CELL_MAX_KW_LENGTH] = {
        "FILL",
        "MAT",
        "IMP",
        "TMP",
        "VOL",
        "INNER"
};
#ifdef __cplusplus
extern "C" {
#endif

cell_t *cell_init();
bool particle_is_in_cell(const cell_t *obj, const double pos[3], const double dir[3], map *surfs);

#ifdef __cplusplus
}
#endif

#endif //TTYW_CELL_H
