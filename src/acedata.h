//
// Created by xaq on 9/2/17.
//

#ifndef TTYW_ACEDATA_H
#define TTYW_ACEDATA_H

#include "common.h"
#include "nuclide.h"

typedef struct{
    int tot_nucs_include_sab;  /* 所有的非重复的核素数目，包括热化核素在内 */
    int tot_nucs_exclude_sab;  /* 所有非重复核素的数目，不包括热化核素在内 */
    double therm_func[52];
} acedata_t;


#ifdef __cplusplus
extern "C" {
#endif

void check_ce_ace_block();

void calc_therm_Gfun(acedata_t *obj);

void doppler_broaden();

double erf2_function(double a);

void get_nuc_tot_fis_cs(acedata_t *obj, nuclide_t *nuc, nuclide_t *sab_nuc, double erg, double cell_tmp);

void get_nuc_abs_scatt_cs(acedata_t *obj, nuclide_t *nuc, double erg, int interp_pos0, double interp_frac0,
                          int interp_pos, double interp_frac);

double get_nuc_mt_cs(nuclide_t *nuc, int MT, int interp_pos, double interp_frac);

void interpolate_sab(nuclide_t *nuc, nuclide_t *sab_nuc, double erg);

void treat_URR(nuclide_t *nuc, double erg);

void get_ce_exist_erg_mu(const nuclide_t *nuc, int MT, double incident_erg, double *exit_erg_lab, double *exit_mu_lab);

double get_scatt_cosine(const nuclide_t *nuc, int MT, double incident_erg);

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
