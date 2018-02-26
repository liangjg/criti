//
// Created by x1314aq on 17-11-26.
//

#include "common.h"
#include "map.h"
#include "criticality.h"
#include "IO_releated.h"
#include "nuclide.h"


extern map *base_univs;
extern map *base_cells;
extern map *base_surfs;
extern map *base_mats;
extern map *base_nucs;
extern criti_t base_criti;
extern IOfp_t base_IOfp;
extern nuc_cs_t *base_nuc_cs[NUMBERS_SLAVES];

void release_resource(){
    map_free(base_univs);
    free(base_univs);
    map_free(base_cells);
    free(base_cells);
    map_free(base_surfs);
    free(base_surfs);
    map_free(base_mats);
    free(base_mats);
    map_free(base_nucs);
    free(base_nucs);

    for(int i = 0; i < NUMBERS_SLAVES; i++){
        free(base_criti.fission_src[i]);
        free(base_criti.fission_bank[i]);
        free(base_nuc_cs[i]);
    }

    /* close all FILE structure if opened */
    if(base_IOfp.opt_fp) fclose(base_IOfp.opt_fp);
    if(base_IOfp.inp_fp) fclose(base_IOfp.inp_fp);
    if(base_IOfp.tally_fp) fclose(base_IOfp.tally_fp);
    if(base_IOfp.mat_fp) fclose(base_IOfp.mat_fp);
}