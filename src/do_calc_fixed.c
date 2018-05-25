//
// Created by 叶鑫 on 2018/3/27.
//

#include "RNG.h"
#include "fixed_source.h"
#include "neutron_transport.h"
#include "geometry.h"
#include "slave.h"


#define SZ_SRC    600
#define SZ_BANK   300

__thread_local volatile unsigned int get_reply, put_reply;
__thread_local int my_id;

/* 每个从核要模拟的粒子总数 */
__thread_local int tot_neu;

/* 每个从核的随机数发生器RNG */
__thread_local RNG_t RNG;

/* 每个从核在输运过程中存储核素截面的结构 */
__thread_local nuc_xs_t *nuc_xs;

/* ***********************************************************************
 * 每个从核LDM中都保存了900个bank_t对象，其中600个用于存储这一代需要
 * 模拟的source，300个用于存储产生的fission source；总大小约为:
 * sizeof(bank_t) * 900 = 64bytes * 900 = 57.6kB
 * ***********************************************************************/
__thread_local bank_t src[SZ_SRC], bank[SZ_BANK];

/* cur_bank_cnt为当前产生的裂变粒子数目，bank_cnt为总数 */
__thread_local int bank_cnt, cur_bank_cnt;

/* 每个从核都需要一个存储在LDM中的particle_state对象，用于进行粒子输运 */
__thread_local particle_status_t par_status;

/* 每个从核在本代模拟的粒子发生的总碰撞次数 */
__thread_local int col_cnt, cur_col_cnt;

extern double base_start_wgt;
extern int base_num_threads;

void
_do_calc_fixed(int neu);

void
do_calc_fixed(void *args)
{
    int i, neu, time;
    pth_arg_t *pth_arg;
    int quotient, remainder;

    pth_arg = (pth_arg_t *) args;

    my_id = athread_get_id(-1);

    get_reply = 0;
    put_reply = 0;

    /* 要模拟的粒子总数 */
    athread_get(PE_MODE, &pth_arg->src_cnt, &tot_neu, sizeof(int), &get_reply, 0, 0, 0);
    while(get_reply != 1);

    quotient = tot_neu / SZ_SRC;
    remainder = tot_neu - quotient * SZ_SRC;

    /* 从主核处取得随机数发生器RNG */
    athread_get(PE_MODE, &pth_arg->RNG, &RNG, sizeof(RNG_t), &get_reply, 0, 0, 0);
    while(get_reply != 2);

    /* 从主核处取得nuc_cs地址 */
    athread_get(PE_MODE, &pth_arg->nuc_xs, &nuc_xs, sizeof(nuc_xs_t *), &get_reply, 0, 0, 0);
    while(get_reply != 3);

    /* 初始化部分变量 */
    bank_cnt = 0;
    col_cnt = 0;

    for(time = 0; time < quotient; time++) {
        athread_get(PE_MODE, &pth_arg->src[time * SZ_SRC], src, SZ_SRC * sizeof(bank_t), &get_reply, 0, 0, 0);
        while(get_reply != 4 + time);

        for(neu = 0; neu < SZ_SRC; neu++) {
            _do_calc_fixed(neu);
        }
    }

    if(remainder) {
        athread_get(PE_MODE, &pth_arg->src[quotient * SZ_SRC], src, remainder * sizeof(bank_t), &get_reply, 0, 0, 0);
        while(get_reply != 4 + quotient);

        for(neu = 0; neu < remainder; neu++) {
            _do_calc_fixed(neu);
        }
    }

    athread_put(PE_MODE, &col_cnt, &pth_arg->col_cnt, sizeof(int), &put_reply, 0, 0);
    while(put_reply != 1);
}

void
_do_calc_fixed(int neu)
{
    int i;
    cell_t *cell;
    bank_t *cur_src;

    if(neu % 100 == 0 && my_id == 0)
        printf("neutron: %d\n", neu * base_num_threads);

    get_rand_seed(&RNG);

    /* 抽样要输运的粒子 */
    memset(&par_status, 0x0, sizeof(particle_status_t));

    cur_src = &src[neu];

    for(i = 0; i < 3; i++) {
        par_status.pos[i] = cur_src->pos[i];
        par_status.dir[i] = cur_src->dir[i];
    }

    par_status.erg = cur_src->erg;
    par_status.wgt = base_start_wgt;
    par_status.cell = locate_particle(&par_status, NULL, par_status.pos, par_status.dir);

    if(!par_status.cell)
        return;

    cell = par_status.cell;
    par_status.mat = cell->mat;
    par_status.cell_tmp = cell->tmp;

    while(1) {
        cur_col_cnt = 0;
        do {
            geometry_tracking_fixed(&par_status, nuc_xs, &RNG);
            if(par_status.is_killed) break;

            sample_col_nuclide(&par_status, nuc_xs, &RNG);
            if(par_status.is_killed) break;

            calc_col_nuc_cs(&par_status, &RNG);

            /* implicit capture(excluding fission) */
            treat_implicit_capture_fixed(&par_status, &RNG);
            if(par_status.is_killed) break;

            par_status.collision_type = sample_col_type_fixed(&par_status, &RNG);
            if(par_status.is_killed) break;

            get_exit_state_fixed(&par_status, &RNG, bank, &bank_cnt);
            if(par_status.is_killed) break;

            /* update particle state */
            par_status.erg = par_status.exit_erg;
            for(i = 0; i < 3; i++)
                par_status.dir[i] = par_status.exit_dir[i];
            double length = ONE / sqrt(SQUARE(par_status.dir[0]) + SQUARE(par_status.dir[1]) + SQUARE(par_status.dir[2]));
            par_status.dir[0] *= length;
            par_status.dir[1] *= length;
            par_status.dir[2] *= length;
        } while(++cur_col_cnt < MAX_ITER);
        col_cnt += cur_col_cnt;

        if(bank_cnt) {
            memset(&par_status, 0x0, sizeof(par_status));

            cur_src = &bank[--bank_cnt];
            for(i = 0; i < 3; i++) {
                par_status.pos[i] = cur_src->pos[i];
                par_status.dir[i] = cur_src->dir[i];
            }

            par_status.erg = cur_src->erg;
            par_status.wgt = cur_src->wgt;
            par_status.cell = locate_particle(&par_status, NULL, par_status.pos, par_status.dir);

            if(!par_status.cell)
                continue;

            cell = par_status.cell;
            par_status.mat = cell->mat;
            par_status.cell_tmp = cell->tmp;
        } else break;
    }
}