//
// Created by x1314aq on 18-1-16.
//

#include "fixed_source.h"
#include "RNG.h"
#include "IO_releated.h"


extern fixed_src_t base_fixed_src;

void calc_fixed_source(){
    init_external_source();

    particle_state_t par_state;
    for(int neu = 1; neu <= base_fixed_src.tot_neu_num; neu++){
        if(neu % 1000 == 0)
            printf("neutron: %d\n", neu);

        get_rand_seed();

        sample_fixed_source(&par_state);

        while(1){
            track_history_fixed(&par_state);

            if(base_fixed_src.fixed_src_bank_cnt)
                sample_fission_source_fixed(&par_state);
            else
                break;
        }
    }
    output_summary_fixed();
}