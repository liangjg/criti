#=============================================================================
# Set environment variables for the build.

# Set the target
KSRC_TARGET := build/bin/criti-ksrc
FSRC_TARGET := build/bin/criti-fsrc

# Set the C host compiler
CC_HOST := sw5cc -host

# Set the C slave compiler
CC_SLAVE := sw5cc -slave

# Set the CXX host compiler
CXX_HOST := mpiCC

# Set the Linker
LINKER := mpiCC

# Set the C compiler flags
C_FLAGS := -Wall -Wextra -pedantic -Wno-pointer-arith -O0 -g

# Set the C defines
C_DEFINES := -DUNIX

# Set the C header files search path
C_INCLUDES :=

# Set the CXX compiler flags
CXX_FLAGS := -Wall -Wextra -pedantic -Wno-pointer-arith -O0 -g

# Set the CXX defines
CXX_DEFINES := -DUNIX

# Set the CXX header files search path
CXX_INCLUDES :=

# # Debug build settings
# DBG_C_FLAGS = $(C_FLAGS) -O0 -g
# DBG_CXX_FLAGS = $(CXX_FLAGS) -O0 -g
# DBG_TARGET = $(TARGET)

# # Release build settings
# REL_C_FLAGS = $(C_FLAGS) -O3 -DNDEBUG
# REL_CXX_FLAGS = $(CXX_FLAGS) -O3 -DNDEBUG
# REL_TARGET = $(TARGET)

# Object files for target criti
criti_OBJECTS = \
build/objs/RNG_host.c.o \
build/objs/RNG_slave.c.o \
build/objs/build_neighbor_list.c.o \
build/objs/calc_col_nuc_cs.c.o \
build/objs/calc_dist_to_bound.c.o \
build/objs/calc_dist_to_lat.c.o \
build/objs/calc_dist_to_surf.c.o \
build/objs/calc_erg_mu.c.o \
build/objs/calc_surf_sense.c.o \
build/objs/calc_therm_Gfun.c.o \
build/objs/cell.c.o \
build/objs/check_ce_ace_block.c.o \
build/objs/convert_mat_nuc_den.c.o \
build/objs/doppler_broaden.c.o \
build/objs/find_lat_index.c.o \
build/objs/find_neighbor_cell.c.o \
build/objs/find_next_cell.c.o \
build/objs/geometry_tracking.c.o \
build/objs/geometry_tracking_fixed.c.o \
build/objs/get_ce_exit_state.c.o \
build/objs/get_delayed_nu.c.o \
build/objs/get_erg_func_value.c.o \
build/objs/get_exit_state.c.o \
build/objs/get_exit_state_fixed.c.o \
build/objs/get_fis_neu_state.c.o \
build/objs/get_fis_neu_state_fixed.c.o \
build/objs/get_law_type.c.o \
build/objs/get_nuc_abs_scatt_cs.c.o \
build/objs/get_nuc_mt_cs.c.o \
build/objs/get_nuc_tot_fis_cs.c.o \
build/objs/get_scatt_cosine.c.o \
build/objs/get_surf_norm_vec.c.o \
build/objs/get_tot_nu.c.o \
build/objs/global_fun.c.o \
build/objs/init_external_src.c.o \
build/objs/init_fission_source.c.o \
build/objs/interpolate_sab.c.o \
build/objs/interpolate_xss_table.c.o \
build/objs/locate_particle.c.o \
build/objs/main.c.o \
build/objs/map.c.o \
build/objs/material.c.o \
build/objs/move_to_origin_lat.c.o \
build/objs/nuclide.c.o \
build/objs/offset_neighbor_lat.c.o \
build/objs/output_ending.c.o \
build/objs/output_heading.c.o \
build/objs/output_mat_file.c.o \
build/objs/output_summary.c.o \
build/objs/output_summary_fixed.c.o \
build/objs/particle_is_in_cell.c.o \
build/objs/preprocess_geometry.c.o \
build/objs/process_cycle_end.c.o \
build/objs/react_by_law.c.o \
build/objs/read_ace_data.c.o \
build/objs/read_criticality_block.c.o \
build/objs/read_fixed_src_block.c.o \
build/objs/read_input_blocks.c.o \
build/objs/read_surf_block.c.o \
build/objs/read_universe_block.c.o \
build/objs/reflect_par.c.o \
build/objs/release_resource.c.o \
build/objs/rotate_dir.c.o \
build/objs/sample_col_nuclide.c.o \
build/objs/sample_col_type.c.o \
build/objs/sample_col_type_fixed.c.o \
build/objs/sample_free_fly_dis.c.o \
build/objs/sample_maxwell_host.c.o \
build/objs/sample_maxwell_slave.c.o \
build/objs/sample_watt.c.o \
build/objs/set_RNG_paras.c.o \
build/objs/surface.c.o \
build/objs/trans_univ_coord.c.o \
build/objs/trans_univ_dir.c.o \
build/objs/treat_URR.c.o \
build/objs/treat_fission.c.o \
build/objs/treat_free_gas_model.c.o \
build/objs/treat_implicit_capture.c.o \
build/objs/treat_implicit_capture_fixed.c.o \
build/objs/treat_sab_collision_type.c.o \
build/objs/universe.c.o \
build/objs/vector.c.o \
build/objs/read_cell_card.cpp.o \
build/objs/read_material_block.cpp.o


