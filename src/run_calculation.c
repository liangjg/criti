//
// Created by xaq on 10/26/17.
//

#include "common.h"
#include "calculation.h"
#include "geometry.h"

void run_calculation(CALC_MODE_T mode){
    build_neighbor_list();

    switch(mode){
        case CRITICALITY:
            puts("\n******** Calculation mode: criticality ********\n");
            calc_criticality();
            break;
        case FIXEDSOURCE:
            puts("\n******** Calculation mode: fixed-source ********\n");
//            calc_fixed_source();
            break;
        case BURNUP:
            puts("\n******** Calculation mode: burnup ********\n");
//            calc_burnup();
            break;
        case POINTBURN:
            puts("\n******** Calculation mode: point burnup ********\n");
//            calc_point_burn();
            break;
        default:
            puts("\n******** Unknown calculation mode. ********\n");
            break;
    }
}