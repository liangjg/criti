//
// Created by xaq on 9/2/17.
//

#include "global_fun.h"


int
get_intplt_pos(const double *vec,
               const double key,
               int low,
               int high)
{
    int lo, hi, mi;
    lo = low;
    /* RMC在使用这个函数的时候使用的是[low, high]这种左右都闭合的区间，为了满足算法的要求，需要把high加1，变成[low, high) */
    hi = high + 1;

    /* 如果key < vec[low]，则返回low */
    if(key < vec[low])
        return low;
    while(lo < hi) {
        mi = (lo + hi) >> 1;
        if(key < vec[mi])
            hi = mi;
        else
            lo = mi + 1;
        //        key < vec[mi] ? hi = mi : lo = mi + 1;
    }
    /* 查找失败的时候，统一返回不大于key的元素所在的最大位置 */
    return --lo;
}

int
get_intplt_pos_fr(const double *vec,
                  const double key,
                  int low,
                  int high,
                  double *frac)
{
    int lo, hi, mi;
    lo = low;
    hi = high + 1;

    /* 如果key < vec[low]，则返回low */
    if(key < vec[low]) {
        *frac = ZERO;
        return low;
    }
    /* 如果key >= vec[high]，则返回high - 1，这里与上面不同 */
    if(key >= vec[high]) {
        *frac = ONE;
        return high - 1;
    }

    while(lo < hi) {
        mi = (lo + hi) >> 1;
        if(key < vec[mi])
            hi = mi;
        else
            lo = mi + 1;
    }

    /* 查找完了之后，lo指向大于key的最小元素的位置 */
    if(vec[lo - 1] == vec[lo])
        *frac = ZERO;
    else
        *frac = (key - vec[lo - 1]) / (vec[lo] - vec[lo - 1]);

    return --lo;
}

double
intplt_by_pos_fr(const double *vec,
                 int pos,
                 double frac)
{
    return vec[pos] + (vec[pos + 1] - vec[pos]) * frac;
}