# KSRC objects
KSRC_OBJECTS = \
build/objs/calc_criticality.c.o \
build/objs/do_calc.c.o


# FSRC objects
FSRC_OBJECTS = \
build/objs/calc_fixed_src.c.o \
build/objs/do_calc_fixed.c.o


# External object files for target criti
criti_EXTERNAL_OBJECTS =


.PHONY: all clean ksrc fsrc


# Default build
all: ksrc

ksrc: C_DEFINES += -DKSRC
ksrc: CXX_DEFINES += -DKSRC
ksrc: $(KSRC_TARGET)

$(KSRC_TARGET): $(criti_OBJECTS) $(KSRC_OBJECTS)
	$(LINKER) $(criti_OBJECTS) $(KSRC_OBJECTS) -lm_slave -o $@


fsrc: C_DEFINES += -DFSRC
fsrc: CXX_DEFINES += -DFSRC
fsrc: $(FSRC_TARGET)

$(FSRC_TARGET): $(criti_OBJECTS) $(FSRC_OBJECTS)
	$(LINKER) $(criti_OBJECTS) $(FSRC_OBJECTS) -lm_slave -o $@


build/objs/RNG_host.c.o: src/RNG_host.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/RNG_host.c.o   -c src/RNG_host.c


build/objs/RNG_slave.c.o: src/RNG_slave.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/RNG_slave.c.o   -c src/RNG_slave.c


build/objs/build_neighbor_list.c.o: src/build_neighbor_list.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/build_neighbor_list.c.o   -c src/build_neighbor_list.c


build/objs/calc_col_nuc_cs.c.o: src/calc_col_nuc_cs.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/calc_col_nuc_cs.c.o   -c src/calc_col_nuc_cs.c


build/objs/calc_criticality.c.o: src/calc_criticality.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/calc_criticality.c.o   -c src/calc_criticality.c


build/objs/calc_dist_to_bound.c.o: src/calc_dist_to_bound.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/calc_dist_to_bound.c.o   -c src/calc_dist_to_bound.c


build/objs/calc_dist_to_lat.c.o: src/calc_dist_to_lat.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/calc_dist_to_lat.c.o   -c src/calc_dist_to_lat.c


build/objs/calc_dist_to_surf.c.o: src/calc_dist_to_surf.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/calc_dist_to_surf.c.o   -c src/calc_dist_to_surf.c


build/objs/calc_erg_mu.c.o: src/calc_erg_mu.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/calc_erg_mu.c.o  -c src/calc_erg_mu.c


build/objs/calc_fixed_src.c.o: src/calc_fixed_src.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/calc_fixed_src.c.o -c src/calc_fixed_src.c


build/objs/calc_surf_sense.c.o: src/calc_surf_sense.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/calc_surf_sense.c.o   -c src/calc_surf_sense.c


build/objs/calc_therm_Gfun.c.o: src/calc_therm_Gfun.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/calc_therm_Gfun.c.o   -c src/calc_therm_Gfun.c


build/objs/cell.c.o: src/cell.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/cell.c.o   -c src/cell.c


build/objs/check_ce_ace_block.c.o: src/check_ce_ace_block.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/check_ce_ace_block.c.o   -c src/check_ce_ace_block.c


build/objs/convert_mat_nuc_den.c.o: src/convert_mat_nuc_den.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/convert_mat_nuc_den.c.o   -c src/convert_mat_nuc_den.c


build/objs/do_calc.c.o: src/do_calc.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/do_calc.c.o   -c src/do_calc.c


build/objs/do_calc_fixed.c.o: src/do_calc_fixed.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/do_calc_fixed.c.o -c src/do_calc_fixed.c


build/objs/doppler_broaden.c.o: src/doppler_broaden.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/doppler_broaden.c.o   -c src/doppler_broaden.c


build/objs/find_lat_index.c.o: src/find_lat_index.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/find_lat_index.c.o   -c src/find_lat_index.c


build/objs/find_neighbor_cell.c.o: src/find_neighbor_cell.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/find_neighbor_cell.c.o   -c src/find_neighbor_cell.c


build/objs/find_next_cell.c.o: src/find_next_cell.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/find_next_cell.c.o   -c src/find_next_cell.c


