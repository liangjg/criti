//
// Created by x1314aq on 2018/4/22.
//

#ifndef CRITI_PARALLEL_H
#define CRITI_PARALLEL_H

#include "common.h"


typedef struct parallel_t {
    int id;
    int tot_procs;
    int rand_num_sum;
    int tot_bank_cnt;
    MPI_Datatype bank_type;

    int *bank_load;
    int *bank_load_sum;
    int *src_load;
    int *src_load_sum;
    int *rand_num_pos;

    bank_t *bank;
    bank_t *src;
} parallel_t;

BEGIN_DECL

#define IS_MASTER  (base_parallel.id == 0)

void
load_balance(void *args);

END_DECL
#endif //CRITI_PARALLEL_H
