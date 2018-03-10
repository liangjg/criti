//
// Created by x1314aq on 18-1-16.
//

#include "fixed_source.h"
#include "RNG.h"
#include "IO_releated.h"


extern fixed_src_t base_fixed_src;

void
calc_fixed_src()
{
    init_external_src();

    particle_status_t par_status;
    int tot_neu_num = base_fixed_src.tot_neu_num;

    for(int neu = 1; neu <= tot_neu_num; neu++) {
        if(neu % 1000 == 0)
            printf("neutron: %d\n", neu);

        get_rand_seed();

        sample_fixed_src(&par_status);

        while(1) {
            track_history_fixed(&par_status);

            if(base_fixed_src.fixed_bank_cnt)
                sample_fission_src_fixed(&par_status);
            else
                break;
        }
    }
    output_summary_fixed();
}