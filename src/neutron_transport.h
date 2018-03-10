//
// Created by xaq on 10/27/17.
//

#include "common.h"
#include "particle_status.h"


#ifndef CRITI_NEUTRON_TRANSPORT_H
#define CRITI_NEUTRON_TRANSPORT_H

BEGIN_DECL
void
geometry_tracking(particle_status_t *par_status);

double
sample_free_fly_dis(particle_status_t *par_status,
                    bool erg_changed);

void
sample_col_nuclide(particle_status_t *par_status);

void
calc_col_nuc_cs(particle_status_t *par_status);

void
treat_fission(particle_status_t *par_status);

void
treat_implicit_capture(particle_status_t *par_status);

void
treat_implicit_capture_fixed(particle_status_t *par_status);

int
sample_col_type(particle_status_t *par_status);

int
sample_col_type_fixed(particle_status_t *par_status);

void
get_exit_state(particle_status_t *par_status);

void
get_exit_state_fixed(particle_status_t *par_status);

void
rotate_dir(double mu,
           const double *old_dir,
           double *new_dir);

void
treat_free_gas_model(particle_status_t *par_status,
                     double nuc_wgt);

void
get_ce_exit_state(particle_status_t *par_status,
                  int MT,
                  bool is_free_gas_col);
END_DECL

#endif //CRITI_NEUTRON_TRANSPORT_H
