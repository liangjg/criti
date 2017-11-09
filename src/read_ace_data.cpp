//
// Created by xaq on 11/9/17.
//

#include "IO_releated.h"
#include "acedata.h"
#include "map.h"
#include "material.h"
#include "nuclide.h"

extern IOfp_t base_IOfp;

extern acedata_t base_acedata;

extern map *base_mats;

extern map *base_nucs;

void read_ace_data(){
    map_entry *entry, *nuc_entry;
    mat_t *mat;
    nuclide_t *nuc;
    int tot_nuc_exc_sab;
    int tot_nuc_inc_sab;
    int used_table;
    int tot_mat;
    int used_size;

    tot_nuc_exc_sab = 0;
    tot_nuc_inc_sab = 0;
    used_table = base_mats->table;
    tot_mat = base_mats->ht[used_table].used;
    used_size = base_mats->ht[used_table].size;

    for(int i = 0; i < used_size; i++){
        entry = base_mats->ht[used_table].buckets[i];
        while(entry){
            mat = (mat_t *) (entry->v.val);
            for(int j = 0; j < mat->tot_nuc_num; j++){
                nuc_entry = map_find(base_nucs, (uint64_t) mat->nuc_id[j]);
                if(!nuc_entry){
                    nuc = new nuclide_t;
                    memset(nuc, 0x0, sizeof(nuclide_t));
                    strcpy(nuc->id, mat->nuc_id[j]);
                    map_put(base_nucs, (uint64_t) mat->nuc_id[j], nuc);
                    tot_nuc_exc_sab++;
                }
            }

            for(int j = 0; j < mat->tot_sab_nuc_num; j++){
                nuc = new nuclide_t;
                memset(nuc, 0x0, sizeof(nuclide_t));
                strcpy(nuc->id, mat->sab_nuc_id);
                map_put(base_nucs, (uint64_t) mat->sab_nuc_id, nuc);
                tot_nuc_inc_sab++;
            }
            entry = entry->next;
        }
    }

    tot_nuc_inc_sab += tot_nuc_exc_sab;
}