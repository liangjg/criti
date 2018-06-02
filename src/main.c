//
//  main.c
//  criti
//
//  Created by 叶鑫 on 2017/8/24.
//  Copyright © 2017年 xaq. All rights reserved.
//

#include <unistd.h>
#include "IO_releated.h"
#include "criticality.h"
#include "fixed_source.h"
#include "calculation.h"
#include "acedata.h"
#include "material.h"
#include "geometry.h"
#include "map.h"


#ifdef USE_MPI
#include "parallel.h"

parallel_t base_parallel;
#endif


/* 全局变量初始化 */
unsigned base_warnings;
double base_start_wgt;
int base_num_threads;
criti_t base_criti;
fixed_src_t base_fixed_src;
IOfp_t base_IOfp;
RNG_t base_RNG;
acedata_t base_acedata;
nuc_xs_t **base_nuc_xs;
int base_tot_nucs;

/* key: universe index; val: corresponding universe instance address */
map *base_univs;

/* key: material index; val: corresponding material instance address */
map *base_mats;

/* key: cell index; val: corresponding cell instance address */
map *base_cells;

/* key: surface index; val: corresponding surface instance address */
map *base_surfs;

/* key: nuclide id; val: corresponding nuclide instance address */
map *base_nucs;

/* -------------------------- hash function prototypes ---------------------------- */
static uint64_t
_int_hash_func(const void *key);

static uint64_t
_str_hash_func(const void *key);

static int
_str_key_comp_func(uint64_t key1,
                   uint64_t key2);

static void
_univ_free(void *obj);

static void
_cell_free(void *obj);

static void
_surf_free(void *obj);

static void
_mat_free(void *obj);

static void
_nuc_free(void *obj);

