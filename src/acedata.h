//
// Created by xaq on 9/2/17.
//

#ifndef CRITI_ACEDATA_H
#define CRITI_ACEDATA_H

#include "nuclide.h"
#include "particle_status.h"
#include "RNG.h"


typedef struct {
    int tot_nucs_include_sab;  /* 所有的非重复的核素数目，包括热化核素在内 */
    int tot_nucs_exclude_sab;  /* 所有非重复核素的数目，不包括热化核素在内 */
    double therm_func[52];
} acedata_t;

BEGIN_DECL
void
check_ce_ace_block();

void
calc_therm_Gfun(acedata_t *obj);

void
doppler_broaden();

void
get_nuc_tot_fis_cs(acedata_t *obj,
                   nuclide_t *nuc,
                   nuclide_t *sab_nuc,
                   nuc_xs_t *cur_nuc_xs,
                   double erg,
                   double cell_tmp,
                   RNG_t *RNG);

void
get_nuc_abs_scatt_cs(acedata_t *obj,
                     nuclide_t *nuc,
                     nuc_xs_t *cur_nuc_xs,
                     particle_status_t *par_status);

double
get_nuc_mt_cs(nuclide_t *nuc,
              int MT,
              int interp_pos,
              double interp_frac);

void
interpolate_sab(nuclide_t *nuc,
                nuclide_t *sab_nuc,
                nuc_xs_t *cur_nuc_xs,
                double erg);

void
treat_URR(nuclide_t *nuc,
          nuc_xs_t *cur_nuc_xs,
          RNG_t *RNG,
          double erg);

void
calc_erg_mu(const nuclide_t *nuc,
            RNG_t *RNG,
            int MT,
            double incident_erg,
            double *exit_erg_lab,
            double *exit_mu_lab);

double
get_scatt_cosine(const nuclide_t *nuc,
                 RNG_t *RNG,
                 int MT,
                 double incident_erg);

int
get_law_type(const nuclide_t *nuc,
             const int MT,
             const double incident_erg,
             RNG_t *RNG,
             int *LDAT);

void
react_by_laws(const nuclide_t *nuc,
              RNG_t *RNG,
              int MT,
              int law_type,
              int LDAT,
              double incident_erg,
              double *exit_erg_cm,
              double *mu_cm);

void
treat_sab_colli_type(const nuclide_t *sab_nuc,
                     particle_status_t *par_status,
                     RNG_t *RNG);

double
get_total_nu(nuclide_t *nuc,
             double erg);

double
get_delayed_nu(nuclide_t *nuc,
               double erg);

void
interpolate_xss_table(const nuclide_t *nuc,
                      double incident_erg,
                      int LDAT,
                      int *nPos,
                      double *dFrac,
                      int *num_of_interp_region,
                      int *num_of_erg_grid);

double
get_erg_func_value(const nuclide_t *nuc,
                   int LDAT,
                   double erg);
END_DECL

#endif //CRITI_ACEDATA_H