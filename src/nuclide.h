//
// Created by xaq on 9/2/17.
//

#ifndef TTYW_NUCLIDE_H
#define TTYW_NUCLIDE_H

#include "common.h"

typedef struct {
    char id[12];
    int zaid;
    double atom_wgt;
    double tmp;
    double broaden_tmp;

    int NXS[17];              /* NXS[1~16], size=17 */
    int JXS[33];              /* JXS[1~32], size=33 */

    double *XSS;
    int XSS_sz;

    int *MTR_index;
    int MTR_index_sz;

    int *LSIG;
    int LSIG_sz;

    int *LAND;
    int LAND_sz;

    int *LDLW;
    int LDLW_sz;

    double *fis_XSS;
    int fis_XSS_sz;

    double *inel_XSS;
    int inel_XSS_sz;

    /* cross sections */
    double tot;
    double abs;
    double el;
    double inel;
    double fis;
    double nu;

    int inter_pos;
    double inter_frac;
//    int prob_table_flag;
} nuclide_t;

#ifdef __cplusplus
extern "C" {
#endif

/*ce ace data */
#define Get_XSS_length(nuclide)             ((nuclide)->NXS[1])
#define Get_nuclide_za(nuclide)             ((nuclide)->NXS[2])
#define Get_erg_grid_num(nuclide)           ((nuclide)->NXS[3])
#define Get_non_el_mt_num(nuclide)          ((nuclide)->NXS[4])
#define Get_non_el_mt_num_with_neu(nuclide) ((nuclide)->NXS[5])
#define Get_NPCR(nuclide)                   ((nuclide)->NXS[8])
#define Get_loc_of_ESZ(nuclide)             ((nuclide)->JXS[1])
#define Get_loc_of_NU(nuclide)              ((nuclide)->JXS[2])
#define Get_loc_of_MTR(nuclide)             ((nuclide)->JXS[3])
#define Get_loc_of_LQR(nuclide)             ((nuclide)->JXS[4])
#define Get_loc_of_TYR(nuclide)             ((nuclide)->JXS[5])
#define Get_loc_of_LSIG(nuclide)            ((nuclide)->JXS[6])
#define Get_loc_of_SIG(nuclide)             ((nuclide)->JXS[7])
#define Get_loc_of_LAND(nuclide)            ((nuclide)->JXS[8])
#define Get_loc_of_AND(nuclide)             ((nuclide)->JXS[9])
#define Get_loc_of_LDLW(nuclide)            ((nuclide)->JXS[10])
#define Get_loc_of_DLW(nuclide)             ((nuclide)->JXS[11])
#define Get_loc_of_FIS(nuclide)             ((nuclide)->JXS[21])
#define Get_loc_of_LUNR(nuclide)            ((nuclide)->JXS[23])
#define Get_loc_of_DNU(nuclide)             ((nuclide)->JXS[24])
#define Get_loc_of_BDD(nuclide)             ((nuclide)->JXS[25])
#define Get_loc_of_DNEDL(nuclide)           ((nuclide)->JXS[26])
#define Get_loc_of_DNED(nuclide)            ((nuclide)->JXS[27])
#define Get_emiss_neu_num(nuclide, MT)      ((MT) == 2 ? 1 : (int)((nuclide)->XSS[(nuclide)->JXS[5] + (nuclide)->MTR_index[MT] - 1]))
#define Get_nuc_fis_Q(nuclide, MT)          ((nuclide)->MTR_index[MT] <= 0 ? 0 : (nuclide)->XSS[(nuclide)->JXS[4] + (nuclide)->MTR_index[MT] - 1])

/* sab ace data */
#define Get_sab_inel_mode(nuclide)           ((nuclide)->NXS[2])
#define Get_sab_inel_dim_para(nuclide)       ((nuclide)->NXS[3])
#define Get_sab_inel_eout_num(nuclide)       ((nuclide)->NXS[4])
#define Get_sab_el_mode(nuclide)             ((nuclide)->NXS[5])
#define Get_sab_el_dim_para(nuclide)         ((nuclide)->NXS[6])
#define Get_sab_sec_erg_mode(nuclide)        ((nuclide)->NXS[7])
#define Get_loc_of_sab_inel_erg(nuclide)     ((nuclide)->JXS[1])
#define Get_loc_of_sab_inel_xs(nuclide)      ((nuclide)->JXS[2])
#define Get_loc_of_sab_inel_erg_mu(nuclide)  ((nuclide)->JXS[3])
#define Get_loc_of_sab_el_erg(nuclide)       ((nuclide)->JXS[4])
#define Get_loc_of_sab_el_xs(nuclide)        ((nuclide)->JXS[5])
#define Get_loc_of_sab_el_mu(nuclide)        ((nuclide)->JXS[6])

#ifdef __cplusplus
}
#endif

#endif //TTYW_NUCLIDE_H
