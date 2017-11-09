//
// Created by xaq on 10/27/17.
//

#include "common.h"

#ifndef TTYW_NEUTRON_TRANSPORT_H
#define TTYW_NEUTRON_TRANSPORT_H

#ifdef __cplusplus
extern "C"{
#endif

void geometry_tracking();

double sample_free_fly_dis(bool is_erg_changed);

void sample_col_nuclide();

void calc_col_nuc_cs();

void treat_implicit_capture();

void sample_col_type();

void get_exit_state();

void update_par_state();

#ifdef __cplusplus
}
#endif

#endif //TTYW_NEUTRON_TRANSPORT_H
