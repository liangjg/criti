//
// Created by xaq on 10/25/17.
//

#include "IO_releated.h"
#include "map.h"
#include "surface.h"

extern map *base_surfs;
extern IOfp_t base_IOfp;

void read_surf_block(){
    char buf[256];
    char *ret;

    while((ret = fgets(buf, MAX_LINE_LENGTH, base_IOfp.inp_fp)) != nullptr){
        while(ISSPACE(*ret)) ret++;
        if(ISCOMMENT(*ret)) continue;
        if(ISRETURN(*ret)) break;    /* current line is blank, SURFACE block terminates */

        while(!ISNUMBER(*ret)) ret++;
        int index = 0;
        do{
            index *= 10;
            index += *ret++ - '0';
        } while(ISNUMBER(*ret));

        surface_t *surf = surf_init();
        map_put(base_surfs, index, surf);

        while(!ISALPHA(*ret)) ret++;
        char *type_start = ret;

        if(TOUPPER(*type_start) == 'P'){
            type_start++;
            if(ISALPHA(*type_start)){
                char c = TOUPPER(*type_start);
                if(c == 'X')
                    surf->type = PX;
                else if(c == 'Y')
                    surf->type = PY;
                else if(c == 'Z')
                    surf->type = PZ;
            }
            else surf->type = P;
        }
        else if(TOUPPER(*type_start) == 'S'){
            type_start++;
            if(ISALPHA(*type_start)){
                char c = TOUPPER(*type_start);
                if(c == 'X')
                    surf->type = SX;
                else if(c == 'Y')
                    surf->type = SY;
                else if(c == 'Z')
                    surf->type = SZ;
            }
            else surf->type = S;
        }
        else if(TOUPPER(*type_start) == 'C'){
            type_start++;
            char c = TOUPPER(*type_start);
            if(c == 'X')
                surf->type = CX;
            else if(c == 'Y')
                surf->type = CY;
            else if(c == 'Z')
                surf->type = CZ;
        }

        ret += 2;

        char *end;
        for(int i = 0; i < 4; i++){
            surf->paras[i] = strtod(ret, &end);
            ret = end;
        }

        while(!ISRETURN(*ret) && !ISNUMBER(*ret)) ret++;

        if(ISNUMBER(*ret)) surf->bc = *ret - '0';
    }
}