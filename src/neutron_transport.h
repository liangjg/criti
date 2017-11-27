//
// Created by xaq on 10/27/17.
//

#include "common.h"
#include "particle_state.h"

#ifndef TTYW_NEUTRON_TRANSPORT_H
#define TTYW_NEUTRON_TRANSPORT_H

#ifdef __cplusplus
extern "C"{
#endif

void geometry_tracking(particle_state_t *par_state);

double sample_free_fly_dis(particle_state_t *par_state, bool is_erg_changed);

void sample_col_nuclide(particle_state_t *par_state);

void calc_col_nuc_cs(particle_state_t *par_state);

void treat_fission(particle_state_t *par_state);

void treat_implicit_capture(particle_state_t *par_state);

int sample_col_type(particle_state_t *par_state);

void get_exit_state(particle_state_t *par_state);

void rotate_dir(double mu, const double *old_dir, double *new_dir);

void treat_free_gas_model(particle_state_t *par_state, double nuc_wgt);

void get_ce_exit_state(particle_state_t *par_state, int MT);

#ifdef __cplusplus
}
#endif

#endif //TTYW_NEUTRON_TRANSPORT_H
