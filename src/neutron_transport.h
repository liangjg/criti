//
// Created by xaq on 10/27/17.
//

#ifndef CRITI_NEUTRON_TRANSPORT_H
#define CRITI_NEUTRON_TRANSPORT_H

#include "nuclide.h"
#include "RNG.h"
#include "particle_status.h"
#include "pth_arg.h"


BEGIN_DECL
void
geometry_tracking(particle_status_t *par_status,
                  double *keff_wgt_sum,
                  nuc_xs_t *nuc_xs,
                  RNG_t *RNG);

void
geometry_tracking_fixed(particle_status_t *par_status,
                        nuc_xs_t *nuc_xs,
                        RNG_t *RNG);

double
sample_free_fly_dis(particle_status_t *par_status,
                    nuc_xs_t *nuc_xs,
                    struct RNG *RNG,
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
              RNG_t *RNG,
              double *keff_wgt_sum,
              bank_t *bank,
              int *cur_bank_cnt,
              double keff_final);

void
treat_implicit_capture(particle_status_t *par_status,
                       RNG_t *RNG);

void
treat_implicit_capture_fixed(particle_status_t *par_status,
                             RNG_t *RNG);

int
sample_col_type(particle_status_t *par_status,
                RNG_t *RNG);

int
sample_col_type_fixed(particle_status_t *par_status,
                      RNG_t *RNG);

void
get_exit_state(particle_status_t *par_status,
               RNG_t *RNG);

void
get_exit_state_fixed(particle_status_t *par_status,
                     RNG_t *RNG,
                     bank_t *bank,
                     int *bank_cnt);

void
rotate_dir(double mu,
           const double *old_dir,
           double *new_dir,
           RNG_t *RNG);

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
