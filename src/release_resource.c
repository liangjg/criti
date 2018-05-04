//
// Created by x1314aq on 17-11-26.
//

#include "common.h"
#include "map.h"
#include "IO_releated.h"
#include "nuclide.h"

#ifdef USE_MPI
#include "parallel.h"

parallel_t base_parallel;
#endif

extern map *base_univs;
extern map *base_cells;
extern map *base_surfs;
extern map *base_mats;
extern map *base_nucs;
extern IOfp_t base_IOfp;
extern nuc_xs_t *base_nuc_xs;

void
release_resource()
{
    int i;

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

    free(base_nuc_xs);

    /* close all FILE structure if opened */
    if(base_IOfp.opt_fp) fclose(base_IOfp.opt_fp);
    if(base_IOfp.inp_fp) fclose(base_IOfp.inp_fp);
    if(base_IOfp.tally_fp) fclose(base_IOfp.tally_fp);
    if(base_IOfp.mat_fp) fclose(base_IOfp.mat_fp);

#ifdef USE_MPI
    MPI_Finalize();
    free(base_parallel.bank_load);
    free(base_parallel.bank_load_sum);
    free(base_parallel.src_load);
    free(base_parallel.src_load_sum);
    free(base_parallel.rand_num_pos);
    free(base_parallel.bank);
    free(base_parallel.src);
#endif
}
