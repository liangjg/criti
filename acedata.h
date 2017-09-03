//
// Created by xaq on 9/2/17.
//

#ifndef TTYW_ACEDATA_H
#define TTYW_ACEDATA_H

#include "common.h"
#include "nuclide.h"

typedef struct {
    nuclide_t **nucs;  /* 长度为tot_nucs的数组，每个元素都是某一核素的地址 */
    int tot_nucs;      /* 所有的非重复的核素数目，包括热化核素在内 */
    double therm_func[52];
} acedata_t;


#ifdef __cplusplus
extern "C" {
#endif

#define GetXSSLength(obj, nNuc)               (Get_XSS_length((obj)->nucs[nNuc]))
#define GetNuclideZA(obj, nNuc)               (Get_nuclide_za((obj)->nucs[nNuc]))
#define GetErgGridNum(obj, nNuc)              (Get_erg_grid_num((obj)->nucs[nNuc]))
#define GetNonElMtNum(obj, nNuc)              (Get_non_el_mt_num((obj)->nucs[nNuc]))
#define GetNonElMtNumWithNeu(obj, nNuc)       (Get_non_el_mt_num_with_neu((obj)->nucs[nNuc]))
#define GetNPCR(obj, nNuc)                    (Get_NPCR((obj)->nucs[nNuc]))
#define GetLocOfESZ(obj, nNuc)                (Get_loc_of_ESZ((obj)->nucs[nNuc]))
#define GetLocOfNU(obj, nNuc)                 (Get_loc_of_NU((obj)->nucs[nNuc]))
#define GetLocOfMTR(obj, nNuc)                (Get_loc_of_MTR((obj)->nucs[nNuc]))
#define GetLocOfLQR(obj, nNuc)                (Get_loc_of_LQR((obj)->nucs[nNuc]))
#define GetLocOfTYR(obj, nNuc)                (Get_loc_of_TYR((obj)->nucs[nNuc]))
#define GetLocOfLSIG(obj, nNuc)               (Get_loc_of_LSIG((obj)->nucs[nNuc]))
#define GetLocOfSIG(obj, nNuc)                (Get_loc_of_SIG((obj)->nucs[nNuc]))
#define GetLocOfLAND(obj, nNuc)               (Get_loc_of_LAND((obj)->nucs[nNuc]))
#define GetLocOfAND(obj, nNuc)                (Get_loc_of_AND((obj)->nucs[nNuc]))
#define GetLocOfLDLW(obj, nNuc)               (Get_loc_of_LDLW((obj)->nucs[nNuc]))
#define GetLocOfDLW(obj, nNuc)                (Get_loc_of_DLW((obj)->nucs[nNuc]))
#define GetLocOfFIS(obj, nNuc)                (Get_loc_of_FIS((obj)->nucs[nNuc]))
#define GetLocOfLUNR(obj, nNuc)               (Get_loc_of_LUNR((obj)->nucs[nNuc]))
#define GetLocOfDNU(obj, nNuc)                (Get_loc_of_DNU((obj)->nucs[nNuc]))
#define GetLocOfBDD(obj, nNuc)                (Get_loc_of_BDD((obj)->nucs[nNuc]))
#define GetLocOfDNEDL(obj, nNuc)              (Get_loc_of_DNEDL((obj)->nucs[nNuc]))
#define GetLocOfDNED(obj, nNuc)               (Get_loc_of_DNED((obj)->nucs[nNuc]))
#define GetEmissNeuNum(obj, nNuc, nMT)        (Get_emiss_neu_num((obj)->nucs[nNuc], nMT))
#define GetNucFisQ(obj, nNuc, nMT)            (Get_nuc_fis_Q((obj)->nucs[nNuc], nMT))
#define GetSabInelMode(obj, SabNuc)           (Get_sab_inel_mode((obj)->nucs[SabNuc]))
#define GetSabInelDimPara(obj, SabNuc)        (Get_sab_inel_dim_para((obj)->nucs[SabNuc]))
#define GetSabInelEoutNum(obj, SabNuc)        (Get_sab_inel_eout_num((obj)->nucs[SabNuc]))
#define GetSabElMode(obj, SabNuc)             (Get_sab_el_mode((obj)->nucs[SabNuc]))
#define GetSabElDimPara(obj, SabNuc)          (Get_sab_el_dim_para((obj)->nucs[SabNuc]))
#define GetSabSecErgMode(obj, SabNuc)         (Get_sab_sec_erg_mode((obj)->nucs[SabNuc]))
#define GetLocOfSabInelErg(obj, SabNuc)       (Get_loc_of_sab_inel_erg((obj)->nucs[SabNuc]))
#define GetLocOfSabInelXs(obj, SabNuc)        (Get_loc_of_sab_inel_xs((obj)->nucs[SabNuc]))
#define GetLocOfSabInelErgMu(obj, SabNuc)     (Get_loc_of_sab_inel_erg_mu((obj)->nucs[SabNuc]))
#define GetLocOfSabElErg(obj, SabNuc)         (Get_loc_of_sab_el_erg((obj)->nucs[SabNuc]))
#define GetLocOfSabElXs(obj, SabNuc)          (Get_loc_of_sab_el_xs((obj)->nucs[SabNuc]))
#define GetLocOfSabElMu(obj, SabNuc)          (Get_loc_of_sab_el_mu((obj)->nucs[SabNuc]))

void check_ce_ace_block(acedata_t *obj, int nTotNucNum, int nTotNonSabNucNum);
void calc_therm_Gfun(acedata_t *obj);
int dpplr_brd_nuc_tmp(acedata_t *obj, int nNuc, double dTmp);
double erf2_function(double a);
void get_nuc_tot_fis_cs(acedata_t *obj, int nNuc, int nSabNuc, double dEg0, double dCell_T, bool bUseProbTable);
void get_nuc_abs_scatt_cs(acedata_t *obj, int nNuc, double dEg0, int nInterpPos0, double dInterpFrac0, int nInterpPos,
                          double dInterpFrac);
double get_nuc_mt_cs(acedata_t *obj, int nNuc, int nMT, int nInterpPos, double dInterpFrac);
void interpolate_sab(acedata_t *obj, int nNuc_n, int nNuc_sab, double dEg0);
void treat_URR(acedata_t *obj, int nNuc, double dEg0);
void get_ce_exist_erg_mu(acedata_t *obj, int nNuc, int nMT, double dIncidErg, double &dExitErg_Lab, double &dMu_Lab);
double get_scatt_cosine(acedata_t *obj, int nNuc, int nMT, double dIncidErg);
void get_law_type(acedata_t *obj, int nNuc, int nMT, double nIncidErg, int &nLawType, int &LDAT);
void react_by_laws(acedata_t *obj, int nNuc, int nMT, int nLawType, int LDAT, double dEin, double &dExitErgInCm,
                   double &dMuInCm);
void transform_cm_to_lab(acedata_t *obj, int nNuc, int nMT, double dEin, double dExitErgInCm, double dMuInCm, double &dExitErgInLab, double &dMuInLab);
void treat_sab_colli_type(acedata_t *obj, int nSabColliNuc, double dSIG_sab_el, double dSIG_sab_inel, double dEin,
                          double *dDirectin, double &dExitErgInLab, double *dDirectout);
double get_total_nu(acedata_t *obj, double dEg0, int nNuc);
double get_delayed_nu(acedata_t *obj, double dEg0, int nNuc);
void interpolate_xss_table(acedata_t *obj, double dErg, int nNuc, int LDAT, int &nPos, double &dFrac, int &NR, int &NE);
double get_erg_func_value(acedata_t *obj, int nNuc, int L, double dErg);
double sample_maxwell(acedata_t *obj, double T);
double sample_watt(acedata_t *obj, double a, double b);

/* functions for Doppler Broaden */
void dpplr_brdn_nuc_tot_cs(acedata_t *obj, int nNuc, double dCell_T, double dEg0);
void dppler_brdn_nuc_abs_scatt(acedata_t *obj, int nNuc, double dEg0);

#ifdef __cplusplus
}
#endif

#endif //TTYW_ACEDATA_H
