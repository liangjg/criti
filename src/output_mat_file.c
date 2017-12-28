//
// Created by x1314aq on 17-12-13.
//

#include "IO_releated.h"
#include "material.h"
#include "nuclide.h"
#include "map.h"


#define AVGDN    (BARN * AVG / NEU_MASS)
extern IOfp_t base_IOfp;
extern map *base_mats;

void output_mat_file(){
    mat_t *mat;
    map_entry *entry;
    int tot_nuc_num;

    fprintf(base_IOfp.mat_fp, "====================== Material Information Start ======================\n");
    fprintf(base_IOfp.mat_fp, "Material    Nuclide       Mass Density     Atom Density     User Input\n");
    fprintf(base_IOfp.mat_fp, "                           (g/cm^3)      (10^24atom/cm^3)             \n");

    for(unsigned long i = 0; i < base_mats->ht.size; i++){
        entry = base_mats->ht.buckets[i];
        while(entry){
            mat = (mat_t *) entry->v.val;
            tot_nuc_num = mat->tot_nuc_num;
            fprintf(base_IOfp.mat_fp, "%-4ld          Sum         %.5E      %.5E      %.5E\n", entry->key,
                    mat->gram_den, mat->atom_den, mat->user_den);
            for(int j = 0; j < tot_nuc_num; j++)
                fprintf(base_IOfp.mat_fp, "           %-10s     %.5E      %.5E      %.5E\n", mat->nuc_id[j],
                        mat->nuc_gram_den[j], mat->nuc_atom_den[j], mat->nuc_user_den[j]);
            entry = entry->next;
        }
    }
    fprintf(base_IOfp.mat_fp, "======================= Material Information End =======================\n\n");
}