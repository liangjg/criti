//
// Created by x1314aq on 18-6-3.
//

#ifndef CRITI_MESH_TALLY_H
#define CRITI_MESH_TALLY_H

#include "particle_status.h"


struct mesh_tally_t {
    int id;
    int erg_bin_sz;
    int scope[3];       /* 总网格数为abs(scope[0] * scope[1] * scope[2])，在读取过程中将-1转换为1存储*/
    double bound_min[3];
    double bound_max[3];
    double delta[3];
    double *erg_bin;
    double **result;    /* 第一维为总网格数，第二维为erg_bin_sz + 1 */

    void (*do_tally)(struct mesh_tally_t *, particle_status_t *, double);
};

typedef struct mesh_tally_t mesh_tally_t;

BEGIN_DECL
/* type 1: 栅元通量 */
void mtally_flux(mesh_tally_t *,
                particle_status_t *,
                double);

/* type 2: 栅元功率 */
void mtally_power(mesh_tally_t *,
                  particle_status_t *,
                  double);

/* type 3: 栅元裂变反应率 */
void mtally_fis_rate(mesh_tally_t *,
                     particle_status_t *,
                     double);

/* type 4: 栅元吸收反应率 */
void mtally_abs_rate(mesh_tally_t *,
                     particle_status_t *,
                     double);
END_DECL

void mesh_tally_free(mesh_tally_t *);

#endif //CRITI_MESH_TALLY_H
