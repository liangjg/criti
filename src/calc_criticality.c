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
extern int numbers_per_slave[64];
extern unsigned int offset_get_per_slave[64];
extern unsigned int offset_put_per_slave[64];

/* 从核写回的计算结果 */
extern double keff_wgt_sum[64][3];
extern int col_cnt[64];

void calc_criticality(){
    /* 初始化裂变源 */
    init_fission_source();

    athread_init();
    for(int cyc = 1; cyc <= base_criti.tot_cycle_num; cyc++){
        base_criti.current_cycle = cyc;

        /* 初始偏移均为0 */
        for(int i = 0; i < 64; i++){
            offset_put_per_slave[i] = 0;
            offset_get_per_slave[i] = 0;
        }

        for(int i = 0; i < base_criti.tot_transfer_num; i++){
            /* 准备从核需要的数据 */
            if(i == base_criti.tot_transfer_num - 1){    /* 如果已经是最后一次传输数据，那么把剩下的全部全部传过去 */
                for(int j = 0; j < 64; j++)
                    numbers_per_slave[j] = base_criti.fission_src_cnt[j] - 400 * i;
            }
            else{    /* 如果不是最后一次传输数据，说明后续还有数据，那么这次最多传输400个 */
                for(int j = 0; j < 64; j++)
                    numbers_per_slave[j] = 400;
            }

            /* 执行计算 */
            athread_spawn(do_calc, NULL);
            athread_join();

            /* 处理从核回传的计算结果 */
            for(int m = 0; m < 64; m++){
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