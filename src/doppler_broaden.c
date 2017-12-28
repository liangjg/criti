//
// Created by x1314aq on 17-12-13.
//

#include "acedata.h"
#include "map.h"
#include "IO_releated.h"
#include "cell.h"
#include "material.h"


extern map *base_cells;
extern map *base_nucs;
extern map *base_mats;
extern IOfp_t base_IOfp;
extern acedata_t base_acedata;

/***************************************************************************************************
 * 对所有nuclide进行多普勒展宽预处理
 * 如果所有cell都是相同的温度，那么一次将所有nuclide都展宽到该温度；
 * 如果某些cell有不同的温度，那么先将所有nuclide都展宽到0K温度，然后在输运过程中进一步根据cell->tmp进行进一步展宽
 ***************************************************************************************************/
void doppler_broaden(){
    mat_t *mat;
    nuclide_t *nuc;
    cell_t *cell;
    map_entry *cell_entry, *nuc_entry;
    int cnt;
    double broaden_tmp;
    bool first_time;    /* 是否所有cell都是相同的温度 */

    cnt = 0;
    for(unsigned long i = 0; i < base_nucs->ht.size; i++){
        nuc_entry = base_nucs->ht.buckets[i];
        while(nuc_entry){
            nuc = (nuclide_t *) nuc_entry->v.val;
            nuc->broaden_tmp = nuc->tmp;
            if(!ISNUMBER(*nuc->id)) {
                nuc_entry = nuc_entry->next;
                continue;
            }
            first_time = true;
            broaden_tmp = ZERO;
            for(unsigned long j = 0; j < base_cells->ht.size; j++){
                cell_entry = base_cells->ht.buckets[j];
                while(cell_entry){
                    cell = (cell_t *) cell_entry->v.val;
                    mat = (mat_t *) map_get(base_mats, cell->mat);
                    if(!mat){
                        cell_entry = cell_entry->next;
                        continue;
                    }
                    for(int k = 0; k < mat->tot_nuc_num; k++){
                        if(strcmp(nuc->id, mat->nuc_id[k]) == 0){
                            if(first_time){
                                broaden_tmp = cell->tmp;
                                first_time = false;
                            } else if(!EQ_ZERO(cell->tmp - broaden_tmp)){
                                broaden_tmp = 0;
                                goto END;
                            }
                        }
                    }
                    cell_entry = cell_entry->next;
                }
            }
END:
            cnt += dppler_brdn_nuc_tmp(&base_acedata, nuc, broaden_tmp);
            nuc_entry = nuc_entry->next;
        }
    }

    fputs("===================== Cross-section doppler broaden ====================\n", base_IOfp.mat_fp);
    fprintf(base_IOfp.mat_fp, "Doppler broaden applied to %d nuclide.\n", cnt);
    fputs("========================================================================\n", base_IOfp.mat_fp);
}