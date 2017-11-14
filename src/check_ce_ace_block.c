//
// Created by xaq on 9/2/17.
//

#include "acedata.h"
#include "map.h"
#include "nuclide.h"

extern map *base_nucs;

void check_ce_ace_block(){
    map_entry *nuc_entry;
    nuclide_t *nuc;
    int *old_addr, old_sz;
    int table = base_nucs->table;
    for(int i = 0; i < base_nucs->ht->size; i++){
        nuc_entry = base_nucs->ht[table].buckets[i];
        while(nuc_entry){
            nuc = (nuclide_t *)nuc_entry->v.val;
            if(ISNUMBER(*nuc->id)){
                /// NU block
                /// MTR &  LSIG &  LAND  &  LDLW
                /// number of reactions excluding elastic
                int NMT_4 = Get_non_el_mt_num(nuc);
                /// NO MT array
                if(NMT_4 == 0){
                    /// maximum MT = 107 is requireed for reaction rate tally
                    nuc->MTR_index_sz = 110;
                    nuc->LAND_sz = 3;
                    /// elastic LAND
                    nuc->LAND = (int *)malloc(sizeof(int) * nuc->LAND_sz);
                    nuc->LAND[2] = (int)(nuc->XSS[Get_loc_of_LAND(nuc)]);
                }else{
                    int L3, L6, L8, L10, NMT_5, MT_max_4, MT_max_5;
                    NMT_5 = GetNonElMtNumWithNeu(nuc);
                    L3 = GetLocOfMTR(nuc);
                    L6 = GetLocOfLSIG(nuc);
                    L8 = GetLocOfLAND(nuc);
                    L10 = GetLocOfLDLW(nuc);
                    MT_max_4 = (int)(MAX(nuc->XSS[L3 + NMT_4 - 1], nuc->XSS[L3]));
                    MT_max_5 = (int)(MAX(nuc->XSS[L3 + NMT_5 - 1], nuc->XSS[L3]));
                    nuc->MTR_index_sz = MAX(110, MT_max_4 + 1);
                    nuc->MTR_index = (int *)malloc(sizeof(int) * nuc->MTR_index_sz); // need MT=107 when tally reaction rate
                    nuc->LSIG_sz = nuc->MTR_index_sz;
                    nuc->LSIG = (int)malloc(sizeof(int) * nuc->LSIG_sz);   //  only containing NXS_5 MT actually
                    nuc->LAND_sz = MT_max_5 + 3;
                    nuc->LAND = (int *)malloc(sizeof(int) * nuc->LAND_sz);             //  only containing NXS_5 MT actually
                    nuc->LAND[2] = (int)(nuc->XSS[L8]);           //  elastic LAND
                    nuc->LDLW_sz = MT_max_5 + 3;
                    nuc->LDLW = (int *)malloc(sizeof(int) * nuc->LDLW_sz);
                    for(int j = 1; j <= NMT_4; j++){
                        int MT_temp = (int)(nuc->XSS[L3 + j - 1]);
                        if(MT_temp <= 0){
                            printf("negative MT = %d for nuclide %s", MT_temp, nuc->id);
                            exit(0);
                        }

                        if(MT_temp >= nuc->MTR_index_sz){
                            /* resize obj->nucs[nuc]->MTR_index */
                            old_addr = nuc->MTR_index;
                            old_sz = nuc->MTR_index_sz;
                            nuc->MTR_index_sz = MT_temp + 1;
                            nuc->MTR_index = (int *)malloc(sizeof(int) * (MT_temp + 1));
                            memcpy(nuc->MTR_index, old_addr, old_sz * sizeof(int));
                            free(old_addr);

                            /* resize nuc->LSIG */
                            old_addr = nuc->LSIG;
                            old_sz = nuc->LSIG_sz;
                            nuc->LSIG_sz = MT_temp + 1;
                            nuc->LSIG = (int *)malloc(sizeof(int) * (MT_temp + 1));
                            memcpy(nuc->LSIG, old_addr, old_sz * sizeof(int));
                            free(old_addr);
                        }
                        nuc->MTR_index[MT_temp] = j;
                        nuc->LSIG[MT_temp] = (int)(nuc->XSS[L6 + j - 1]);

                        if(j <= NMT_5){
                            nuc->LAND[MT_temp] = (int)(nuc->XSS[L8 + j]);
                            nuc->LDLW[MT_temp] = (int)(nuc->XSS[L10 + j - 1]);
                        }
                    }
                }

                int NE = Get_erg_grid_num(nuc);
                nuc->fis_XSS_sz = NE + 1;
                nuc->fis_XSS = (double *)malloc(sizeof(double) * (NE + 1));
                nuc->inel_XSS_sz = NE + 1;
                nuc->inel_XSS = (double *)malloc(sizeof(double) * (NE + 1));

                bool bMt18Exist = false;
                for(int j = 1; j <= GetErgGridNum(nuc); ++j) {
                    nuc->fis_XSS[j] = 0;
                    nuc->inel_XSS[j] = 0;
                    int nloc = GetLocOfMTR(nuc) - 1;
                    for(int k = 1; k <= GetNonElMtNumWithNeu(nuc); k++) {
                        int MT_temp = (int)(nuc->XSS[nloc + k]);

                        if(MT_temp <= 0) {
                            printf("unknown MT number.\n   Nuc = %d, MT = XSS[%d] = %d.\n", nuc->zaid, nloc + k, MT_temp);
                            base_warnings++;
                        }

                        int nIE_LOCA = GetLocOfSIG(nuc) + nuc->LSIG[MT_temp] - 1;
                        int nSIG_IE = (int)(nuc->XSS[nIE_LOCA]);
                        int nSIG_NE = (int)(nuc->XSS[nIE_LOCA + 1]);
                        if((nSIG_IE + nSIG_NE - 1) != NE) {
                            printf("Abnormal cross-section of reaction MT=%d in nuc=%s \n", MT_temp, nuc->id);
                            exit(0);
                        }
                        if(MT_temp == 18)
                            bMt18Exist = true;
                        if(k >= nSIG_IE) {
                            if(MT_temp != 18 && MT_temp != 19 && MT_temp != 20 && MT_temp != 21 && MT_temp != 38) {
                                nuc->inel_XSS[k] = nuc->inel_XSS[k] + nuc->XSS[nIE_LOCA + 2 + k - nSIG_IE];
                            } else {
                                if(bMt18Exist && MT_temp == 18)
                                    nuc->fis_XSS[k] = nuc->XSS[nIE_LOCA + 2 + k - nSIG_IE];
                                else if(!bMt18Exist)
                                    nuc->fis_XSS[k] = nuc->fis_XSS[k] + nuc->XSS[nIE_LOCA + 2 + k - nSIG_IE];
                            }
                        }
                    }
                }
            }
            nuc_entry = nuc_entry->next;
        }
    }
}