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

/* 每个从核在输运过程中存储核素截面的结构 */
__thread_local nuc_xs_t *nuc_xs;

/* 每个从核都需要一个存储在LDM中的particle_state对象，用于进行粒子输运 */
__thread_local particle_status_t par_status;

/* 每个从核的pth_arg_t结构，直接拷贝的参数 */
__thread_local pth_arg_t pth_arg;

/* 主核上的变量 */
extern double base_start_wgt;
extern universe_t *root_universe;
extern int base_tot_nucs;
extern nuc_xs_t *base_nuc_xs;

void
do_calc(void *args)
{
    int i, neu, tot_neu, MT;
    double length;
    cell_t *cell;

    /* 每个从核的id */
    my_id = athread_get_id(-1);

    /* 直接从主核所传的参数复制 */
    get_reply = 0;
    athread_get(PE_MODE, args, &pth_arg, sizeof(pth_arg_t), &get_reply, 0, 0, 0);
    while(get_reply != 1);

    nuc_xs = ldm_malloc(base_tot_nucs * sizeof(nuc_xs_t));

    /* 初始化部分变量 */
    tot_neu = pth_arg.src_cnt;
    pth_arg.bank_cnt = 0;
    pth_arg.col_cnt = 0;
    for(i = 0; i < 3; i++)
        pth_arg.keff_wgt_sum[i] = ZERO;
    for(i = 0; i < base_tot_nucs; i++)
        nuc_xs[i].ptable = base_nuc_xs[i].ptable;

    for(neu = 0; neu < tot_neu; neu++){
        get_rand_seed_slave(&pth_arg.RNG);

        /* 抽样要输运的粒子 */
        memset(&par_status, 0x0, sizeof(particle_status_t));

        for(i = 0; i < 3; i++) {
            par_status.pos[i] = pth_arg.src[neu].pos[i];
            par_status.dir[i] = pth_arg.src[neu].dir[i];
        }

        par_status.erg = pth_arg.src[neu].erg;
        par_status.wgt = base_start_wgt;
        par_status.cell = locate_particle(&par_status, root_universe, par_status.pos, par_status.dir);

        if(!par_status.cell)
            continue;

        cell = par_status.cell;
        par_status.mat = cell->mat;
        par_status.cell_tmp = cell->tmp;

        do {
            /* geometry tracking: free flying */
            geometry_tracking(&par_status, pth_arg.keff_wgt_sum, nuc_xs, &pth_arg.RNG);
            if(par_status.is_killed) break;

            /* sample collision nuclide */
            sample_col_nuclide(&par_status, nuc_xs, &pth_arg.RNG);
            if(par_status.is_killed) break;

            /* calculate cross-section */
            calc_col_nuc_cs(&par_status, &pth_arg.RNG);

            /* treat fission */
            treat_fission(&par_status, &pth_arg);

            /* implicit capture(including fission) */
            treat_implicit_capture(&par_status, &pth_arg.RNG);
            if(par_status.is_killed) break;

            /* sample collision type */
            MT = sample_col_type(&par_status, &pth_arg.RNG);
            if(par_status.is_killed) break;

            /* sample exit state */
            get_exit_state(&par_status, MT, &pth_arg.RNG);
            if(par_status.is_killed) break;

            /* update particle state */
            par_status.erg = par_status.exit_erg;
            for(i = 0; i < 3; i++)
                par_status.dir[i] = par_status.exit_dir[i];
            length = ONE / sqrt(SQUARE(par_status.dir[0]) + SQUARE(par_status.dir[1]) + SQUARE(par_status.dir[2]));
            par_status.dir[0] *= length;
            par_status.dir[1] *= length;
            par_status.dir[2] *= length;
        } while(++pth_arg.col_cnt < MAX_ITER);
    }

    ldm_free(nuc_xs, base_tot_nucs * sizeof(nuc_xs_t));

    /* 写回计算结果 */
    put_reply = 0;
    athread_put(PE_MODE, &pth_arg, args, sizeof(pth_arg_t), &put_reply, 0, 0);
    while(put_reply != 1);
}
