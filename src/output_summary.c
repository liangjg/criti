//
// Created by xaq on 11/15/17.
//

#include "IO_releated.h"
#include "criticality.h"


#ifdef USE_MPI
#include "parallel.h"

extern parallel_t base_parallel;
#endif

extern IOfp_t base_IOfp;
extern criti_t base_criti;

void
output_summary()
{
    int tot_col_cnt = base_criti.tot_col_cnt;
    unsigned tot_warnings = base_warnings;

#ifdef USE_MPI
    MPI_Reduce(&base_criti.tot_col_cnt, &tot_col_cnt, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&base_warnings, &tot_warnings, 1, MPI_UNSIGNED, MPI_SUM, 0, MPI_COMM_WORLD);
    if(!IS_MASTER) return;
#endif

    if(tot_warnings)
        printf("\n%d warning found.\n", tot_warnings);

    printf("\nAverage collisions per particle: %.2f \n",
           tot_col_cnt / (base_criti.tot_start_wgt * base_criti.tot_cycle_num));
    printf("\nFinal Keff: %f      Standard Deviation: %f\n", base_criti.keff_covw_ave[3], base_criti.keff_covw_std[3]);

    fprintf(base_IOfp.opt_fp, "\n%d warning found.\n", tot_warnings);
    fprintf(base_IOfp.opt_fp, "\nAverage collisions per particle: %.2f \n",
            tot_col_cnt / (base_criti.tot_start_wgt * base_criti.tot_cycle_num));
    fprintf(base_IOfp.opt_fp, "\nFinal Keff: %f      Standard Deviation: %f\n", base_criti.keff_covw_ave[3],
            base_criti.keff_covw_std[3]);
}