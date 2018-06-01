//
// Created by xaq on 9/2/17.
//

#include "acedata.h"
#include "map.h"
#include "material.h"

#ifdef USE_MPI
#include "parallel.h"

extern parallel_t base_parallel;
#endif

extern map *base_nucs;
extern map *base_mats;
extern nuc_xs_t **base_nuc_xs;
extern int base_num_threads;

void
check_ce_ace_block()
{
    map_entry *entry;
    nuclide_t *nuc, *sab_nuc;
    mat_t *mat;
    double esa1, esa2;
    int location;
    int *old_addr, old_sz;
    map_iterator *mat_iter = map_get_iter(base_mats);
    map_iterator *nuc_iter = map_get_iter(base_nucs);

    /* 连续能量ACE截面模块 */
    while((entry = map_iter_next(nuc_iter))) {
        nuc = (nuclide_t *) entry->v.val;
        if(ISNUMBER(*nuc->id)) {
            /// NU block
            /// MTR &  LSIG &  LAND  &  LDLW
            /// number of reactions excluding elastic
            int NMT_4 = Get_non_el_mt_num(nuc);
            /// NO MT array
            if(NMT_4 == 0) {
                /// maximum MT = 107 is requireed for reaction rate tally
                nuc->MTR_index_sz = 110;
                nuc->MTR_index = calloc(nuc->MTR_index_sz, sizeof(int));
                nuc->LAND_sz = 3;
                /// elastic LAND
                nuc->LAND = calloc(nuc->LAND_sz, sizeof(int));
                nuc->LAND[2] = (int) (nuc->XSS[Get_loc_of_LAND(nuc)]);
            } else {
                int L3, L6, L8, L10, NMT_5, MT_max_4, MT_max_5;
                NMT_5 = Get_non_el_mt_num_with_neu(nuc);
                L3 = Get_loc_of_MTR(nuc);
                L6 = Get_loc_of_LSIG(nuc);
                L8 = Get_loc_of_LAND(nuc);
                L10 = Get_loc_of_LDLW(nuc);
                MT_max_4 = (int) (MAX(nuc->XSS[L3 + NMT_4 - 1], nuc->XSS[L3]));
                MT_max_5 = (int) (MAX(nuc->XSS[L3 + NMT_5 - 1], nuc->XSS[L3]));
                nuc->MTR_index_sz = MAX(110, MT_max_4 + 1);
                nuc->MTR_index = calloc(nuc->MTR_index_sz, sizeof(int)); // need MT=107 when tally reaction rate
                nuc->LSIG_sz = MAX(110, MT_max_4 + 1);
                nuc->LSIG = calloc(nuc->LSIG_sz, sizeof(int));             //  only containing NXS_5 MT actually
                nuc->LAND_sz = MT_max_5 + 3;
                nuc->LAND = calloc(nuc->LAND_sz, sizeof(int));           //  only containing NXS_5 MT actually
                nuc->LAND[2] = (int) (nuc->XSS[L8]);                              //  elastic LAND
                nuc->LDLW_sz = MT_max_5 + 3;
                nuc->LDLW = calloc(nuc->LDLW_sz, sizeof(int));
                for(int j = 1; j <= NMT_4; j++) {
                    int MT_temp = (int) (nuc->XSS[L3 + j - 1]);
                    if(MT_temp <= 0) {
#ifdef USE_MPI
                        if(IS_MASTER)
#endif
                            printf("negative MT = %d for nuclide %s", MT_temp, nuc->id);
                        release_resource();
                        exit(0);
                    }

                    if(MT_temp >= nuc->MTR_index_sz) {
                        /* resize nuc->MTR_index */
                        old_addr = nuc->MTR_index;
                        old_sz = nuc->MTR_index_sz;
                        nuc->MTR_index_sz = MT_temp + 1;
                        nuc->MTR_index = calloc(nuc->MTR_index_sz, sizeof(int));
                        memcpy(nuc->MTR_index, old_addr, old_sz * sizeof(int));
                        free(old_addr);

                        /* resize nuc->LSIG */
                        old_addr = nuc->LSIG;
                        old_sz = nuc->LSIG_sz;
                        nuc->LSIG_sz = MT_temp + 1;
                        nuc->LSIG = calloc(nuc->LSIG_sz, sizeof(int));
                        memcpy(nuc->LSIG, old_addr, old_sz * sizeof(int));
                        free(old_addr);
                    }
                    nuc->MTR_index[MT_temp] = j;
                    nuc->LSIG[MT_temp] = (int) (nuc->XSS[L6 + j - 1]);

                    if(j <= NMT_5) {
                        nuc->LAND[MT_temp] = (int) (nuc->XSS[L8 + j]);
                        nuc->LDLW[MT_temp] = (int) (nuc->XSS[L10 + j - 1]);
                    }
                }
            }

            if(Get_loc_of_LUNR(nuc)) {
                int num_prob_erg = (int) (nuc->XSS[Get_loc_of_LUNR(nuc)]);
                int num_prob_tab = (int) (nuc->XSS[Get_loc_of_LUNR(nuc) + 1]);
                int start_loc = Get_loc_of_LUNR(nuc) + 6;
                int data_length = num_prob_erg * 6 * num_prob_tab;

                for(int i = 0; i < data_length; i++) {
                    if(nuc->XSS[start_loc + i] < ZERO){
#ifdef USE_MPI
                        if(IS_MASTER)
#endif
                            printf("Find negative value %lf (xss[%d]) in ptable of %s. ptable is diabled to avoid error.\n",
                               nuc->XSS[start_loc + i], start_loc + i, nuc->id);
                        nuc->JXS[23] = 0;    /* 关闭该核素的概率表 */
                        break;
                    }
                }
                for(int i = 0; i < base_num_threads; i++)
                    base_nuc_xs[i][nuc->xs].ptable = Get_loc_of_LUNR(nuc);
            }

            int NE = Get_erg_grid_num(nuc);
            nuc->fis_XSS_sz = NE + 1;
            nuc->fis_XSS = calloc(nuc->fis_XSS_sz, sizeof(double));
            nuc->inel_XSS_sz = NE + 1;
            nuc->inel_XSS = calloc(nuc->inel_XSS_sz, sizeof(double));

            bool MT_18_exist = false;
            for(int j = 1; j <= Get_erg_grid_num(nuc); j++) {
                nuc->fis_XSS[j] = 0;
                nuc->inel_XSS[j] = 0;
                int loc = Get_loc_of_MTR(nuc) - 1;
                for(int k = 1; k <= Get_non_el_mt_num_with_neu(nuc); k++) {
                    int MT_temp = (int) (nuc->XSS[loc + k]);

                    if(MT_temp <= 0) {
#ifdef USE_MPI
                        if(IS_MASTER)
#endif
                            printf("unknown MT number.\n   Nuc = %d, MT = XSS[%d] = %d.\n", nuc->zaid, loc + k, MT_temp);
                        base_warnings++;
                    }

                    int IE_LOCA = Get_loc_of_SIG(nuc) + nuc->LSIG[MT_temp] - 1;
                    int SIG_IE = (int) (nuc->XSS[IE_LOCA]);
                    int SIG_NE = (int) (nuc->XSS[IE_LOCA + 1]);
                    if((SIG_IE + SIG_NE - 1) != NE) {
#ifdef USE_MPI
                        if(IS_MASTER)
#endif
                            printf("Abnormal cross-section of reaction MT=%d in nuc=%s \n", MT_temp, nuc->id);
                        release_resource();
                        exit(0);
                    }
                    if(MT_temp == 18)
                        MT_18_exist = true;
                    if(j >= SIG_IE) {
                        if(MT_temp != 18 && MT_temp != 19 && MT_temp != 20 && MT_temp != 21 && MT_temp != 38) {
                            nuc->inel_XSS[j] += nuc->XSS[IE_LOCA + 2 + j - SIG_IE];
                        } else {
                            if(MT_18_exist)
                                nuc->fis_XSS[j] = nuc->XSS[IE_LOCA + 2 + j - SIG_IE];
                            else
                                nuc->fis_XSS[j] += nuc->XSS[IE_LOCA + 2 + j - SIG_IE];
                        }
                    }
                }
            }
        }
    }


    /* 检查SAB截面模块 */
    while((entry = map_iter_next(mat_iter))) {
        mat = entry->v.val;
        if(mat->tot_sab_nuc_num) {
            sab_nuc = mat->sab_nuc;
            if(!sab_nuc)
                continue;
            for(int j = 0; j < mat->tot_nuc_num; j++) {
                nuc = mat->nucs[j];
                if(nuc->zaid == sab_nuc->zaid) {
                    location = Get_loc_of_sab_inel_erg(sab_nuc);
                    esa1 = sab_nuc->XSS[location + (int) (sab_nuc->XSS[location])];
                    if(Get_loc_of_sab_el_erg(sab_nuc) && Get_sab_el_mode(sab_nuc) != 4) {
                        location = Get_loc_of_sab_el_erg(sab_nuc);
                        esa2 = sab_nuc->XSS[location + (int) (sab_nuc->XSS[location])];
                        esa1 = MIN(esa1, esa2);
                    }
                    mat->sab_nuc_esa = esa1;
                }
            }
        }
    }

    map_release_iter(nuc_iter);
    map_release_iter(mat_iter);
}