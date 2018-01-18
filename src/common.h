//
// Created by xaq on 9/8/17.
//

#ifndef CRITI_COMMON_H
#define CRITI_COMMON_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <float.h>
#include <time.h>
#include <stdbool.h>


#ifdef USE_MPI
    #include <mpi.h>
#endif

#if defined(__linux__) || defined(__linux) || defined(linux)
    #define OS_LINUX
#elif defined(__WIN32__) || defined(_WIN32) || defined(WIN32)
#define OS_WIN32
#elif defined(__APPLE__)
    #define OS_MAC
#endif

#define MAX(_x, _y)     ((_x) < (_y) ? (_y) : (_x))
#define MIN(_x, _y)     ((_x) < (_y) ? (_x) : (_y))
#define SQUARE(_x)      ((_x) * (_x))
#define ISNUMBER(_c)    ((48 <= (_c)) && ((_c) <= 57))
#define ISUPPER(_c)     ((65 <= (_c)) && ((_c) <= 90))
#define ISLOWER(_c)     ((97 <= (_c)) && ((_c) <= 122))
#define ISALPHA(_c)     (ISLOWER(_c) || ISUPPER(_c))
#define ISRETURN(_c)    (((_c) == 10) || ((_c) == 13))
#define ISSPACE(_c)     ((_c) == 32)
#define ISCOMMENT(_c)   ((_c) == 47)
#define TOUPPER(_c)     (ISLOWER(_c) ? ((_c) - 32) : (_c))
#define TOLOWER(_c)     (ISUPPER(_c) ? ((_c) + 32) : (_c))


#define MAX_LINE_LENGTH      220
#define MAX_WORLD_LENGTH     20
#define MAX_FILENAME_LENGTH  50

#define KW_NUMBER       10
#define MAX_KW_LENGTH   15

#define CODE_VERSION  "Beta 2.5.7"

#define WGT_CUTOFF    0.25
#define EG0_CUTOFF    1.0E-20

#define PI            3.1415926536
#define AVG           6.022043446928244e+23
#define NEU_MASS      1.008664967
#define BARN          1.0E-24
#define ZERO_ERG      1.0E-24
#define MAX_ITER      100000
#define ZERO          0.0
#define HALF          0.5
#define ONE           1.0
#define TWO           2.0
#define EPSILON       1.0E-10

#define GT_ZERO(_x)     ((_x) > EPSILON)
#define LT_ZERO(_x)     ((_x) < -EPSILON)
#define EQ_ZERO(_x)     ((-EPSILON <= (_x)) && ((_x) <= EPSILON))

/* Global static read-only key word */
static const char keyword[KW_NUMBER][MAX_KW_LENGTH] = {
        "UNIVERSE",
        "SURFACE",
        "MATERIAL",
        "CRITICALITY",
        "TALLY",
        "FIXEDSOURCE",
        "DEPLETION",
        "BURNUP"
};

/* number of boolean expression operator */
#define N_OPTR  6

/* 交,并,补,左括号,右括号,表达式结束 */
typedef enum{INTER, UNION, COMPLEMENT, L_P, R_P, EOE} OPERATOR_T;

/* 大于、小于和等于号表示运算的优先级，空格表示该情况不存在 */
static const char priority[N_OPTR][N_OPTR] = {
        /*-----------|---------------CurOp-------------------|*/
        /*-----------&-------:-------!-------(-------)------\0*/
    {   /*-|--&-*/  '>',    '>',    '<',    '<',    '>',    '>'},
    {   /*-T--:-*/  '>',    '>',    '<',    '<',    '>',    '>'},
    {   /*-o--!-*/  '>',    '>',    '>',    '<',    '>',    '>'},
    {   /*-p--(-*/  '<',    '<',    '<',    '<',    '=',    ' '},
    {   /*-O--)-*/  ' ',    ' ',    ' ',    ' ',    ' ',    ' '},
    {   /*-P-\0-*/  '<',    '<',    '<',    '<',    ' ',    '='}
};

/* calculation mode */
typedef enum{
    CRITICALITY = 1,
    BURNUP,
    POINTBURN,
    FIXEDSOURCE
} CALC_MODE_T;

/* time elapsed */
extern time_t start_time;
extern time_t finish_time;

/* warnings found */
extern unsigned base_warnings;

#ifdef __cplusplus
extern "C" {
#endif

void release_resource();

#ifdef __cplusplus
}
#endif

#endif //CRITI_COMMON_H
