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

/* 全局变量初始化 */
unsigned base_warnings = 0;

unsigned base_errors = 0;

criti_t base_criti;

IOfp_t base_IOfp;

RNG_t base_RNG;

/* key: universe index; val: corresponding universe instance address */
map *base_univs = map_create(nullptr);

/* key: material index; val: corresponding material instance address */
map *base_mats = map_create(nullptr);

/* key: cell index; val: corresponding cell instance address */
map *base_cells = map_create(nullptr);

/* key: surface index; val: corresponding surface instance address */
map *base_surfs = map_create(nullptr);

/* -------------------------- hash function prototypes ---------------------------- */
uint64_t _int_hash_func(const void *key);

int main(int argc, char *argv[]){
    /* set hash function of base_map_type */
    map_type *base_map_type = new map_type;
    base_map_type->hash_func = _int_hash_func;
    base_map_type->value_dup = nullptr;
    base_map_type->value_free = nullptr;

    base_univs->type = base_map_type;
    base_mats->type = base_map_type;
    base_cells->type = base_map_type;
    base_surfs->type = base_map_type;

    /* check command line arguments */
    check_IO_file(argc, argv);

    /* output heading */
    output_heading();

    /* read input file */
    read_input_blocks();

    /* run calculation */
    //    run_calculation();

    /* output ending */
    //    output_ending();

    /* release all resource */
    //    release_resource();

    return 0;
}

/* ------------------------ hash function implementation ---------------------- */
uint64_t _int_hash_func(const void *key){
    return _default_int_hash_func(*(uint32_t *) key);
}
