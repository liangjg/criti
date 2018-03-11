//
// Created by xaq on 10/27/17.
//


#ifndef CRITI_NEUTRON_TRANSPORT_H
#define CRITI_NEUTRON_TRANSPORT_H

#include "particle_status.h"
#include "RNG.h"
#include "nuclide.h"
#include "pth_arg.h"


BEGIN_DECL
void
geometry_tracking(particle_status_t *par_status,
                  double *keff_wgt_sum,
                  nuc_xs_t *nuc_cs,
                  RNG_t *RNG);

double
sample_free_fly_dis(particle_status_t *par_status,
                    nuc_xs_t *nuc_xs,
                    RNG_t *RNG,
                    bool erg_changed);

void
sample_col_nuclide(particle_status_t *par_status,
                   nuc_xs_t *nuc_xs,
                   RNG_t *RNG);

void
calc_col_nuc_cs(particle_status_t *par_status,
                RNG_t *RNG);

void
treat_fission(particle_status_t *par_status,
              pth_arg_t *arg);

void
treat_implicit_capture(particle_status_t *par_status,
                       RNG_t *RNG);

void
treat_implicit_capture_fixed(particle_status_t *par_status);

int
sample_col_type(particle_status_t *par_status,
                RNG_t *RNG);

int
sample_col_type_fixed(particle_status_t *par_status);

void
get_exit_state(particle_status_t *par_status,
               RNG_t *RNG);

void
get_exit_state_fixed(particle_status_t *par_status);

void
rotate_dir(double mu,
           RNG_t *RNG,
           const double *old_dir,
           double *new_dir);

void
treat_free_gas_model(particle_status_t *par_status,
                     RNG_t *RNG,
                     double nuc_wgt);

void
get_ce_exit_state(particle_status_t *par_status,
                  RNG_t *RNG,
                  int MT,
                  bool is_free_gas_col);
END_DECL

#endif //CRITI_NEUTRON_TRANSPORT_H
