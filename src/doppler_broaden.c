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
    map_iterator *nuc_iter = map_get_iter(base_nucs);
    map_iterator *cell_iter = map_get_iter(base_cells);

    cnt = 0;
    while((nuc_entry = map_iter_next(nuc_iter))){
        nuc = nuc_entry->v.val;
        nuc->broaden_tmp = nuc->tmp;
        if(!ISNUMBER(*nuc->id))
            continue;
        first_time = true;
        broaden_tmp = ZERO;
        while((cell_entry = map_iter_next(cell_iter))){
            cell = cell_entry->v.val;
            mat = map_get(base_mats, cell->mat);
            if(!mat)
                continue;
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
        }
END:
        cnt += dppler_brdn_nuc_tmp(&base_acedata, nuc, broaden_tmp);
    }

    map_release_iter(nuc_iter);
    map_release_iter(cell_iter);

    fputs("===================== Cross-section doppler broaden ====================\n", base_IOfp.mat_fp);
    fprintf(base_IOfp.mat_fp, "Doppler broaden applied to %d nuclide.\n", cnt);
    fputs("========================================================================\n", base_IOfp.mat_fp);
}