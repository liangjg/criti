//
// Created by xaq on 9/2/17.
//

#ifndef CRITI_MATERIAL_H
#define CRITI_MATERIAL_H

#include "common.h"


typedef struct {
    int id;
    int tot_nuc_num;        /* 不包括热化核素在内的，该材料的所有核素数目 */
    int tot_sab_nuc_num;

    double user_den;
    double atom_den;
    double gram_den;

    void **nucs;            /* 该material所包含的全部nuclide实例的地址 */
    void *sab_nuc;          /* 该material所含有的sab_nuclide实例的地址 */
    char sab_nuc_id[12];

    double sab_nuc_esa;
    double *nuc_user_den;
    double *nuc_gram_den;
    double *nuc_atom_den;
} mat_t;

BEGIN_DECL
mat_t *
mat_init();

void
mat_free(mat_t *obj);

void
convert_mat_nuc_den();

END_DECL

#endif //CRITI_MATERIAL_H
