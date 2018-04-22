//
// Created by xaq on 10/25/17.
//

#include "IO_releated.h"


#ifdef USE_MPI
#include "parallel.h"


extern parallel_t base_parallel;
#endif

extern IOfp_t base_IOfp;

/* -------------------------- private prototypes ---------------------------- */
int
_identify_kw(char *kw);

/* ----------------------------- API implementation ------------------------- */
void
read_input_blocks(CALC_MODE_T *calc_mode)
{
    char buf[256];
    char *ret;
    char *kw_start;

#ifdef USE_MPI
    if(IS_MASTER)
#endif
        printf("Reading input file...");

    while((ret = fgets(buf, MAX_LINE_LENGTH, base_IOfp.inp_fp))) {
        /* find the first non-space character */
        while(ISSPACE(*ret)) ret++;

        /* comment line or blank line */
        if(ISCOMMENT(*ret) || ISRETURN(*ret)) continue;

        if(ISALPHA(*ret)) {
            kw_start = ret;

            while(ISALPHA(*ret)) {
                *ret = TOUPPER(*ret);
                ret++;
            }
            *ret = 0;

            /* process all cases depending on key words */
            switch(_identify_kw(kw_start)) {
                case 0:    /* UNIVERSE */
                    ret++;
                    read_universe_block(ret);
                    break;
                case 1:    /* SURFACE */
                    read_surf_block();
                    break;
                case 2:    /* MATERIAL */
                    read_material_block();
                    break;
                case 3:    /* CRITICALITY */
                    *calc_mode = CRITICALITY;
                    read_criticality_block();
                    break;
                case 4:    /* TALLY */
                    /*read_tally_block();*/
                    break;
                case 5:    /* FIXEDSOURCE */
                    *calc_mode = FIXEDSOURCE;
                    read_fixed_src_block();
                case 6:    /* DEPLETION */
                    *calc_mode = POINTBURN;
                    break;
                case 7:    /* BURNUP */
                    *calc_mode = BURNUP;
                    /*read_burnup_block();*/
                    break;
                default:
                    printf("unknown key word %s.\n", kw_start);
                    break;
            }
        }
    }

#ifdef USE_MPI
    if(IS_MASTER)
#endif
        puts("Finished.");
}

/* ------------------------ private API implementation ---------------------- */
int
_identify_kw(char *kw)
{
    int i;
    for(i = 0; i < KW_NUMBER; i++) {
        if(strcmp(kw, keyword[i]) == 0)
            return i;
    }
    return -1;
}
