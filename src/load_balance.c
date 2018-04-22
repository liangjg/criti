//
// Created by x1314aq on 2018/4/22.
//

#ifdef USE_MPI

#include "parallel.h"
#include "criticality.h"


extern criti_t base_criti;
extern parallel_t base_parallel;
extern int base_num_threads;

static void
_sr_id_search(int *send_id,
              int *recv_id);

void
load_balance(void *args)
{
    pth_arg_t *pth_args = (pth_arg_t *) args;
    int tot_bank_cnt;
    int quotient, remainder;
    int id, my_id, i;
    bank_t *start_addr;

    my_id = base_parallel.id;
    tot_bank_cnt = base_parallel.tot_bank_cnt;

    /* 将pth_args[i].bank中的数据全部拷贝到base_parallel.bank中，用于后续进程间发送和接收数据 */
    start_addr = base_parallel.bank;
    for(i = 0; i < base_num_threads; i++) {
        memcpy(start_addr, pth_args[i].bank, sizeof(bank_t) * pth_args[i].bank_cnt);
        start_addr += pth_args[i].bank_cnt;
    }

    MPI_Allgather(&base_criti.tot_bank_cnt, 1, MPI_INT, base_parallel.bank_load, 1, MPI_INT, MPI_COMM_WORLD);
    base_parallel.bank_load_sum[0] = 0;
    for(id = 1; id < base_parallel.tot_procs; id++)
        base_parallel.bank_load_sum[id] = base_parallel.bank_load_sum[id - 1] + base_parallel.bank_load[id - 1];
    base_parallel.bank_load_sum[base_parallel.tot_procs] = tot_bank_cnt;

    quotient = tot_bank_cnt / base_parallel.tot_procs;
    remainder = tot_bank_cnt - base_parallel.tot_procs * quotient;
    for(id = 0; id < base_parallel.tot_procs; id++)
        base_parallel.src_load[id] = quotient;
    for(id = 0; id < remainder; id++)
        base_parallel.src_load[id]++;

    base_criti.tot_bank_cnt = base_parallel.src_load[my_id];

    base_parallel.src_load_sum[0] = 0;
    base_parallel.rand_num_pos[0] = base_parallel.rand_num_sum;
    for(id = 1; id < base_parallel.tot_procs; id++) {
        base_parallel.src_load_sum[id] = base_parallel.src_load_sum[id - 1] + base_parallel.src_load[id - 1];
        base_parallel.rand_num_pos[id] = base_parallel.rand_num_pos[id - 1] + base_parallel.src_load[id - 1];
    }
    base_parallel.rand_num_sum += tot_bank_cnt;
    base_parallel.src_load_sum[base_parallel.tot_procs] = tot_bank_cnt;

    int send_id, recv_id;
    _sr_id_search(&send_id, &recv_id);

    int send_cnt, send_len, send_times;
    int recv_cnt, recv_len, recv_times;
    MPI_Request *send_reqs, *recv_reqs;

    send_reqs = calloc((size_t) base_parallel.tot_procs, sizeof(MPI_Request));
    recv_reqs = calloc((size_t) base_parallel.tot_procs, sizeof(MPI_Request));

    send_cnt = 0;
    send_times = 0;
    while(MAX_ITER > send_times) {
        if(base_parallel.src_load_sum[send_id + 1] >= base_parallel.bank_load_sum[my_id + 1]) {
            send_len = base_parallel.bank_load[my_id] - send_cnt;
            if(send_len > 0) {
                send_reqs[send_times++] = 0;
                MPI_Isend(&base_parallel.bank[send_cnt], send_len, base_parallel.bank_type, send_id, 0, MPI_COMM_WORLD,
                          &send_reqs[send_times - 1]);
            }
            break;
        }

        send_len = base_parallel.src_load_sum[send_id + 1] - base_parallel.bank_load_sum[my_id] - send_cnt;
        if(send_len > 0) {
            send_reqs[send_times++] = 0;
            MPI_Isend(&base_parallel.bank[send_cnt], send_len, base_parallel.bank_type, send_id, 0, MPI_COMM_WORLD,
                      &send_reqs[send_times - 1]);
        }

        send_id++;
        send_cnt += send_len;
    }

    recv_cnt = 0;
    recv_times = 0;
    while(MAX_ITER > recv_times) {
        if(base_parallel.bank_load_sum[recv_id + 1] >= base_parallel.src_load_sum[my_id + 1]) {
            recv_len = base_parallel.src_load[my_id] - recv_cnt;
            if(recv_len > 0) {
                recv_reqs[recv_times++] = 0;
                MPI_Irecv(&base_parallel.src[recv_cnt], recv_len, base_parallel.bank_type, recv_id, 0, MPI_COMM_WORLD,
                          &recv_reqs[recv_times - 1]);
            }
            break;
        }

        recv_len = base_parallel.bank_load_sum[recv_id + 1] - base_parallel.src_load_sum[my_id] - recv_cnt;
        if(recv_len > 0) {
            recv_reqs[recv_times++] = 0;
            MPI_Irecv(&base_parallel.src[recv_cnt], recv_len, base_parallel.bank_type, recv_id, 0, MPI_COMM_WORLD,
                      &recv_reqs[recv_times - 1]);
        }

        recv_id++;
        recv_cnt += recv_len;
    }

    if(send_times > 0)
        MPI_Waitall(send_times, send_reqs, MPI_STATUSES_IGNORE);
    MPI_Waitall(recv_times, recv_reqs, MPI_STATUSES_IGNORE);

    int send_test, recv_test;
    if(send_times > 0)
        MPI_Testall(send_times, send_reqs, &send_test, MPI_STATUSES_IGNORE);
    MPI_Testall(recv_times, recv_reqs, &recv_test, MPI_STATUSES_IGNORE);

    if(send_test == 0 || recv_test == 0)
        printf("Error in sending fission banks. Thread:%d, Send:%d, Recv:%d\n", my_id, send_test, recv_test);

    free(send_reqs);
    free(recv_reqs);
}

void
_sr_id_search(int *send_id,
              int *recv_id)
{
    int min, max, mid, val;
    min = 0;
    max = base_parallel.tot_procs;
    val = base_parallel.bank_load_sum[base_parallel.id];
    if(val <= base_parallel.src_load_sum[min])
        *send_id = min;
    else if(val >= base_parallel.src_load_sum[max])
        *send_id = max - 1;
    else {
        while(max - min > 1) {
            mid = (min + max) / 2;
            if(val >= base_parallel.src_load_sum[mid])
                min = mid;
            else max = mid;
        }
        *send_id = min;
    }

    min = 0;
    max = base_parallel.tot_procs;
    val = base_parallel.src_load_sum[base_parallel.id];
    if(val <= base_parallel.bank_load_sum[min])
        *recv_id = min;
    else if(val >= base_parallel.bank_load_sum[max])
        *recv_id = max - 1;
    else {
        while(max - min > 1) {
            mid = (min + max) / 2;
            if(val >= base_parallel.bank_load_sum[mid])
                min = mid;
            else max = mid;
        }
        *recv_id = min;
    }
}

#endif