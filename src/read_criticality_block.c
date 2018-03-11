//
// Created by xaq on 10/26/17.
//

#include "IO_releated.h"
#include "criticality.h"
#include "RNG.h"


extern criti_t base_criti;
extern IOfp_t base_IOfp;
extern RNG_t base_RNG;

void
read_criticality_block()
{
    /* set the default arguments for base_criti and base_RNG */
    base_criti.keff_final = 1.0;
    set_RNG_paras(2);

    char buf[256];
    char *ret;

    while((ret = fgets(buf, MAX_LINE_LENGTH, base_IOfp.inp_fp))) {
        while(ISSPACE(*ret)) ret++;

        if(ISCOMMENT(*ret)) continue;
        if(ISRETURN(*ret)) break;

        char *kw_start = ret;
        while(ISALPHA(*ret)) {
            *ret = TOUPPER(*ret);
            ret++;
        }
        *ret = 0;

        if(strcmp(kw_start, "POWERITER") == 0) {
            while(!ISRETURN(*ret) && !ISCOMMENT(*ret)) {
                while(!ISALPHA(*ret)) ret++;

                char *sub_kw_start = ret;
                while(ISALPHA(*ret)) {
                    *ret = TOUPPER(*ret);
                    ret++;
                }
                *ret = 0;

                if(strcmp(sub_kw_start, "POPULATION") == 0) {
                    while(!ISNUMBER(*ret)) ret++;
                    char *end;
                    base_criti.cycle_neu_num = strtol(ret, &end, 10);
                    ret = end;
                    base_criti.inactive_cycle_num = strtol(ret, &end, 10);
                    ret = end;
                    base_criti.tot_cycle_num = strtol(ret, &end, 10);
                    ret = end;
                } else if(strcmp(sub_kw_start, "KEFF") == 0) {
                    while(!ISNUMBER(*ret)) ret++;
                    char *end;
                    base_criti.keff_final = strtod(ret, &end);
                    ret = end;
                } else puts("unknown key word.");
                while(ISSPACE(*ret)) ret++;
            }
        } else if(strcmp(kw_start, "INITSRC") == 0) {
            while(!ISRETURN(*ret) && !ISCOMMENT(*ret)) {
                while(!ISALPHA(*ret)) ret++;

                char *sub_kw_start = ret;
                while(ISALPHA(*ret)) {
                    *ret = TOUPPER(*ret);
                    ret++;
                }
                *ret = 0;

                if(strcmp(sub_kw_start, "POINT") == 0) {
                    base_criti.ksrc_type = POINT;

                    while(!ISNUMBER(*ret)) ret++;
                    char *end;
                    for(int i = 0; i < 3; i++) {
                        base_criti.ksrc_para[i] = strtod(ret, &end);
                        ret = end;
                    }
                    break;
                } else if(strcmp(sub_kw_start, "SLAB") == 0) {
                    base_criti.ksrc_type = SLAB;

                    while(!ISNUMBER(*ret)) ret++;
                    char *end;
                    for(int i = 0; i < 6; i++) {
                        base_criti.ksrc_para[i] = strtod(ret, &end);
                        ret = end;
                    }
                    break;
                } else if(strcmp(sub_kw_start, "SPHERE") == 0) {
                    base_criti.ksrc_type = SLAB;

                    while(!ISNUMBER(*ret)) ret++;
                    char *end;
                    for(int i = 0; i < 4; i++) {
                        base_criti.ksrc_para[i] = strtod(ret, &end);
                        ret = end;
                    }
                    break;
                } else puts("unknown key word.");

                while(ISSPACE(*ret)) ret++;
            }
            if(feof(base_IOfp.inp_fp)) return;
        } else if(strcmp(kw_start, "RNG") == 0) {
            /* TODO: complete this section */
            continue;
        } else if(strcmp(kw_start, "PARALLELBANK") == 0) {
            /* TODO: complete this section */
            continue;
        } else puts("unknown key word in CRITICALITY block.");
    }
}