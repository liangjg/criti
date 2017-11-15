//
// Created by xaq on 10/26/17.
//

#include "criticality.h"
#include "RNG.h"
#include "IO_releated.h"

extern criti_t base_criti;

extern RNG_t base_RNG;

void calc_criticality(){
    init_fission_source();

    for(int cyc = 1; cyc <= base_criti.tot_cycle_num; cyc++){
        for(int neu = 1; neu <= base_criti.cycle_neutron_num; neu++){
            get_rand_seed();

            sample_fission_source();

            track_history();
        }
        process_cycle_end();
    }

    output_summary();
}