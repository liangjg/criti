//
// Created by xaq on 9/2/17.
//

#include "acedata.h"

void check_ce_ace_block(acedata_t *obj, int nTotNucNum, int nTotNonSabNucNum)
{
    int *old_addr, old_sz;
    for(int nuc = 1; nuc <= nTotNonSabNucNum; ++nuc) {
        // NU block
        // MTR &  LSIG &  LAND  &  LDLW
        int NMT_4 = GetNonElMtNum(obj, nuc);    //number of reactions excluding elastic
        if(NMT_4 == 0) { // NO MT array
            obj->nucs[nuc]->MTR_index_sz = 110;
            obj->nucs[nuc]->MTR_index = new int[110];     // maximum MT = 107 is requireed for reaction rate tally
            obj->nucs[nuc]->LAND_sz = 3;
            obj->nucs[nuc]->LAND = new int[3];
            obj->nucs[nuc]->LAND[2] = int(obj->nucs[nuc]->XSS[GetLocOfLAND(obj, nuc)]); // elastic LAND
        } else {
            int L3, L6, L8, L10, NMT_5, MT_max_4, MT_max_5;
            NMT_5 = GetNonElMtNumWithNeu(obj, nuc);
            L3 = GetLocOfMTR(obj, nuc);
            L6 = GetLocOfLSIG(obj, nuc);
            L8 = GetLocOfLAND(obj, nuc);
            L10 = GetLocOfLDLW(obj, nuc);
            MT_max_4 = int(MAX(obj->nucs[nuc]->XSS[L3 + NMT_4 - 1], obj->nucs[nuc]->XSS[L3]));
            MT_max_5 = int(MAX(obj->nucs[nuc]->XSS[L3 + NMT_5 - 1], obj->nucs[nuc]->XSS[L3]));
            obj->nucs[nuc]->MTR_index_sz = MAX(110, MT_max_4 + 1);
            obj->nucs[nuc]->MTR_index = new int[MAX(110, MT_max_4 + 1)]; // need MT=107 when tally reaction rate
            obj->nucs[nuc]->LSIG_sz = MAX(110, MT_max_4 + 1);
            obj->nucs[nuc]->LSIG = new int[MAX(110, MT_max_4 + 1)];   //  only containing NXS_5 MT actually
            obj->nucs[nuc]->LAND_sz = MAX(110, MT_max_4 + 1);
            obj->nucs[nuc]->LAND = new int[MT_max_5 + 3];             //  only containing NXS_5 MT actually
            obj->nucs[nuc]->LAND[2] = int(obj->nucs[nuc]->XSS[L8]);           //  elastic LAND
            obj->nucs[nuc]->LDLW_sz = MT_max_5 + 3;
            obj->nucs[nuc]->LDLW = new int[MT_max_5 + 3];
            for(int i = 1; i <= NMT_4; i++) {
                int nMT_temp = int(obj->nucs[nuc]->XSS[L3 + i - 1]);
                if(nMT_temp <= 0) {
                    printf("negative MT = %d for nuclide %s \n", nMT_temp, obj->nucs[nuc]->id);
                    exit(0);
                }
                if(nMT_temp >= obj->nucs[nuc]->MTR_index_sz) {
                    /* resize obj->nucs[nuc]->MTR_index */
                    old_addr = obj->nucs[nuc]->MTR_index;
                    old_sz = obj->nucs[nuc]->MTR_index_sz;
                    obj->nucs[nuc]->MTR_index_sz = nMT_temp + 1;
                    obj->nucs[nuc]->MTR_index = new int[nMT_temp + 1];
                    memcpy(obj->nucs[nuc]->MTR_index, old_addr, old_sz * sizeof(int));
                    delete[]old_addr;

                    /* resize obj->nucs[nuc]->LSIG */
                    old_addr = obj->nucs[nuc]->LSIG;
                    old_sz = obj->nucs[nuc]->LSIG_sz;
                    obj->nucs[nuc]->LSIG_sz = nMT_temp + 1;
                    obj->nucs[nuc]->LSIG = new int[nMT_temp + 1];
                    memcpy(obj->nucs[nuc]->LSIG, old_addr, old_sz * sizeof(int));
                    delete[]old_addr;
                }

                obj->nucs[nuc]->MTR_index[nMT_temp] = i;
                obj->nucs[nuc]->LSIG[nMT_temp] = int(obj->nucs[nuc]->XSS[L6 + i - 1]);
                if(i <= NMT_5) {
                    obj->nucs[nuc]->LAND[nMT_temp] = int(obj->nucs[nuc]->XSS[L8 + i]);
                    obj->nucs[nuc]->LDLW[nMT_temp] = int(obj->nucs[nuc]->XSS[L10 + i - 1]);
                }
            }
        }

        // check negative values for URR probability table, 20140317, 20141212
        // negative value may cause negative xs when use patble ( CDAceData::TreatURR() )
        // ptable will be closed if find negative value
//        if(obj->p_bUseProbTable) {
//            int nLoc_lunr_table = GetLocOfLUNR(obj, nuc);
//            if(nLoc_lunr_table != 0) { // probability table exists
//                int nNum_prob_erg = int(obj->nucs[nuc]->XSS[nLoc_lunr_table]);
//                int nNum_prob_tab = int(obj->nucs[nuc]->XSS[nLoc_lunr_table + 1]);
//
//                int nStart_loc = nLoc_lunr_table + 6;
//                int nData_length = nNum_prob_erg * 6 * nNum_prob_tab;
//                for(int i = 0; i < nData_length; i++) {
//                    if(obj->nucs[nuc]->XSS[nStart_loc + i] < 0) {
//                        Output.OutputMessage([&]() {
//                            sprintf(Output.p_sPrintStr, "find negative value %f (xss[%d]) in probability table of nuclide %s. Ptable of this nuclide is disabled to avoid error.\n",
//                                    obj->nucs[nuc]->XSS[nStart_loc + i], nStart_loc + i, obj->nucs[nuc]->id);
//                        }, Output.p_fpOutputFilePtr, CDOutput::_WARNING);
//                        obj->nucs[nuc]->JXS[23] = 0;// close flag of urr ptable, only this nuclide
//                        break;
//                    }
//                }
//            }
//        }

        ////  Process FissXSS and InelXSS ////
        int nNE = GetErgGridNum(obj, nuc);
        obj->nucs[nuc]->fis_XSS = new double[nNE + 1];
        obj->nucs[nuc]->inel_XSS = new double[nNE + 1];

        bool bMt18Exist = false;
        for(int i = 1; i <= GetErgGridNum(obj, nuc); ++i) {
            obj->nucs[nuc]->fis_XSS[i] = 0;
            obj->nucs[nuc]->inel_XSS[i] = 0;
            int nloc = GetLocOfMTR(obj, nuc) - 1;
            for(int j = 1; j <= GetNonElMtNumWithNeu(obj, nuc); j++) {
                int nMT_temp = int(obj->nucs[nuc]->XSS[nloc + j]);

                if(nMT_temp <= 0) {
                    printf("unknown MT number.\n   Nuc = %d, MT = XSS[%d] = %d.\n", obj->nucs[nuc]->zaid, nloc + j, nMT_temp);
                    warnings++;
                }

                int nIE_LOCA = GetLocOfSIG(obj, nuc) + obj->nucs[nuc]->LSIG[nMT_temp] - 1;
                int nSIG_IE = int(obj->nucs[nuc]->XSS[nIE_LOCA]);
                int nSIG_NE = int(obj->nucs[nuc]->XSS[nIE_LOCA + 1]);
                if((nSIG_IE + nSIG_NE - 1) != nNE) {
                    printf("Abnormal cross-section of reaction MT=%d in nuc=%s \n", nMT_temp, obj->nucs[nuc]->id);
                    exit(0);
                }
                if(nMT_temp == 18)
                    bMt18Exist = true;
                if(i >= nSIG_IE) {
                    if(nMT_temp != 18 && nMT_temp != 19 && nMT_temp != 20 && nMT_temp != 21 && nMT_temp != 38) {
                        obj->nucs[nuc]->inel_XSS[i] = obj->nucs[nuc]->inel_XSS[i] + obj->nucs[nuc]->XSS[nIE_LOCA + 2 + i - nSIG_IE];
                    } else {
                        if(bMt18Exist && nMT_temp == 18)
                            obj->nucs[nuc]->fis_XSS[i] = obj->nucs[nuc]->XSS[nIE_LOCA + 2 + i - nSIG_IE];
                        else if(!bMt18Exist)
                            obj->nucs[nuc]->fis_XSS[i] = obj->nucs[nuc]->fis_XSS[i] + obj->nucs[nuc]->XSS[nIE_LOCA + 2 + i - nSIG_IE];
                    }
                }
            }
        }
    }
}