build/objs/geometry_tracking.c.o: src/geometry_tracking.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/geometry_tracking.c.o   -c src/geometry_tracking.c


build/objs/geometry_tracking_fixed.c.o: src/geometry_tracking_fixed.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/geometry_tracking_fixed.c.o  -c src/geometry_tracking_fixed.c


build/objs/get_ce_exit_state.c.o: src/get_ce_exit_state.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/get_ce_exit_state.c.o   -c src/get_ce_exit_state.c


build/objs/get_delayed_nu.c.o: src/get_delayed_nu.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/get_delayed_nu.c.o   -c src/get_delayed_nu.c


build/objs/get_erg_func_value.c.o: src/get_erg_func_value.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/get_erg_func_value.c.o   -c src/get_erg_func_value.c


build/objs/get_exit_state.c.o: src/get_exit_state.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/get_exit_state.c.o   -c src/get_exit_state.c


build/objs/get_exit_state_fixed.c.o: src/get_exit_state_fixed.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/get_exit_state_fixed.c.o   -c src/get_exit_state_fixed.c


build/objs/get_fis_neu_state.c.o: src/get_fis_neu_state.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/get_fis_neu_state.c.o   -c src/get_fis_neu_state.c


build/objs/get_fis_neu_state_fixed.c.o: src/get_fis_neu_state_fixed.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/get_fis_neu_state_fixed.c.o   -c src/get_fis_neu_state_fixed.c


build/objs/get_law_type.c.o: src/get_law_type.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/get_law_type.c.o   -c src/get_law_type.c


build/objs/get_nuc_abs_scatt_cs.c.o: src/get_nuc_abs_scatt_cs.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/get_nuc_abs_scatt_cs.c.o   -c src/get_nuc_abs_scatt_cs.c


build/objs/get_nuc_mt_cs.c.o: src/get_nuc_mt_cs.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/get_nuc_mt_cs.c.o   -c src/get_nuc_mt_cs.c


build/objs/get_nuc_tot_fis_cs.c.o: src/get_nuc_tot_fis_cs.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/get_nuc_tot_fis_cs.c.o   -c src/get_nuc_tot_fis_cs.c


build/objs/get_scatt_cosine.c.o: src/get_scatt_cosine.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/get_scatt_cosine.c.o   -c src/get_scatt_cosine.c


build/objs/get_surf_norm_vec.c.o: src/get_surf_norm_vec.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/get_surf_norm_vec.c.o   -c src/get_surf_norm_vec.c


build/objs/get_tot_nu.c.o: src/get_tot_nu.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/get_tot_nu.c.o   -c src/get_tot_nu.c


build/objs/global_fun.c.o: src/global_fun.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/global_fun.c.o   -c src/global_fun.c


build/objs/init_external_src.c.o: src/init_external_src.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/init_external_src.c.o  -c src/init_external_src.c


build/objs/init_fission_source.c.o: src/init_fission_source.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/init_fission_source.c.o   -c src/init_fission_source.c


build/objs/interpolate_sab.c.o: src/interpolate_sab.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/interpolate_sab.c.o   -c src/interpolate_sab.c


build/objs/interpolate_xss_table.c.o: src/interpolate_xss_table.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/interpolate_xss_table.c.o   -c src/interpolate_xss_table.c


build/objs/locate_particle.c.o: src/locate_particle.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/locate_particle.c.o   -c src/locate_particle.c


build/objs/main.c.o: src/main.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/main.c.o   -c src/main.c


build/objs/map.c.o: src/map.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/map.c.o   -c src/map.c


build/objs/material.c.o: src/material.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/material.c.o   -c src/material.c


build/objs/move_to_origin_lat.c.o: src/move_to_origin_lat.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/move_to_origin_lat.c.o   -c src/move_to_origin_lat.c


build/objs/nuclide.c.o: src/nuclide.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/nuclide.c.o   -c src/nuclide.c


build/objs/offset_neighbor_lat.c.o: src/offset_neighbor_lat.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/offset_neighbor_lat.c.o   -c src/offset_neighbor_lat.c


build/objs/output_ending.c.o: src/output_ending.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/output_ending.c.o   -c src/output_ending.c


build/objs/output_heading.c.o: src/output_heading.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/output_heading.c.o   -c src/output_heading.c


build/objs/output_mat_file.c.o: src/output_mat_file.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/output_mat_file.c.o   -c src/output_mat_file.c


build/objs/output_summary.c.o: src/output_summary.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/output_summary.c.o   -c src/output_summary.c


build/objs/output_summary_fixed.c.o: src/output_summary_fixed.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/output_summary_fixed.c.o  -c src/output_summary_fixed.c


build/objs/particle_is_in_cell.c.o: src/particle_is_in_cell.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/particle_is_in_cell.c.o   -c src/particle_is_in_cell.c


