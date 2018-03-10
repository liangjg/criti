//
// Created by x1314aq on 18-1-17.
//

#include "IO_releated.h"
#include "fixed_source.h"


extern fixed_src_t base_fixed_src;
extern IOfp_t base_IOfp;

void
output_summary_fixed()
{
    if(base_warnings)
        printf("\n%d warning found.\n", base_warnings);

    double ave_col_cnt = base_fixed_src.tot_col_cnt / base_fixed_src.tot_start_wgt;
    printf("\nAverage collisions per particle: %.2f \n", ave_col_cnt);
    fprintf(base_IOfp.opt_fp, "\nAverage collisions per particle: %.2f \n", ave_col_cnt);
}