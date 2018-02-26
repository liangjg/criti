//
// Created by xaq on 10/26/17.
//

#include "criticality.h"
#include "RNG.h"
#include "IO_releated.h"
#include <athread.h>


extern SLAVE_FUN(do_calc)();

extern criti_t base_criti;

/* 主核需要准备的数据 */
int numbers_per_slave[NUMBERS_SLAVES];
unsigned int offset_get_per_slave[NUMBERS_SLAVES];
unsigned int offset_put_per_slave[NUMBERS_SLAVES];

/* 从核写回的计算结果 */
double keff_wgt_sum[NUMBERS_SLAVES][3];
RNG_t RNGs[NUMBERS_SLAVES];
int col_cnt[NUMBERS_SLAVES];

void calc_criticality(){
    /* 初始化裂变源 */
    init_fission_source();

    athread_init();
    for(int cyc = 1; cyc <= base_criti.tot_cycle_num; cyc++){
        base_criti.current_cycle = cyc;

        /* 初始偏移均为0 */
        for(int i = 0; i < NUMBERS_SLAVES; i++){
            offset_put_per_slave[i] = 0;
            offset_get_per_slave[i] = 0;
        }

        for(int i = 0; i < base_criti.tot_transfer_num; i++){
            /* 准备从核需要的数据 */
            if(i + 1 == base_criti.tot_transfer_num){    /* 如果已经是最后一次传输数据，那么把剩下的全部传过去 */
                for(int j = 0; j < NUMBERS_SLAVES; j++)
                    numbers_per_slave[j] = base_criti.fission_src_cnt[j] - 400 * i;
            }
            else{    /* 如果不是最后一次传输数据，说明后续还有数据，那么这次最多传输400个 */
                for(int j = 0; j < NUMBERS_SLAVES; j++)
                    numbers_per_slave[j] = 400;
            }

            /* 执行计算 */
            athread_spawn(do_calc, NULL);
            athread_join();

            /* 处理从核回传的计算结果 */
            for(int m = 0; m < NUMBERS_SLAVES; m++){
                for(int n = 0; n < 3; n++)
                    base_criti.keff_wgt_sum[n] += keff_wgt_sum[m][n];

                base_criti.tot_col_cnt += col_cnt[m];
                base_criti.tot_fission_bank_cnt += base_criti.fission_bank_cnt[m];
                offset_get_per_slave[m] += numbers_per_slave[m];
                offset_put_per_slave[m] += base_criti.fission_bank_cnt[m];
            }
        }

        /* 处理这一代计算完之后的结果 */
        process_cycle_end();
    }
    output_summary();

    athread_halt();
}