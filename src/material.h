//
// Created by xaq on 9/2/17.
//

#ifndef TTYW_MATERIAL_H
#define TTYW_MATERIAL_H

#include "common.h"

typedef struct {
    double user_den;
    double atom_den;
    double gram_den;

    int tot_nuc_num;        /* 不包括热化核素在内的，该材料的所有核素数目 */
    int tot_sab_nuc_num;

    char **nuc_id;
    char sab_nuc_id[12];    /* 只考虑了只包含一个热化核素的情况 */

    double sab_nuc_esa;
    double *nuc_user_den;
    double *nuc_atom_den;
} mat_t;

#ifdef __cplusplus
extern "C" {
#endif

mat_t *mat_init();
#define Get_tot_nuc_num(mat)         ((mat)->tot_nuc_num)
#define Get_tot_sab_nuc_num(mat)     ((mat)->tot_sab_nuc_num)
#define Get_nuc_id(mat, i)           (*((mat)->nuc_id + (i)))
#define Get_sab_nuc_id(mat, i)       (*((mat)->sab_nuc_id + (i)))
#define Get_nuc_user_den(mat, i)     (*((mat)->nuc_user_den + (i)))
#define Get_nuc_atom_den(mat, i)     (*((mat)->nuc_atom_den + (i)))

#ifdef __cplusplus
}
#endif

#endif //TTYW_MATERIAL_H
