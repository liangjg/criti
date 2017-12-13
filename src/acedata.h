//
// Created by xaq on 9/2/17.
//

#ifndef TTYW_ACEDATA_H
#define TTYW_ACEDATA_H

#include "common.h"
#include "nuclide.h"

typedef struct{
//    nuclide_t **nucs;           /* 长度为tot_nucs_include_sab的数组，前tot_nucs_exclude_sab为非热化核素 */
    int tot_nucs_include_sab;  /* 所有的非重复的核素数目，包括热化核素在内 */
    int tot_nucs_exclude_sab;  /* 所有非重复核素的数目，不包括热化核素在内 */
    double therm_func[52];
} acedata_t;


#ifdef __cplusplus
extern "C" {
#endif

#define GetXSSLength(nuclide)               Get_XSS_length((nuclide))
#define GetNuclideZA(nuclide)               Get_nuclide_za((nuclide))
#define GetErgGridNum(nuclide)              Get_erg_grid_num((nuclide))
#define GetNonElMtNum(nuclide)              Get_non_el_mt_num((nuclide))
#define GetNonElMtNumWithNeu(nuclide)       Get_non_el_mt_num_with_neu((nuclide))
#define GetNPCR(nuclide)                    Get_NPCR((nuclide))
#define GetLocOfESZ(nuclide)                Get_loc_of_ESZ((nuclide))
#define GetLocOfNU(nuclide)                 Get_loc_of_NU((nuclide))
#define GetLocOfMTR(nuclide)                Get_loc_of_MTR((nuclide))
#define GetLocOfLQR(nuclide)                Get_loc_of_LQR((nuclide))
#define GetLocOfTYR(nuclide)                Get_loc_of_TYR((nuclide))
#define GetLocOfLSIG(nuclide)               Get_loc_of_LSIG((nuclide))
#define GetLocOfSIG(nuclide)                Get_loc_of_SIG((nuclide))
#define GetLocOfLAND(nuclide)               Get_loc_of_LAND((nuclide))
#define GetLocOfAND(nuclide)                Get_loc_of_AND((nuclide))
#define GetLocOfLDLW(nuclide)               Get_loc_of_LDLW((nuclide))
#define GetLocOfDLW(nuclide)                Get_loc_of_DLW((nuclide))
#define GetLocOfFIS(nuclide)                Get_loc_of_FIS((nuclide))
#define GetLocOfLUNR(nuclide)               Get_loc_of_LUNR((nuclide))
#define GetLocOfDNU(nuclide)                Get_loc_of_DNU((nuclide))
#define GetLocOfBDD(nuclide)                Get_loc_of_BDD((nuclide))
#define GetLocOfDNEDL(nuclide)              Get_loc_of_DNEDL((nuclide))
#define GetLocOfDNED(nuclide)               Get_loc_of_DNED((nuclide))
#define GetEmissNeuNum(nuclide, MT)         Get_emiss_neu_num((nuclide), (MT))
#define GetNucFisQ(nuclide, MT)             Get_nuc_fis_Q((nuclide), (MT))
#define GetSabInelMode(sab_nuclide)         Get_sab_inel_mode((sab_nuclide))
#define GetSabInelDimPara(sab_nuclide)      Get_sab_inel_dim_para((sab_nuclide))
#define GetSabInelEoutNum(sab_nuclide)      Get_sab_inel_eout_num((sab_nuclide))
#define GetSabElMode(sab_nuclide)           Get_sab_el_mode((sab_nuclide))
#define GetSabElDimPara(sab_nuclide)        Get_sab_el_dim_para((sab_nuclide))
#define GetSabSecErgMode(sab_nuclide)       Get_sab_sec_erg_mode((sab_nuclide))
#define GetLocOfSabInelErg(sab_nuclide)     Get_loc_of_sab_inel_erg((sab_nuclide))
#define GetLocOfSabInelXs(sab_nuclide)      Get_loc_of_sab_inel_xs((sab_nuclide))
#define GetLocOfSabInelErgMu(sab_nuclide)   Get_loc_of_sab_inel_erg_mu((sab_nuclide))
#define GetLocOfSabElErg(sab_nuclide)       Get_loc_of_sab_el_erg((sab_nuclide))
#define GetLocOfSabElXs(sab_nuclide)        Get_loc_of_sab_el_xs((sab_nuclide))
#define GetLocOfSabElMu(sab_nuclide)        Get_loc_of_sab_el_mu((sab_nuclide))

void check_ce_ace_block();

void calc_therm_Gfun(acedata_t *obj);

void doppler_broaden();

double erf2_function(double a);

void get_nuc_tot_fis_cs(acedata_t *obj, nuclide_t *nuc, nuclide_t *sab_nuc, double erg, double cell_tmp);

void get_nuc_abs_scatt_cs(acedata_t *obj, nuclide_t *nuc, double erg, int nInterpPos0, double dInterpFrac0,
                          int nInterpPos, double dInterpFrac);

double get_nuc_mt_cs(nuclide_t *nuc, int MT, int nInterpPos, double dInterpFrac);

void interpolate_sab(nuclide_t *nuc, nuclide_t *sab_nuc, double erg);

void treat_URR(nuclide_t *nuc, double erg);

void get_ce_exist_erg_mu(const nuclide_t *nuc, int MT, double incident_erg, double *exit_erg_lab, double *exit_mu_lab);

double get_scatt_cosine(const nuclide_t *nuc, int MT, double dIncidErg);

int get_law_type(const nuclide_t *nuc, int MT, double incident_erg, int *LDAT);

void react_by_laws(const nuclide_t *nuc, int MT, int law_type, int LDAT, double incident_erg, double *exit_erg_cm, double *mu_cm);

void transform_cm_to_lab(const nuclide_t *nuc, int MT, double incident_erg, double exit_erg_cm, double exit_mu_cm,
                         double *exit_erg_lab, double *exit_mu_lab);

void treat_sab_colli_type(const nuclide_t *sab_nuc, double SIG_sab_el, double SIG_sab_inel, double incident_erg,
                          double *incident_dir, double *exit_erg_lab, double *exit_dir);

double get_total_nu(nuclide_t *nuc, double erg);

double get_delayed_nu(nuclide_t *nuc, double erg);

void interpolate_xss_table(const nuclide_t *nuc, double incident_erg, int LDAT, int *nPos, double *dFrac,
                           int *num_of_interp_region, int *num_of_erg_grid);

double get_erg_func_value(const nuclide_t *nuc, int LDAT, double erg);

/* functions for Doppler Broaden */
void dppler_brdn_nuc_tot_cs(acedata_t *obj, nuclide_t *nuc, double cell_tmp, double erg);

void dppler_brdn_nuc_abs_scatt(acedata_t *obj, nuclide_t *nuc, double erg);

int dppler_brdn_nuc_tmp(acedata_t *obj, nuclide_t *nuc, double tmp);

#ifdef __cplusplus
}
#endif

#endif //TTYW_ACEDATA_H
