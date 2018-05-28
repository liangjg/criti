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
}