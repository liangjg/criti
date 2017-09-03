#ifndef COMC_COMMON_H
#define COMC_COMMON_H

#ifdef __cplusplus
  #include <cstdlib>
  #include <cstdio>
  #include <cmath>
  #include <cstring>
  #include <cfloat>
  #include <ctime>
  #include <cstdbool>
#else
  #include <stdlib.h>
  #include <stdio.h>
  #include <math.h>
  #include <string.h>
  #include <float.h>
  #include <time.h>
  #include <stdbool.h>
#endif

#ifdef USE_MPI
#include <mpi.h>
#endif


#define MAX(_x, _y)     ((_x) < (_y) ? (_y) : (_x))
#define MIN(_x, _y)     ((_x) < (_y) ? (_x) : (_y))
#define SQUARE(_x)     ((_x) * (_x))
#define ISNUMBER(_c)   ((48 <= (_c)) && ((_c) <= 57))
#define ISUPPER(_c)    ((65 <= (_c)) && ((_c) <= 90))
#define ISLOWER(_c)    ((97 <= (_c)) && ((_c) <= 122))
#define ISALPHA(_c)    (ISLOWER(_c) || ISUPPER(_c))
#define ISRETURN(_c)   (((_c) == 10) || ((_c) == 13))
#define ISSPACE(_c)    ((_c) == 32)
#define ISCOMMENT(_c)  ((_c) == 47)
#define TOUPPER(_c)    (ISLOWER(_c) ? ((_c) - 32) : (_c))
#define TOLOWER(_c)    (ISUPPER(_c) ? ((_c) + 32) : (_c))


#define MAX_LINE_LENGTH      220
#define MAX_WORLD_LENGTH     20
#define MAX_FILENAME_LENGTH  50

#define KW_NUMBER       10
#define MAX_KW_LENGTH   15

#define CODE_VERSION  "Beta 2.5.7"

#define WGT_CUTOFF    0.25
#define EG0_CUTOFF    1.0E-20

#define PI            3.1415926536
#define ZERO_ERG      1.0E-25
#define MAX_ITER      100000
#define HALF          0.5
#define ONE           1.0
#define TWO           2.0

#define EG0_CUTOFF    1.0E-20
/* Global static read-only key word */
static const char keyword[KW_NUMBER][MAX_KW_LENGTH] = {
    "UNIVERSE",
    "CELL",
    "SURFACE",
    "SURF",
    "MATERIAL",
    "MAT",
    "SAB",
    "CRITICALITY",
    "POWERITER",
    "INITSRC"
};

/* number of boolean expression operator */
#define N_OPTR  6

/* 交,并,补,左括号,右括号,表达式结束 */
typedef enum {INTER, UNION, COMPLEMENT, L_P, R_P, EOE} Operator;

/* 大于、小于和等于号表示运算的优先级，空格表示该情况不存在 */
static const char priority[N_OPTR][N_OPTR] = {
/*-----------|---------------当前运算符--------------|*/
/*-----------&       :       !       (       )      \0*/
/*-|---&-*/ '>',    '>',    '<',    '<',    '>',    '>',
/*-栈--:-*/ '>',    '>',    '<',    '<',    '>',    '>',
/*-顶--!-*/ '>',    '>',    '>',    '<',    '>',    '>',
/*-运--(-*/ '<',    '<',    '<',    '<',    '=',    ' ',
/*-算--)-*/ ' ',    ' ',    ' ',    ' ',    ' ',    ' ',
/*-符-\0-*/ '<',    '<',    '<',    '<',    ' ',    '='
};

/* time elapsed */
extern time_t start_time;
extern time_t finish_time;

/* warnings found */
extern unsigned warnings;

#endif