build/objs/preprocess_geometry.c.o: src/preprocess_geometry.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/preprocess_geometry.c.o -c src/preprocess_geometry.c


build/objs/process_cycle_end.c.o: src/process_cycle_end.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/process_cycle_end.c.o   -c src/process_cycle_end.c


build/objs/react_by_law.c.o: src/react_by_law.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/react_by_law.c.o   -c src/react_by_law.c


build/objs/read_ace_data.c.o: src/read_ace_data.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/read_ace_data.c.o   -c src/read_ace_data.c


build/objs/read_criticality_block.c.o: src/read_criticality_block.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/read_criticality_block.c.o   -c src/read_criticality_block.c


build/objs/read_fixed_src_block.c.o: src/read_fixed_src_block.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/read_fixed_src_block.c.o  -c src/read_fixed_src_block.c


build/objs/read_input_blocks.c.o: src/read_input_blocks.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/read_input_blocks.c.o   -c src/read_input_blocks.c


build/objs/read_surf_block.c.o: src/read_surf_block.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/read_surf_block.c.o   -c src/read_surf_block.c


build/objs/read_universe_block.c.o: src/read_universe_block.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/read_universe_block.c.o   -c src/read_universe_block.c


build/objs/reflect_par.c.o: src/reflect_par.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/reflect_par.c.o   -c src/reflect_par.c


build/objs/release_resource.c.o: src/release_resource.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/release_resource.c.o   -c src/release_resource.c


build/objs/rotate_dir.c.o: src/rotate_dir.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/rotate_dir.c.o   -c src/rotate_dir.c


build/objs/sample_col_nuclide.c.o: src/sample_col_nuclide.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/sample_col_nuclide.c.o   -c src/sample_col_nuclide.c


build/objs/sample_col_type.c.o: src/sample_col_type.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/sample_col_type.c.o   -c src/sample_col_type.c


build/objs/sample_col_type_fixed.c.o: src/sample_col_type_fixed.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/sample_col_type_fixed.c.o  -c src/sample_col_type_fixed.c


build/objs/sample_free_fly_dis.c.o: src/sample_free_fly_dis.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/sample_free_fly_dis.c.o   -c src/sample_free_fly_dis.c


build/objs/sample_maxwell_host.c.o: src/sample_maxwell_host.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/sample_maxwell_host.c.o   -c src/sample_maxwell_host.c


build/objs/sample_maxwell_slave.c.o: src/sample_maxwell_slave.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/sample_maxwell_slave.c.o   -c src/sample_maxwell_slave.c


build/objs/sample_watt.c.o: src/sample_watt.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/sample_watt.c.o   -c src/sample_watt.c


build/objs/set_RNG_paras.c.o: src/set_RNG_paras.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/set_RNG_paras.c.o    -c src/set_RNG_paras.c


build/objs/surface.c.o: src/surface.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/surface.c.o   -c src/surface.c


build/objs/trans_univ_coord.c.o: src/trans_univ_coord.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/trans_univ_coord.c.o   -c src/trans_univ_coord.c


build/objs/trans_univ_dir.c.o: src/trans_univ_dir.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/trans_univ_dir.c.o   -c src/trans_univ_dir.c


build/objs/treat_URR.c.o: src/treat_URR.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/treat_URR.c.o   -c src/treat_URR.c


build/objs/treat_fission.c.o: src/treat_fission.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/treat_fission.c.o   -c src/treat_fission.c


build/objs/treat_free_gas_model.c.o: src/treat_free_gas_model.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/treat_free_gas_model.c.o   -c src/treat_free_gas_model.c


build/objs/treat_implicit_capture.c.o: src/treat_implicit_capture.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/treat_implicit_capture.c.o   -c src/treat_implicit_capture.c


build/objs/treat_implicit_capture_fixed.c.o: src/treat_implicit_capture_fixed.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/treat_implicit_capture_fixed.c.o  -c src/treat_implicit_capture_fixed.c


build/objs/treat_sab_collision_type.c.o: src/treat_sab_collision_type.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/treat_sab_collision_type.c.o   -c src/treat_sab_collision_type.c


build/objs/universe.c.o: src/universe.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/universe.c.o   -c src/universe.c


build/objs/vector.c.o: src/vector.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o build/objs/vector.c.o   -c src/vector.c


build/objs/read_cell_card.cpp.o: src/read_cell_card.cpp
	$(CXX_HOST)  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o build/objs/read_cell_card.cpp.o -c src/read_cell_card.cpp


build/objs/read_material_block.cpp.o: src/read_material_block.cpp
	$(CXX_HOST)  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o build/objs/read_material_block.cpp.o -c src/read_material_block.cpp


clean:
	-rm -rf $(TARGET) $(criti_OBJECTS)
