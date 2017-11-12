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

/* 全局变量初始化 */
unsigned base_warnings = 0;

unsigned base_errors = 0;

double base_start_wgt = 0.0;

criti_t base_criti;

IOfp_t base_IOfp;

RNG_t base_RNG;

CALC_MODE_T base_mode;

particle_state_t base_par_state;

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
uint64_t _int_hash_func(const void *key);

uint64_t _str_hash_func(const void *key);

int _str_key_comp_func(uint64_t key1, uint64_t key2);

/* ------------------------ main function --------------------------- */
int main(int argc, char *argv[]){
    /* set hash function of int_type */
    map_type *int_type = new map_type;
    map_type *str_type = new map_type;

    int_type->hash_func = _int_hash_func;
    int_type->value_dup = nullptr;
    int_type->value_free = nullptr;
    int_type->key_compare = nullptr;
    str_type->hash_func = _str_hash_func;
    str_type->value_dup = nullptr;
    str_type->value_free = nullptr;
    str_type->key_compare = _str_key_comp_func;

    base_univs = map_create(int_type);
    base_mats = map_create(int_type);
    base_cells = map_create(int_type);
    base_surfs = map_create(int_type);
    base_nucs = map_create(str_type);

    /* check command line arguments */
    check_IO_file(argc, argv);

    /* output heading */
    output_heading();

    /* read input file */
    read_input_blocks();

    /* read ACE database */
    read_ace_data();

    /* run calculation */
    run_calculation(base_mode);

    /* output ending */
    //    output_ending();

    /* release all resource */
    //    release_resource();

    delete int_type;
    delete str_type;

    return 0;
}

/* ------------------------ hash function implementation ---------------------- */
uint64_t _int_hash_func(const void *key){
    return _default_int_hash_func(*(uint32_t *) key);
}

/* DJB string hash function */
uint64_t _str_hash_func(const void *key){
    char *str = (char *) (*(uint64_t *) key);
    uint64_t hash = 5381;
    int c;
    while((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash;
}

int _str_key_comp_func(uint64_t key1, uint64_t key2){
    return strcmp((const char *)key1, (const char *)key2);
}