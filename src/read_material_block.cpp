//
// Created by xaq on 10/25/17.
//

#include "IO_releated.h"
#include "material.h"
#include "nuclide.h"
#include "map.h"
#include <vector>
#include <string>


extern map *base_mats;
extern IOfp_t base_IOfp;
extern map *base_nucs;

/* -------------------------- private prototypes ---------------------------- */
void
_extract_nucs(mat_t *mat,
              std::vector<std::pair<std::string, double> > &nucs);

/* ----------------------------- API implementation ------------------------- */
void
read_material_block()
{
    char buf[256];
    char *ret, *end, *kw_start;
    map_entry *nuc_entry;
    nuclide_t *nuc;
    mat_t *mat;
    int index;
    std::vector<std::pair<std::string, double> > nucs;

    mat = NULL;

    while((ret = fgets(buf, MAX_LINE_LENGTH, base_IOfp.inp_fp))) {
        while(ISSPACE(*ret)) ret++;

        if(ISALPHA(*ret)) {
            kw_start = ret;
            while(ISALPHA(*ret)) {
                *ret = TOUPPER(*ret);
                ret++;
            }
            *ret++ = 0;

            if(strcmp(kw_start, "MAT") == 0) {
                if(!nucs.empty() && mat)
                    _extract_nucs(mat, nucs);

                /* 更新mat的序号 */
                while(!ISNUMBER(*ret)) ret++;
                index = 0;
                do {
                    index *= 10;
                    index += *ret++ - '0';
                } while(ISNUMBER(*ret));

                /* 这里才真正更新了mat，指向新的材料，之前一直都是上一个材料 */
                mat = mat_init();
                map_put(base_mats, index, mat);
                mat->id = index;
                while(!ISNUMBER(*ret) && *ret != '-') ret++;
                mat->user_den = strtod(ret, &end);
            } else if(strcmp(kw_start, "SAB") == 0) {
                if(!nucs.empty() && mat)
                    _extract_nucs(mat, nucs);

                sscanf(ret, "%d", &index);
                mat = (mat_t *) map_get(base_mats, index);

                if(mat) {
                    mat->tot_sab_nuc_num++;
                    while(!ISALPHA(*ret)) ret++;
                    sscanf(ret, "%s", mat->sab_nuc_id);
                    nuc_entry = map_find(base_nucs, (uint64_t) mat->sab_nuc_id);
                    if(!nuc_entry) {
                        nuc = (nuclide_t *) malloc(sizeof(nuclide_t));
                        memset(nuc, 0x0, sizeof(nuclide_t));
                        strcpy(nuc->id, mat->sab_nuc_id);
                        map_put(base_nucs, (uint64_t) mat->sab_nuc_id, nuc);
                        mat->sab_nuc = nuc;
                    }
                } else {
                    puts("error SAB index.");
                    release_resource();
                    exit(0);
                }
            } else if(strcmp(kw_start, "CEACE") == 0) {
                /* 读取是否使用概率表(ptable) */
                continue;
            } else {
                puts("error in processing MATERIAL block.");
                release_resource();
                exit(0);
            }
        } else if(ISNUMBER(*ret)) {
            char id[10];
            double den;
            sscanf(ret, "%s %lf", id, &den);
            nucs.push_back(std::make_pair(std::string(id), den));
        } else if(ISCOMMENT(*ret)) continue;
        else if(ISRETURN(*ret)) break;
        else {
            puts("error in reading MATERIAL block.");
            release_resource();
            exit(0);
        }
    }

    if(!nucs.empty() && mat)
        _extract_nucs(mat, nucs);
}

/* ------------------------ private API implementation ---------------------- */
void
_extract_nucs(mat_t *mat,
              std::vector<std::pair<std::string, double> > &nucs)
{
    int i;
    map_entry *nuc_entry;
    nuclide_t *nuc;

    mat->tot_nuc_num = nucs.size();
    mat->nucs = (void **) malloc(mat->tot_nuc_num * sizeof(void *));
    mat->nuc_user_den = (double *) malloc(mat->tot_nuc_num * sizeof(double));
    for(i = 0; i < mat->tot_nuc_num; i++) {
        mat->nuc_user_den[i] = nucs[i].second;
        nuc_entry = map_find(base_nucs, (uint64_t) nucs[i].first.c_str());
        if(!nuc_entry) {
            nuc = (nuclide_t *) malloc(sizeof(nuclide_t));
            memset(nuc, 0x0, sizeof(nuclide_t));
            strcpy(nuc->id, nucs[i].first.c_str());
            map_put(base_nucs, (uint64_t) nuc->id, nuc);
            mat->nucs[i] = nuc;
        } else mat->nucs[i] = nuc_entry->v.val;
    }
    if(EQ_ZERO(mat->user_den))
        for(i = 0; i < mat->tot_nuc_num; i++)
            mat->user_den += mat->nuc_user_den[i];
    nucs.clear();
}
