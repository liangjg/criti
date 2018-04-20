//
// Created by xaq on 10/25/17.
//

#include "IO_releated.h"
#include "map.h"
#include "surface.h"


extern map *base_surfs;
extern IOfp_t base_IOfp;

void
read_surf_block()
{
    char buf[256];
    char *ret;

    while((ret = fgets(buf, MAX_LINE_LENGTH, base_IOfp.inp_fp))) {
        while(ISSPACE(*ret)) ret++;
        if(ISCOMMENT(*ret)) continue;
        if(ISRETURN(*ret)) break;    /* current line is blank, SURFACE block terminates */

        while(!ISNUMBER(*ret)) ret++;
        int index = 0;
        do {
            index *= 10;
            index += *ret++ - '0';
        } while(ISNUMBER(*ret));

        surface_t *surf = surf_init();
        surf->id = index;
        map_put(base_surfs, index, surf);

        while(!ISALPHA(*ret)) ret++;
        char *type_start = ret;

        if(TOUPPER(*type_start) == 'P') {    /* 平面类型 */
            type_start++;
            if(ISALPHA(*type_start)) {
                char c = TOUPPER(*type_start);
                if(c == 'X'){
                    surf->funcs->calc_surf_sense = calc_surf_sense_PX;
                    surf->funcs->get_surf_norm_vec = get_surf_norm_vec_PX;
                    surf->funcs->calc_dist_to_surf = calc_dist_to_surf_PX;
                }
                else if(c == 'Y'){
                    surf->funcs->calc_surf_sense = calc_surf_sense_PY;
                    surf->funcs->get_surf_norm_vec = get_surf_norm_vec_PY;
                    surf->funcs->calc_dist_to_surf = calc_dist_to_surf_PY;
                }
                else if(c == 'Z'){
                    surf->funcs->calc_surf_sense = calc_surf_sense_PZ;
                    surf->funcs->get_surf_norm_vec = get_surf_norm_vec_PZ;
                    surf->funcs->calc_dist_to_surf = calc_dist_to_surf_PZ;
                }
            } else {
                surf->funcs->calc_surf_sense = calc_surf_sense_P;
                surf->funcs->get_surf_norm_vec = get_surf_norm_vec_P;
                surf->funcs->calc_dist_to_surf = calc_dist_to_surf_P;
            }
        } else if(TOUPPER(*type_start) == 'S') {    /* 球面类型 */
            type_start++;
            if(ISALPHA(*type_start)) {
                char c = TOUPPER(*type_start);
                if(c == 'X'){
                    surf->funcs->calc_surf_sense = calc_surf_sense_SX;
                    surf->funcs->get_surf_norm_vec = get_surf_norm_vec_SX;
                    surf->funcs->calc_dist_to_surf = calc_dist_to_surf_SX;
                }
                else if(c == 'Y'){
                    surf->funcs->calc_surf_sense = calc_surf_sense_SY;
                    surf->funcs->get_surf_norm_vec = get_surf_norm_vec_SY;
                    surf->funcs->calc_dist_to_surf = calc_dist_to_surf_SY;
                }
                else if(c == 'Z'){
                    surf->funcs->calc_surf_sense = calc_surf_sense_SZ;
                    surf->funcs->get_surf_norm_vec = get_surf_norm_vec_SZ;
                    surf->funcs->calc_dist_to_surf = calc_dist_to_surf_SZ;
                }
                else if(c == 'O'){
                    surf->funcs->calc_surf_sense = calc_surf_sense_SO;
                    surf->funcs->get_surf_norm_vec = get_surf_norm_vec_SO;
                    surf->funcs->calc_dist_to_surf = calc_dist_to_surf_SO;
                }
            } else {
                surf->funcs->calc_surf_sense = calc_surf_sense_S;
                surf->funcs->get_surf_norm_vec = get_surf_norm_vec_S;
                surf->funcs->calc_dist_to_surf = calc_dist_to_surf_S;
            }
        } else if(TOUPPER(*type_start) == 'C') {    /* 圆柱类型 */
            type_start++;
            char c;
            if(*type_start == '/') {
                type_start++;
                c = TOUPPER(*type_start);
                if(c == 'X'){
                    surf->funcs->calc_surf_sense = calc_surf_sense_C_X;
                    surf->funcs->get_surf_norm_vec = get_surf_norm_vec_C_X;
                    surf->funcs->calc_dist_to_surf = calc_dist_to_surf_C_X;
                }
                else if(c == 'Y'){
                    surf->funcs->calc_surf_sense = calc_surf_sense_C_Y;
                    surf->funcs->get_surf_norm_vec = get_surf_norm_vec_C_Y;
                    surf->funcs->calc_dist_to_surf = calc_dist_to_surf_C_Y;
                }
                else if(c == 'Z'){
                    surf->funcs->calc_surf_sense = calc_surf_sense_C_Z;
                    surf->funcs->get_surf_norm_vec = get_surf_norm_vec_C_Z;
                    surf->funcs->calc_dist_to_surf = calc_dist_to_surf_C_Z;
                }
            } else {
                c = TOUPPER(*type_start);
                if(c == 'X'){
                    surf->funcs->calc_surf_sense = calc_surf_sense_CX;
                    surf->funcs->get_surf_norm_vec = get_surf_norm_vec_CX;
                    surf->funcs->calc_dist_to_surf = calc_dist_to_surf_CX;
                }
                else if(c == 'Y'){
                    surf->funcs->calc_surf_sense = calc_surf_sense_CY;
                    surf->funcs->get_surf_norm_vec = get_surf_norm_vec_CY;
                    surf->funcs->calc_dist_to_surf = calc_dist_to_surf_CY;
                }
                else if(c == 'Z'){
                    surf->funcs->calc_surf_sense = calc_surf_sense_CZ;
                    surf->funcs->get_surf_norm_vec = get_surf_norm_vec_CZ;
                    surf->funcs->calc_dist_to_surf = calc_dist_to_surf_CZ;
                }
            }
        }

        ret += 3;

        char *end;
        for(int i = 0; i < 4; i++) {
            surf->paras[i] = strtod(ret, &end);
            ret = end;
        }

        while(!ISRETURN(*ret) && !ISNUMBER(*ret)) ret++;

        if(ISNUMBER(*ret)) surf->bc = *ret - '0';
    }
}