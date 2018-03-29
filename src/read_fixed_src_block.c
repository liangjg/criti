//
// Created by x1314aq on 18-1-17.
//

#include "IO_releated.h"
#include "fixed_source.h"
#include "RNG.h"


extern IOfp_t base_IOfp;
extern fixed_src_t base_fixed_src;
extern RNG_t base_RNG;

#define CONVERT_TO_UPPER(ptr)    \
    do{    \
        while(ISALPHA(*(ptr))){    \
            *(ptr) = TOUPPER(*(ptr));    \
            (ptr)++;    \
        }    \
    *(ptr) = 0;    \
    } while(0)

void
read_fixed_src_block()
{
    int i;
    char buf[256];
    char *end;
    char *ret = fgets(buf, MAX_LINE_LENGTH, base_IOfp.inp_fp);

    set_RNG_paras(&base_RNG, 2);

    while(ISSPACE(*ret)) ret++;

    /* 读取第一个关键字 */
    char *kw_start = ret;
    CONVERT_TO_UPPER(ret);

    if(strcmp(kw_start, "NEUTRON") != 0) {
        puts("Error: unknown source particle type.\nOnly neutron is supported.");
        release_resource();
        exit(0);
    }

    while(!ISALPHA(*ret)) ret++;

    /* 读取第二个关键字 */
    kw_start = ret;
    CONVERT_TO_UPPER(ret);

    if(strcmp(kw_start, "POPULATION") != 0) {
        puts("Error: key word POPULATION needed here after particle type.");
        release_resource();
        exit(0);
    }
    while(!ISNUMBER(*ret)) ret++;
    base_fixed_src.tot_neu_num = (int) strtol(ret, &end, 10);
    ret = end;

    while(!ISALPHA(*ret)) ret++;

    /* 读取第三个关键字 */
    kw_start = ret;
    CONVERT_TO_UPPER(ret);

    if(strcmp(kw_start, "POINT") == 0) {    /* POINT source */
        base_fixed_src.fsrc_type = POINT;
        while(!ISNUMBER(*ret)) ret++;
        for(i = 0; i < 3; i++) {
            base_fixed_src.fsrc_paras[i] = strtod(ret, &end);
            ret = end;
        }
    } else if(strcmp(kw_start, "SLAB") == 0) {    /* SLAB source */
        base_fixed_src.fsrc_type = SLAB;
        while(!ISNUMBER(*ret)) ret++;
        for(i = 0; i < 6; i++) {
            base_fixed_src.fsrc_paras[i] = strtod(ret, &end);
            ret = end;
        }
    } else if(strcmp(kw_start, "SPH") == 0) {    /* SPHERE source */
        base_fixed_src.fsrc_type = SPHERE;
        while(!ISNUMBER(*ret)) ret++;
        for(i = 0; i < 4; i++) {
            base_fixed_src.fsrc_paras[i] = strtod(ret, &end);
            ret = end;
        }
    } else if(strcmp(kw_start, "CYL/X") == 0) {    /* CYL/X source */
        base_fixed_src.fsrc_type = CYL_X;
        while(!ISNUMBER(*ret)) ret++;
        for(i = 0; i < 5; i++) {
            base_fixed_src.fsrc_paras[i] = strtod(ret, &end);
            ret = end;
        }
    } else if(strcmp(kw_start, "CYL/Y") == 0) {    /* CYL/Y source */
        base_fixed_src.fsrc_type = CYL_Y;
        while(!ISNUMBER(*ret)) ret++;
        for(i = 0; i < 5; i++) {
            base_fixed_src.fsrc_paras[i] = strtod(ret, &end);
            ret = end;
        }
    } else if(strcmp(kw_start, "CYL/Z") == 0) {    /* CYL/Z source */
        base_fixed_src.fsrc_type = CYL_Z;
        while(!ISNUMBER(*ret)) ret++;
        for(i = 0; i < 5; i++) {
            base_fixed_src.fsrc_paras[i] = strtod(ret, &end);
            ret = end;
        }
    } else {
        printf("Error: unknown source type %s.", kw_start);
        release_resource();
        exit(0);
    }

    while(!ISALPHA(*ret)) ret++;

    /* 读取第四个关键字 */
    kw_start = ret;
    CONVERT_TO_UPPER(ret);

    if(strcmp(kw_start, "ERG") != 0) {
        puts("Error: key word ERG needed here after source type.");
        release_resource();
        exit(0);
    }

    while(!ISNUMBER(*ret)) ret++;
    base_fixed_src.fsrc_erg = strtod(ret, &end);
}