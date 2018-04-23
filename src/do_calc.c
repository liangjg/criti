//
// Created by xaq on 1/22/18
//

#include "RNG.h"
#include "criticality.h"
#include "neutron_transport.h"
#include "geometry.h"
#include "slave.h"


__thread_local volatile unsigned int get_reply, put_reply;
__thread_local int my_id;

/* 每个从核要模拟的粒子总数 */
__thread_local int tot_neu;

/* 每个从核的随机数发生器RNG */
__thread_local RNG_t RNG;

/* 每个从核在输运过程中存储核素截面的结构 */
__thread_local nuc_xs_t *nuc_xs;

/* ***********************************************************************
 * 每个从核LDM中都保存了900个bank_t对象，其中300个用于存储这一代需要
 * 模拟的source，600个用于存储产生的fission source；总大小约为:
 * sizeof(bank_t) * 900 = 64bytes * 900 = 57.6kB
 * ***********************************************************************/
__thread_local bank_t *src, *bank;

/* cur_bank_cnt为当前产生的裂变粒子数目，bank_cnt为总数 */
__thread_local int bank_cnt;

/* 每个从核都需要一个存储在LDM中的particle_state对象，用于进行粒子输运 */
__thread_local particle_status_t par_status;

/* 每个从核在本代模拟的粒子发生的总碰撞次数 */
__thread_local int col_cnt;

/* 上一代计算得到的k-effective */
__thread_local double keff_final;

/* 每个从核在本代模拟的粒子对k-eff估计器的贡献 */
__thread_local double keff_wgt_sum[3];

/* 主核上的变量 */
extern double base_start_wgt;
extern universe_t *root_universe;

void
_do_calc(int neu);

void
do_calc(void *args)
{
    int i, neu;
    pth_arg_t *pth_arg;
    cell_t *cell;

    pth_arg = (pth_arg_t *) args;

    /* 每个从核的id */
    my_id = athread_get_id(-1);

    get_reply = 0;
    put_reply = 0;

    /* 要模拟的粒子总数 */
    athread_get(PE_MODE, &pth_arg->src_cnt, &tot_neu, sizeof(int), &get_reply, 0, 0, 0);
    while(get_reply != 1);

    /* 上一代的keff */
    athread_get(PE_MODE, &pth_arg->keff_final, &keff_final, sizeof(double), &get_reply, 0, 0, 0);
    while(get_reply != 2);

    /* 随机数发生器RNG */
    athread_get(PE_MODE, &pth_arg->RNG, &RNG, sizeof(RNG_t), &get_reply, 0, 0, 0);
    while(get_reply != 3);

    /* nuc_xs地址 */
    athread_get(PE_MODE, &pth_arg->nuc_xs, &nuc_xs, sizeof(nuc_xs_t *), &get_reply, 0, 0, 0);
    while(get_reply != 4);

    /* 初始化部分变量 */
    src = pth_arg->src;
    bank = pth_arg->bank;
    bank_cnt = 0;
    col_cnt = 0;
    for(i = 0; i < 3; i++)
        keff_wgt_sum[i] = ZERO;

    for(neu = 0; neu < tot_neu; neu++){
        get_rand_seed_slave(&RNG);

        /* 抽样要输运的粒子 */
        memset(&par_status, 0x0, sizeof(particle_status_t));

        for(i = 0; i < 3; i++) {
            par_status.pos[i] = src[neu].pos[i];
            par_status.dir[i] = src[neu].dir[i];
        }

        par_status.erg = src[neu].erg;
        par_status.wgt = base_start_wgt;
        par_status.cell = locate_particle(&par_status, root_universe, par_status.pos, par_status.dir);

        if(!par_status.cell)
            continue;

        cell = par_status.cell;
        par_status.mat = cell->mat;
        par_status.cell_tmp = cell->tmp;

        do {
            /* geometry tracking: free flying */
            geometry_tracking(&par_status, keff_wgt_sum, nuc_xs, &RNG);
            if(par_status.is_killed) break;

            /* sample collision nuclide */
            sample_col_nuclide(&par_status, nuc_xs, &RNG);
            if(par_status.is_killed) break;

            /* calculate cross-section */
            calc_col_nuc_cs(&par_status, &RNG);

            /* treat fission */
            treat_fission(&par_status, &RNG, keff_wgt_sum, bank, &bank_cnt, keff_final);

            /* implicit capture(including fission) */
            treat_implicit_capture(&par_status, &RNG);
            if(par_status.is_killed) break;

            /* sample collision type */
            par_status.collision_type = sample_col_type(&par_status, &RNG);
            if(par_status.is_killed) break;

            /* sample exit state */
            get_exit_state(&par_status, &RNG);
            if(par_status.is_killed) break;

            /* update particle state */
            par_status.erg = par_status.exit_erg;
            for(i = 0; i < 3; i++)
                par_status.dir[i] = par_status.exit_dir[i];
            double length = ONE / sqrt(SQUARE(par_status.dir[0]) + SQUARE(par_status.dir[1]) + SQUARE(par_status.dir[2]));
            par_status.dir[0] *= length;
            par_status.dir[1] *= length;
            par_status.dir[2] *= length;
        } while(++col_cnt < MAX_ITER);
    }

    /* 写回计算结果 */
    athread_put(PE_MODE, keff_wgt_sum, pth_arg->keff_wgt_sum, 3 * sizeof(double), &put_reply, 0, 0);
    while(put_reply != 1);

    athread_put(PE_MODE, &bank_cnt, &pth_arg->bank_cnt, sizeof(int), &put_reply, 0, 0);
    while(put_reply != 2);

    athread_put(PE_MODE, &col_cnt, &pth_arg->col_cnt, sizeof(int), &put_reply, 0, 0);
    while(put_reply != 3);

    if(my_id == 63) {
        athread_put(PE_MODE, &RNG, &pth_arg->RNG, sizeof(RNG_t), &put_reply, 0, 0);
        while(put_reply != 4);
    }
}