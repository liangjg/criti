//
//  main.c
//  criti
//
//  Created by 叶鑫 on 2017/8/24.
//  Copyright © 2017年 xaq. All rights reserved.
//

#include "IO_releated.h"
#include "RNG.h"
#include "map.h"
#include "criticality.h"
#include "calculation.h"
#include "particle_state.h"
#include "acedata.h"
#include "material.h"
#include "geometry.h"

/* 全局变量初始化 */
unsigned base_warnings = 0;
double base_start_wgt = ZERO;
criti_t base_criti;
IOfp_t base_IOfp;
RNG_t base_RNG;
acedata_t base_acedata;

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
static uint64_t _int_hash_func(const void *key);

static uint64_t _str_hash_func(const void *key);

static int _str_key_comp_func(uint64_t key1, uint64_t key2);

static void _univ_free(void *obj);

static void _cell_free(void *obj);

static void _surf_free(void *obj);

static void _mat_free(void *obj);

static void _nuc_free(void *obj);

/* ------------------------ main function --------------------------- */
int main(int argc, char *argv[]){
    /* set hash functions of every map_type */
    map_type *mat_type = new map_type;
    map_type *univ_type = new map_type;
    map_type *cell_type = new map_type;
    map_type *surf_type = new map_type;
    map_type *nuc_type = new map_type;

    /* univ_type specification */
    univ_type->hash_func = _int_hash_func;
    univ_type->value_dup = nullptr;
    univ_type->value_free = _univ_free;
    univ_type->key_compare = nullptr;

    /* cell_type specification */
    cell_type->hash_func = _int_hash_func;
    cell_type->value_dup = nullptr;
    cell_type->value_free = _cell_free;
    cell_type->key_compare = nullptr;

    /* surf_type specification */
    surf_type->hash_func = _int_hash_func;
    surf_type->value_dup = nullptr;
    surf_type->value_free = _surf_free;
    surf_type->key_compare = nullptr;

    /* mat_type specification */
    mat_type->hash_func = _int_hash_func;
    mat_type->value_dup = nullptr;
    mat_type->value_free = _mat_free;
    mat_type->key_compare = nullptr;

    /* nuc_type specification */
    nuc_type->hash_func = _str_hash_func;
    nuc_type->value_dup = nullptr;
    nuc_type->value_free = _nuc_free;
    nuc_type->key_compare = _str_key_comp_func;

    base_univs = map_create(univ_type);
    base_mats = map_create(mat_type);
    base_cells = map_create(cell_type);
    base_surfs = map_create(surf_type);
    base_nucs = map_create(nuc_type);

    CALC_MODE_T calc_mode;

    /* check command line arguments */
    check_IO_file(argc, argv);

    /* output heading */
    output_heading();

    /* read input file */
    read_input_blocks(&calc_mode);

    /* read ACE database */
    read_ace_data();

    /* check ACE data */
    check_ce_ace_block();

    /* 将用户输入的密度转换成程序使用的原子密度 */
    convert_mat_nuc_den();

    /* 输出material文件 */
    output_mat_file();

    /* 构建邻居栅元加速几何查找 */
    build_neighbor_list();

    /* run calculation */
    switch(calc_mode){
        case CRITICALITY:
            puts("\n******** Calculation mode: criticality ********\n");
            calc_criticality();
            break;
        case FIXEDSOURCE:
            puts("\n******** Calculation mode: fixed-source ********\n");
//            calc_fixed_source();
            break;
        case BURNUP:
            puts("\n******** Calculation mode: burnup ********\n");
//            calc_burnup();
            break;
        case POINTBURN:
            puts("\n******** Calculation mode: point burnup ********\n");
//            calc_point_burn();
            break;
        default:
            puts("\n******** Unknown calculation mode. ********\n");
            break;
    }

    /* output ending */
    output_ending();

    /* release all resource */
    release_resource();

    delete mat_type;
    delete univ_type;
    delete cell_type;
    delete surf_type;
    delete nuc_type;

    return 0;
}

/* ------------------------ hash function implementation ---------------------- */
static uint64_t _int_hash_func(const void *key){
    return _default_int_hash_func(*(uint32_t *) key);
}

/* DJB string hash function */
static uint64_t _str_hash_func(const void *key){
    char *str = (char *) (*(uint64_t *) key);
    uint64_t hash = 5381;
    int c;
    while((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash;
}

static int _str_key_comp_func(uint64_t key1, uint64_t key2){
    return strcmp((const char *)key1, (const char *)key2);
}

static void _univ_free(void *obj){
    univ_free((universe_t *)(obj));
}

static void _cell_free(void *obj){
    cell_free((cell_t *)(obj));
}

static void _surf_free(void *obj){
    surf_free((surface_t *)(obj));
}

static void _mat_free(void *obj){
    mat_free((mat_t *)(obj));
}

static void _nuc_free(void *obj){
    nuc_free((nuclide_t *)(obj));
}