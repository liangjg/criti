//
// Created by x1314aq on 17-12-12.
//

#include "material.h"
#include "map.h"
#include "nuclide.h"
#include "vector.h"


#define AVGDN    (1.0E-24 * 6.022043446928244e+23 / 1.008664967)
extern map *base_mats;
extern map *base_nucs;

void convert_mat_nuc_den(){
    map_entry *entry;
    mat_t *mat;
    nuclide_t *nuc;
    vector *nuc_gram_den;
    int tot_nuc_num;
    double sum_atom_den, sum_gram_den, atom_wgt;
    char *nuc_id;
    int table = base_mats->table;
    nuc_gram_den = vector_init(1, sizeof(double));

    for(unsigned long i = 0; i < base_mats->ht[table].size; i++){
        entry = base_mats->ht[table].buckets[i];
        while(entry){
            mat = (mat_t *) entry->v.val;
            tot_nuc_num = mat->tot_nuc_num;
            mat->nuc_atom_den = (double *) malloc(tot_nuc_num * sizeof(double));
            vector_resize(nuc_gram_den, tot_nuc_num);
            sum_atom_den = ZERO;
            sum_gram_den = ZERO;
            for(int j = 0; j < tot_nuc_num; j++){
                nuc_id = mat->nuc_id[j];
                nuc = (nuclide_t *)map_get(base_nucs, (uint64_t)nuc_id);
                atom_wgt = nuc->atom_wgt;
                if(LT_ZERO(mat->nuc_user_den[j])){    /* Convert mass fractions to atom fractions */
                    mat->nuc_atom_den[j] = -mat->nuc_user_den[j] / atom_wgt;
                    *(double *)vector_at(nuc_gram_den, j) = -mat->nuc_user_den[j];
                }
                else{
                    mat->nuc_atom_den[j] = mat->nuc_user_den[j];
                    *(double *)vector_at(nuc_gram_den, j) = mat->nuc_user_den[j] * atom_wgt;
                }
                sum_atom_den += mat->nuc_atom_den[j];
                sum_gram_den += *(double *)vector_at(nuc_gram_den, j);
            }
            /* Normalize the atom fraction and mass fraction */
            for(int j = 0; j < tot_nuc_num; j++){
                mat->nuc_atom_den[j] /= sum_atom_den;
                *(double *)vector_at(nuc_gram_den, j) /= sum_gram_den;
            }
            /* Density of material */
            if(LT_ZERO(mat->user_den)){
                mat->gram_den = -mat->user_den;
                mat->atom_den = -mat->user_den * (AVGDN * sum_atom_den / sum_gram_den);
            }
            else{
                mat->gram_den = mat->user_den / (AVGDN * sum_atom_den / sum_gram_den);
                mat->atom_den = mat->user_den;
            }
            /* Density of nuclides */
            for(int j = 0; j < tot_nuc_num; j++)
                mat->nuc_atom_den[j] *= mat->atom_den;

            entry = entry->next;
        }
    }

    vector_free(nuc_gram_den);
}