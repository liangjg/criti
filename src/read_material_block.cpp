//
// Created by xaq on 10/25/17.
//

#include "IO_releated.h"
#include "map.h"
#include "material.h"
#include "nuclide.h"
#include <vector>
#include <string>

extern map *base_mats;
extern IOfp_t base_IOfp;
extern map *base_nucs;

void read_material_block(){
    char buf[256];
    char *ret, *end;
    map_entry *nuc_entry;
    nuclide_t *nuc;
    mat_t *mat;
    int index;
    std::vector<std::pair<std::string, double>> nucs;

    while((ret = fgets(buf, MAX_LINE_LENGTH, base_IOfp.inp_fp)) != nullptr){
        while(ISSPACE(*ret)) ret++;

        if(ISALPHA(*ret)){
            char *kw_start = ret;
            while(ISALPHA(*ret)){
                *ret = TOUPPER(*ret);
                ret++;
            }
            *ret++ = 0;

            if(strcmp(kw_start, "MAT") == 0){
                if(!nucs.empty() && mat){
                    mat->tot_nuc_num = nucs.size();
                    mat->nuc_id = new char *[mat->tot_nuc_num];
                    mat->nuc_user_den = new double[mat->tot_nuc_num];
                    for(int i = 0; i < mat->tot_nuc_num; i++){
                        mat->nuc_id[i] = new char[12];
                        strcpy(mat->nuc_id[i], nucs[i].first.c_str());
                        mat->nuc_user_den[i] = nucs[i].second;
                        nuc_entry = map_find(base_nucs, (uint64_t)mat->nuc_id[i]);
                        if(!nuc_entry){
                            nuc = new nuclide_t;
                            memset(nuc, 0x0, sizeof(nuclide_t));
                            strcpy(nuc->id, mat->nuc_id[i]);
                            map_put(base_nucs, (uint64_t)mat->nuc_id[i], nuc);
                        }
                    }
                    nucs.clear();
                }

                /* update material index */
                while(!ISNUMBER(*ret)) ret++;
                index = 0;
                do{
                    index *= 10;
                    index += *ret++ - '0';
                } while(ISNUMBER(*ret));

                /* update material point */
                mat = mat_init();
                map_put(base_mats, index, mat);
                while(!ISNUMBER(*ret) && *ret != '-') ret++;
                (mat->user_den = strtod(ret, &end)) < 0.0 ? mat->gram_den = mat->user_den
                                                          : mat->atom_den = mat->user_den;
            } else if(strcmp(kw_start, "SAB") == 0){
                if(!nucs.empty() && mat){
                    mat->tot_nuc_num = nucs.size();
                    mat->nuc_id = new char *[mat->tot_nuc_num];
                    mat->nuc_user_den = new double[mat->tot_nuc_num];
                    for(int i = 0; i < mat->tot_nuc_num; i++){
                        mat->nuc_id[i] = new char[12];
                        strcpy(mat->nuc_id[i], nucs[i].first.c_str());
                        mat->nuc_user_den[i] = nucs[i].second;
                        nuc_entry = map_find(base_nucs, (uint64_t)mat->nuc_id[i]);
                        if(!nuc_entry){
                            nuc = new nuclide_t;
                            memset(nuc, 0x0, sizeof(nuclide_t));
                            strcpy(nuc->id, mat->nuc_id[i]);
                            map_put(base_nucs, (uint64_t)mat->nuc_id[i], nuc);
                        }
                    }
                    nucs.clear();
                }

                sscanf(ret, "%d", &index);
                mat = (mat_t *) map_get(base_mats, index);

                if(mat){
                    mat->tot_sab_nuc_num++;
                    while(!ISALPHA(*ret)) ret++;
                    sscanf(ret, "%s", mat->sab_nuc_id);
                    nuc_entry = map_find(base_nucs, (uint64_t)mat->sab_nuc_id);
                    if(!nuc_entry){
                        nuc = new nuclide_t;
                        memset(nuc, 0x0, sizeof(nuclide_t));
                        strcpy(nuc->id, mat->sab_nuc_id);
                        map_put(base_nucs, (uint64_t)mat->sab_nuc_id, nuc);
                    }
                } else{
                    puts("error SAB index.");
                }
            } else puts("error in processing MATERIAL block.");
        } else if(ISNUMBER(*ret)){
            char id[10];
            double den;
            sscanf(ret, "%s %lf", id, &den);
            nucs.push_back(std::make_pair(std::string(id), den));
        } else if(ISCOMMENT(*ret)) continue;
        else if(ISRETURN(*ret)) break;
        else puts("error in processing MATERIAL block.");
    }
}