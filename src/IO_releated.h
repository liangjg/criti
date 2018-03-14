//
// Created by xaq on 9/2/17.
//

#ifndef CRITI_IO_RELEATED_H
#define CRITI_IO_RELEATED_H

#include "common.h"
#include "universe.h"


typedef struct {
    FILE *inp_fp;
    FILE *opt_fp;
    FILE *mat_fp;
    FILE *tally_fp;

    char inp_file_name[MAX_FILENAME_LENGTH];
    char opt_file_name[MAX_FILENAME_LENGTH];
} IOfp_t;

BEGIN_DECL
void
read_input_blocks(CALC_MODE_T *calc_mode);

void
read_universe_block(char *);

void
read_cell_card(universe_t *);

void
read_surf_block();

void
read_material_block();

void
read_criticality_block();

void
read_burnup_block();

void
read_fixed_src_block();

void
read_tally_block();

void
read_cell_tally_card();

void
read_mesh_tally_card();

void
read_tally_cells();

void
read_cs_tally_card();

void
read_ace_data();

void
output_mat_file();

void
output_summary();

void
output_summary_fixed();

void
output_heading();

void
output_ending();

END_DECL

#endif //CRITI_IO_RELEATED_H
