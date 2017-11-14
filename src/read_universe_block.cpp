//
// Created by xaq on 10/25/17.
//

#include "IO_releated.h"
#include "map.h"
#include "universe.h"
#include <cassert>

extern map *base_univs;
extern IOfp_t base_IOfp;

int _identify_univ_kw(char *kw);

void read_universe_block(char *buf){
    if(*buf == 0) return;

    while(ISSPACE(*buf)) buf++;
    int index = 0;
    if(ISNUMBER(*buf))
        do{
            index *= 10;
            index += *buf++ - '0';
        } while(ISNUMBER(*buf));

    universe_t *univ = univ_init();

    map_put(base_univs, index, univ);

    while(ISSPACE(*buf)) buf++;

    while(!ISRETURN(*buf) && !ISCOMMENT(*buf)){
        while(ISSPACE(*buf)) buf++;
        char *kw_start = buf;
        while(ISALPHA(*buf)) {
            *buf = TOUPPER(*buf);
            buf++;
        }
        *buf = 0;

        char *end;
        switch(_identify_univ_kw(kw_start)){
            case 0:{    /* MOVE */
                while(!ISNUMBER(*buf) && *buf != '-') buf++;
                univ->is_moved = true;
                for(int i = 0; i < 3; i++){
                    univ->origin[i] = strtod(buf, &end);
                    buf = end;
                }
                break;
            }
            case 1:{    /* ROTATE */
                while(!ISNUMBER(*buf) && *buf != '-') buf++;
                univ->is_rotated = true;
                for(int i = 0; i < 3; i++){
                    for(int j = 0; j < 3; j++){
                        univ->rotation[i][j] = strtod(buf, &end);
                        buf = end;
                    }
                }
                break;
            }
            case 2:{    /* LAT */
                while(!ISNUMBER(*buf)) buf++;
                univ->lattice_type = *buf - '0';
                assert(univ->lattice_type == 1 || univ->lattice_type == 2);
                univ->is_lattice = true;
                buf++;
                break;
            }
            case 3:{    /* PITCH */
                while(!ISNUMBER(*buf)) buf++;
                double pitch = 0.0;
                for(int i = 0; i < 3; i++){
                    pitch = strtod(buf, &end);
                    if(pitch > EPSILON)    /* pitch != 0.0 */
                        univ->pitch[i] = pitch;
                    buf = end;
                }
                break;
            }
            case 4:{    /* SCOPE */
                while(!ISNUMBER(*buf)) buf++;
                int scope = 0;
                for(int i = 0; i < 3; i++){
                    scope = strtol(buf, &end, 10);
                    univ->scope[i] = scope;
                    buf = end;
                }
                break;
            }
            case 5:{    /* SITA */
                while(!ISNUMBER(*buf) && *buf != '-') buf++;
                univ->sita = strtod(buf, &end);
                buf = end;
                break;
            }
            case 6:{    /* FILL */
                univ->filled_lat_num = univ->scope[0] * univ->scope[1];
                if(univ->lattice_type == 1) univ->filled_lat_num *= univ->scope[2];
                univ->fill_lat_univese = new int[univ->filled_lat_num];
                for(int i = 0; i < univ->filled_lat_num; i++)
                    fscanf(base_IOfp.inp_fp, "%d", univ->fill_lat_univese + i);
                break;
            }
            default:
                break;
        }
        while(ISSPACE(*buf)) buf++;
    }

    read_cell_card(univ);
}

int _identify_univ_kw(char *kw){
    int i;
    for(i = 0; i < UNIV_KW_NUMBER; i++){
        if(strcmp(kw, universe_kw[i]) == 0)
            return i;
    }
    return -1;
}