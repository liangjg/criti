//
// Created by xaq on 11/15/17.
//

#include "IO_releated.h"
#include "criticality.h"

extern IOfp_t base_IOfp;
extern criti_t base_criti;

void output_summary(){
    if(base_warnings)
        printf("\n%d warning found.\n", base_warnings);

    printf("\nAverage collisions per particle: %.2f \n",
           base_criti.tot_col_cnt / (base_criti.tot_start_wgt * base_criti.tot_cycle_num));
    printf("\nFinal Keff: %f      Standard Deviation: %f\n", base_criti.keff_covw_ave[3], base_criti.keff_covw_std[3]);

    fprintf(base_IOfp.opt_fp, "\n%d warning found.\n", base_warnings);
    fprintf(base_IOfp.opt_fp, "\nAverage collisions per particle: %.2f \n",
            base_criti.tot_col_cnt / (base_criti.tot_start_wgt * base_criti.tot_cycle_num));
    fprintf(base_IOfp.opt_fp, "\nFinal Keff: %f      Standard Deviation: %f\n", base_criti.keff_covw_ave[3],
            base_criti.keff_covw_std[3]);
}