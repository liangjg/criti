//
// Created by 叶鑫 on 2018/1/24.
//

#include "criticality.h"
#include "RNG.h"


extern RNG_t RNGs[64];
extern criti_t base_criti;

typedef unsigned long long ULL;

void prepare_RNG(){
    memcpy(&RNGs[0], &RNGs[63], sizeof(RNG_t));

    for(int i = 1; i < 64; i++){
        memcpy(&RNGs[i], &RNGs[i - 1], sizeof(RNG_t));

        int fis_src_cnt = base_criti.fission_src_cnt[i - 1];
        for(int j = 1; j < fis_src_cnt; j++)
            get_rand_seed(&RNGs[i]);
    }
}