/* ------------------------ main function --------------------------- */
int
main(int argc,
     char *argv[])
{
    char mat_fn[MAX_FILENAME_LENGTH];
    char tally_fn[MAX_FILENAME_LENGTH];
    int c;

#ifdef USE_MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &base_parallel.tot_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &base_parallel.id);

    base_parallel.bank_load = malloc(base_parallel.tot_procs * sizeof(int));
    base_parallel.bank_load_sum = malloc((base_parallel.tot_procs + 1) * sizeof(int));
    base_parallel.src_load = malloc(base_parallel.tot_procs * sizeof(int));
    base_parallel.src_load_sum = malloc((base_parallel.tot_procs + 1) * sizeof(int));
    base_parallel.rand_num_pos = malloc(base_parallel.tot_procs * sizeof(int));
    base_parallel.rand_num_sum = 0;

    int block_length[1] = {8};
    MPI_Aint indices[1] = {0};
    MPI_Datatype old_types[1] = {MPI_DOUBLE};
    MPI_Type_struct(1, block_length, indices, old_types, &base_parallel.bank_type);
    MPI_Type_commit(&base_parallel.bank_type);
#endif

    opterr = 0;
    /* 设置默认参数 */
    base_num_threads = 1;
    strcpy(base_IOfp.xsdir_file_name, "xsdir");

    while((c = getopt(argc, argv, "ho:s:x:")) != -1) {
        switch(c) {
            case 'h': {
#ifdef USE_MPI
                if(IS_MASTER) {
#endif
#ifdef CODE_VERSION
                    printf("RMC -- Reactor Monte Carlo code, version: %s\n", CODE_VERSION);
#else
                    puts("RMC -- Reactor Monte Carlo code, version: Unknown version");
#endif
                    puts("Usage: RMC [OPTION...] FILE");
                    puts("Copyright (c) 2000-2017 REAL Tsinghua University. All Rights Reserved.\n");
                    puts("General options:");
                    puts("  -h        Print this help");
                    puts("  -o        Specify output file");
                    puts("  -x        Specify xsdir file, the default is \"xsdir\"");
#ifdef USE_PTHREAD
                    puts("  -s        Specify how many POSIX threads to calculate simultaneously");
#endif
                    puts("");
#ifdef USE_MPI
                }
                MPI_Finalize();
#endif
                return 0;
            }
            case 'o': {
                strcpy(base_IOfp.opt_file_name, optarg);
                break;
            }
            case 's': {
                base_num_threads = *optarg - '0';
                break;
            }
            case 'x': {
                strcpy(base_IOfp.xsdir_file_name, optarg);
                break;
            }
            default:
#ifdef USE_MPI
                if(IS_MASTER){
#endif
                    printf("Unknown option \"-%c\"!\n", c);
#ifdef USE_MPI
                }
                MPI_Finalize();
#endif
                return 0;
        }
    }

    /* 设置默认的输入输出文件 */
    strcpy(base_IOfp.inp_file_name, "inp");
    if(optind < argc)
        strcpy(base_IOfp.inp_file_name, argv[optind]);
    if(!base_IOfp.opt_file_name[0]) {    /* 没有指定输出文件名 */
        strcpy(base_IOfp.opt_file_name, base_IOfp.inp_file_name);
        strcat(base_IOfp.opt_file_name, ".out");
    }

    strcpy(mat_fn, base_IOfp.inp_file_name);
    strcat(mat_fn, ".mat");
    strcpy(tally_fn, base_IOfp.inp_file_name);
    strcat(tally_fn, ".tally");

    base_IOfp.inp_fp = fopen(base_IOfp.inp_file_name, "rb");
    if(!base_IOfp.inp_fp) {
#ifdef USE_MPI
        if(IS_MASTER)
            printf("%s does not exist.\n", base_IOfp.inp_file_name);
        MPI_Finalize();
#endif
        return 0;
    }

#ifdef USE_MPI
    if(IS_MASTER){
#endif
        base_IOfp.opt_fp = fopen(base_IOfp.opt_file_name, "wb");
        base_IOfp.mat_fp = fopen(mat_fn, "wb");
#ifdef USE_MPI
    }
#endif

    /* set hash functions of every map_type */
    map_type *mat_type = (map_type *) malloc(sizeof(map_type));
    map_type *univ_type = (map_type *) malloc(sizeof(map_type));
    map_type *cell_type = (map_type *) malloc(sizeof(map_type));
    map_type *surf_type = (map_type *) malloc(sizeof(map_type));
    map_type *nuc_type = (map_type *) malloc(sizeof(map_type));

    /* univ_type specification */
    univ_type->hash_func = _int_hash_func;
    univ_type->value_dup = NULL;
    univ_type->value_free = _univ_free;
    univ_type->key_compare = NULL;

    /* cell_type specification */
    cell_type->hash_func = _int_hash_func;
    cell_type->value_dup = NULL;
    cell_type->value_free = _cell_free;
    cell_type->key_compare = NULL;

    /* surf_type specification */
    surf_type->hash_func = _int_hash_func;
    surf_type->value_dup = NULL;
    surf_type->value_free = _surf_free;
    surf_type->key_compare = NULL;

    /* mat_type specification */
    mat_type->hash_func = _int_hash_func;
    mat_type->value_dup = NULL;
    mat_type->value_free = _mat_free;
    mat_type->key_compare = NULL;

    /* nuc_type specification */
    nuc_type->hash_func = _str_hash_func;
    nuc_type->value_dup = NULL;
    nuc_type->value_free = _nuc_free;
    nuc_type->key_compare = _str_key_comp_func;

    base_univs = map_create(univ_type);
    base_mats = map_create(mat_type);
    base_cells = map_create(cell_type);
    base_surfs = map_create(surf_type);
    base_nucs = map_create(nuc_type);

    CALC_MODE_T calc_mode;

    /* output heading */
#ifdef USE_MPI
    if(IS_MASTER)
#endif
        output_heading();

    /* read input file */
    read_input_blocks(&calc_mode);

    /* 进行几何预处理，包括构建邻居栅元等等 */
    preprocess_geometry();

    /* read ACE database */
    read_ace_data();

    /* 将用户输入的密度转换成程序使用的原子密度 */
    convert_mat_nuc_den();

    /* 输出material文件 */
#ifdef USE_MPI
    if(IS_MASTER)
#endif
        output_mat_file();

    /* 多普勒展宽 */
    doppler_broaden();

    /* check ACE data */
    check_ce_ace_block();

    /* run calculation */
    switch(calc_mode) {
        case CRITICALITY:
#ifdef USE_MPI
            if(IS_MASTER)
#endif
                puts("\n******** Calculation mode: criticality ********\n");
            calc_criticality(base_criti.tot_cycle_num);
            break;
        case FIXEDSOURCE:
#ifdef USE_MPI
            if(IS_MASTER)
#endif
                puts("\n******** Calculation mode: fixed-source ********\n");
            calc_fixed_src();
            break;
        default:
#ifdef USE_MPI
            if(IS_MASTER)
#endif
                puts("\n******** Unknown calculation mode. ********\n");
    }

    /* output ending */
#ifdef USE_MPI
    if(IS_MASTER)
#endif
        output_ending();

    /* release all resource */
    release_resource();

    free(mat_type);
    free(univ_type);
    free(cell_type);
    free(surf_type);
    free(nuc_type);

    return 0;
}

/* ------------------------ hash function implementation ---------------------- */
uint64_t
_int_hash_func(const void *key)
{
    return _default_int_hash_func(*(uint32_t *) key);
}

/* DJB string hash function */
uint64_t
_str_hash_func(const void *key)
{
    char *str = (char *) (*(uint64_t *) key);
    uint64_t hash = 5381;
    int c;
    while((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash;
}

int
_str_key_comp_func(uint64_t key1,
                   uint64_t key2)
{
    return strcmp((const char *) key1, (const char *) key2);
}

void
_univ_free(void *obj)
{
    univ_free((universe_t *) (obj));
}

void
_cell_free(void *obj)
{
    cell_free((cell_t *) (obj));
}

void
_surf_free(void *obj)
{
    surf_free((surface_t *) obj);
}

void
_mat_free(void *obj)
{
    mat_free((mat_t *) (obj));
}

void
_nuc_free(void *obj)
{
    nuc_free((nuclide_t *) (obj));
}
