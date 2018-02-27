//
// Created by xaq on 10/27/17.
//

#ifndef CRITI_NEUTRON_TRANSPORT_H
#define CRITI_NEUTRON_TRANSPORT_H

#include "nuclide.h"
#include "RNG.h"
#include "criticality.h"


BEGIN_DECL
void
geometry_tracking(particle_state_t *par_state, double *keff_wgt_sum_slave, nuc_cs_t *nuc_cs_slave, RNG_t *RNG_slave);

double
sample_free_fly_dis(particle_state_t *par_state, nuc_cs_t *nuc_cs_slave, struct RNG *RNG_slave, bool erg_changed);

void sample_col_nuclide(particle_state_t *par_state, nuc_cs_t *nuc_cs_slave, RNG_t *RNG_slave);

void calc_col_nuc_cs(particle_state_t *par_state, RNG_t *RNG_slave);

void treat_fission(particle_state_t *par_state, RNG_t *RNG_slave, fission_bank_t *fis_bank_slave, int *fis_bank_cnt,
                   double *keff_wgt_sum_slave, double keff_final);

void treat_implicit_capture(particle_state_t *par_state, RNG_t *RNG_slave);

int sample_col_type(particle_state_t *par_state, RNG_t *RNG_slave);

void get_exit_state(particle_state_t *par_state, RNG_t *RNG_slave);

void rotate_dir(double mu, const double *old_dir, double *new_dir, RNG_t *RNG_slave);

void treat_free_gas_model(particle_state_t *par_state, RNG_t *RNG_slave, double nuc_wgt);

void get_ce_exit_state(particle_state_t *par_state, RNG_t *RNG_slave, int MT, bool is_free_gas_col);
END_DECL

#endif //CRITI_NEUTRON_TRANSPORT_H
