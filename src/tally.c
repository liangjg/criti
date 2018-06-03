//
// Created by x1314aq on 18-5-28.
//

#include "tally.h"


void
tally_free(tally_t *tally)
{
    int i;

    if(HAS_CELL_TALLY(tally))
        for(i = 0; i < tally->cell_tally_sz; i++)
            cell_tally_free(tally->cell_tallies[i]);
    free(tally->cell_tallies);

    if(HAS_MESH_TALLY(tally))
        for(i = 0; i < tally->mesh_tally_sz; i++)
            mesh_tally_free(tally->mesh_tallies[i]);
    free(tally->mesh_tallies);

//    if(HAS_XS_TALLY(tally))
//        for(i = 0; i < tally->xs_tally_sz; i++)
//            xs_tally_free(tally->xs_tallies[i]);
//    free(tally->xs_tallies);
}