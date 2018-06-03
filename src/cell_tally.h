//
// Created by x1314aq on 18-5-26.
//

#ifndef CRITI_CELL_TALLY_H
#define CRITI_CELL_TALLY_H

#include "particle_status.h"


/* 每个cell_tally_t对应输入文件中的一个cell_tally */
struct cell_tally_t{
    int id;                 // 每个tally的编号
    int erg_bin_sz;         // 能量分箱的数目
    int filter[8];          // filter关键字，长度不超过8，与particle_status中关于几何嵌套的假设相一致；
    int integral[8];        // integral关键字，长度不超过8；
    int vec_sz;             // 实际输入的cell_vector的长度；
    int cells_sz;           // 实际输入的cell_vector的数目（总共要对多少个cell进行tally）；
    int **cells;            // 用户输入的cell_vector，可能有多个，但是要求每一个cell_vector的长度都一样，且不超过8；
    double **result;        // 针对每个cell_vector的统计结果；第一维的大小是cells_sz，第二维的大小是(base.erg_bin_sz + 1)；
    double *erg_bin;        // 能量分箱

    void (*do_tally)(struct cell_tally_t *, particle_status_t *, double);
};

typedef struct cell_tally_t cell_tally_t;

BEGIN_DECL
/* type 1: 栅元通量 */
void ctally_flux(cell_tally_t *,
                particle_status_t *,
                double);

/* type 2: 栅元功率 */
void ctally_power(cell_tally_t *,
                  particle_status_t *,
                  double);

/* type 3: 栅元裂变反应率 */
void ctally_fis_rate(cell_tally_t *,
                     particle_status_t *,
                     double);

/* type 4: 栅元吸收反应率 */
void ctally_abs_rate(cell_tally_t *,
                     particle_status_t *,
                     double);

void cell_tally_free(cell_tally_t *);
END_DECL
#endif //CRITI_CELL_TALLY_H
