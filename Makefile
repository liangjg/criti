TARGET = criti

CC_SLAVE = sw5cc -slave
CC_HOST = sw5cc -host
CXX_HOST = sw5CC -host
LINK = sw5CC -hybrid

C_FLAGS = -Wall -Wextra -pedantic -Wno-pointer-arith -O0

C_DEFINES = -DGIT_SHA1=\"0b242604d21dcc9a2f4425d97533b7249a4a27ab\" -DUNIX

C_INCLUDES =

CXX_FLAGS = -Wall -Wextra -pedantic -Wno-pointer-arith -O0

CXX_DEFINES = -DGIT_SHA1=\"0b242604d21dcc9a2f4425d97533b7249a4a27ab\" -DUNIX

CXX_INCLUDES =

C_SRC = $(shell ls src/*.c)
CPP_SRC = $(shell ls src/*.cpp)
BASE = $(basename $(C_SRC) $(CPP_SRC))
OBJS = $(addsuffix .o, $(BASE))

$(TARGET): $(OBJS)
	$(LINK) $(OBJS) -o $(TARGET)

src/RNG.o: src/RNG.h
src/RNG.o: src/RNG.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/RNG.o -c src/RNG.c

src/acedata.o: src/acedata.h
src/acedata.o: src/common.h
src/acedata.o: src/nuclide.h
src/acedata.o: src/acedata.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o acedata.o -c src/acedata.c

src/build_neighbor_list.o: src/cell.h
src/build_neighbor_list.o: src/common.h
src/build_neighbor_list.o: src/geometry.h
src/build_neighbor_list.o: src/map.h
src/build_neighbor_list.o: src/particle_state.h
src/build_neighbor_list.o: src/surface.h
src/build_neighbor_list.o: src/universe.h
src/build_neighbor_list.o: src/vector.h
src/build_neighbor_list.o: src/build_neighbor_list.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/build_neighbor_list.o -c src/build_neighbor_list.c

src/calc_col_nuc_cs.o: src/acedata.h
src/calc_col_nuc_cs.o: src/common.h
src/calc_col_nuc_cs.o: src/global_fun.h
src/calc_col_nuc_cs.o: src/map.h
src/calc_col_nuc_cs.o: src/material.h
src/calc_col_nuc_cs.o: src/neutron_transport.h
src/calc_col_nuc_cs.o: src/nuclide.h
src/calc_col_nuc_cs.o: src/particle_state.h
src/calc_col_nuc_cs.o: src/vector.h
src/calc_col_nuc_cs.o: src/calc_col_nuc_cs.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/calc_col_nuc_cs.o -c src/calc_col_nuc_cs.c

src/calc_criticality.o: src/IO_releated.h
src/calc_criticality.o: src/RNG.h
src/calc_criticality.o: src/common.h
src/calc_criticality.o: src/criticality.h
src/calc_criticality.o: src/map.h
src/calc_criticality.o: src/particle_state.h
src/calc_criticality.o: src/universe.h
src/calc_criticality.o: src/vector.h
src/calc_criticality.o: src/calc_criticality.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/calc_criticality.o -c src/calc_criticality.c

src/calc_dist_to_bound.o: src/cell.h
src/calc_dist_to_bound.o: src/common.h
src/calc_dist_to_bound.o: src/geometry.h
src/calc_dist_to_bound.o: src/map.h
src/calc_dist_to_bound.o: src/particle_state.h
src/calc_dist_to_bound.o: src/surface.h
src/calc_dist_to_bound.o: src/universe.h
src/calc_dist_to_bound.o: src/vector.h
src/calc_dist_to_bound.o: src/calc_dist_to_bound.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/calc_dist_to_bound.o -c src/calc_dist_to_bound.c

src/cell.o: src/cell.h
src/cell.o: src/common.h
src/cell.o: src/map.h
src/cell.o: src/surface.h
src/cell.o: src/vector.h
src/cell.o: src/cell.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/cell.o -c src/cell.c

src/check_IO_file.o: src/IO_releated.h
src/check_IO_file.o: src/common.h
src/check_IO_file.o: src/map.h
src/check_IO_file.o: src/universe.h
src/check_IO_file.o: src/check_IO_file.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/check_IO_file.o -c src/check_IO_file.c

src/check_ce_ace_block.o: src/acedata.h
src/check_ce_ace_block.o: src/common.h
src/check_ce_ace_block.o: src/map.h
src/check_ce_ace_block.o: src/material.h
src/check_ce_ace_block.o: src/nuclide.h
src/check_ce_ace_block.o: src/check_ce_ace_block.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/check_ce_ace_block.o -c src/check_ce_ace_block.c

src/convert_mat_nuc_den.o: src/common.h
src/convert_mat_nuc_den.o: src/map.h
src/convert_mat_nuc_den.o: src/material.h
src/convert_mat_nuc_den.o: src/nuclide.h
src/convert_mat_nuc_den.o: src/convert_mat_nuc_den.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/convert_mat_nuc_den.o -c src/convert_mat_nuc_den.c

src/doppler_broaden.o: src/IO_releated.h
src/doppler_broaden.o: src/acedata.h
src/doppler_broaden.o: src/cell.h
src/doppler_broaden.o: src/common.h
src/doppler_broaden.o: src/map.h
src/doppler_broaden.o: src/material.h
src/doppler_broaden.o: src/nuclide.h
src/doppler_broaden.o: src/universe.h
src/doppler_broaden.o: src/doppler_broaden.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/doppler_broaden.o -c src/doppler_broaden.c

src/find_neighbor_cell.o: src/cell.h
src/find_neighbor_cell.o: src/common.h
src/find_neighbor_cell.o: src/geometry.h
src/find_neighbor_cell.o: src/map.h
src/find_neighbor_cell.o: src/particle_state.h
src/find_neighbor_cell.o: src/surface.h
src/find_neighbor_cell.o: src/universe.h
src/find_neighbor_cell.o: src/vector.h
src/find_neighbor_cell.o: src/find_neighbor_cell.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/find_neighbor_cell.o -c src/find_neighbor_cell.c

src/find_next_cell.o: src/cell.h
src/find_next_cell.o: src/common.h
src/find_next_cell.o: src/geometry.h
src/find_next_cell.o: src/map.h
src/find_next_cell.o: src/particle_state.h
src/find_next_cell.o: src/surface.h
src/find_next_cell.o: src/universe.h
src/find_next_cell.o: src/vector.h
src/find_next_cell.o: src/find_next_cell.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/find_next_cell.o -c src/find_next_cell.c

src/geometry_tracking.o: src/cell.h
src/geometry_tracking.o: src/common.h
src/geometry_tracking.o: src/criticality.h
src/geometry_tracking.o: src/geometry.h
src/geometry_tracking.o: src/map.h
src/geometry_tracking.o: src/neutron_transport.h
src/geometry_tracking.o: src/particle_state.h
src/geometry_tracking.o: src/surface.h
src/geometry_tracking.o: src/universe.h
src/geometry_tracking.o: src/vector.h
src/geometry_tracking.o: src/geometry_tracking.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/geometry_tracking.o -c src/geometry_tracking.c

src/get_ce_exist_erg_mu.o: src/RNG.h
src/get_ce_exist_erg_mu.o: src/acedata.h
src/get_ce_exist_erg_mu.o: src/common.h
src/get_ce_exist_erg_mu.o: src/nuclide.h
src/get_ce_exist_erg_mu.o: src/get_ce_exist_erg_mu.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/get_ce_exist_erg_mu.o -c src/get_ce_exist_erg_mu.c

src/get_ce_exit_state.o: src/acedata.h
src/get_ce_exit_state.o: src/common.h
src/get_ce_exit_state.o: src/map.h
src/get_ce_exit_state.o: src/material.h
src/get_ce_exit_state.o: src/neutron_transport.h
src/get_ce_exit_state.o: src/nuclide.h
src/get_ce_exit_state.o: src/particle_state.h
src/get_ce_exit_state.o: src/vector.h
src/get_ce_exit_state.o: src/get_ce_exit_state.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/get_ce_exit_state.o -c src/get_ce_exit_state.c

src/get_erg_func_value.o: src/acedata.h
src/get_erg_func_value.o: src/common.h
src/get_erg_func_value.o: src/global_fun.h
src/get_erg_func_value.o: src/nuclide.h
src/get_erg_func_value.o: src/get_erg_func_value.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/get_erg_func_value.o -c src/get_erg_func_value.c

src/get_exit_state.o: src/RNG.h
src/get_exit_state.o: src/acedata.h
src/get_exit_state.o: src/common.h
src/get_exit_state.o: src/map.h
src/get_exit_state.o: src/material.h
src/get_exit_state.o: src/neutron_transport.h
src/get_exit_state.o: src/nuclide.h
src/get_exit_state.o: src/particle_state.h
src/get_exit_state.o: src/vector.h
src/get_exit_state.o: src/get_exit_state.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/get_exit_state.o -c src/get_exit_state.c

src/get_fis_neu_state.o: src/RNG.h
src/get_fis_neu_state.o: src/acedata.h
src/get_fis_neu_state.o: src/common.h
src/get_fis_neu_state.o: src/criticality.h
src/get_fis_neu_state.o: src/map.h
src/get_fis_neu_state.o: src/material.h
src/get_fis_neu_state.o: src/neutron_transport.h
src/get_fis_neu_state.o: src/nuclide.h
src/get_fis_neu_state.o: src/particle_state.h
src/get_fis_neu_state.o: src/vector.h
src/get_fis_neu_state.o: src/get_fis_neu_state.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/get_fis_neu_state.o -c src/get_fis_neu_state.c

src/get_law_type.o: src/RNG.h
src/get_law_type.o: src/acedata.h
src/get_law_type.o: src/common.h
src/get_law_type.o: src/nuclide.h
src/get_law_type.o: src/get_law_type.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/get_law_type.o -c src/get_law_type.c

src/get_nuc_abs_scatt_cs.o: src/acedata.h
src/get_nuc_abs_scatt_cs.o: src/common.h
src/get_nuc_abs_scatt_cs.o: src/global_fun.h
src/get_nuc_abs_scatt_cs.o: src/nuclide.h
src/get_nuc_abs_scatt_cs.o: src/get_nuc_abs_scatt_cs.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/get_nuc_abs_scatt_cs.o -c src/get_nuc_abs_scatt_cs.c

src/get_nuc_mt_cs.o: src/acedata.h
src/get_nuc_mt_cs.o: src/common.h
src/get_nuc_mt_cs.o: src/global_fun.h
src/get_nuc_mt_cs.o: src/nuclide.h
src/get_nuc_mt_cs.o: src/get_nuc_mt_cs.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/get_nuc_mt_cs.o -c src/get_nuc_mt_cs.c

src/get_nuc_tot_fis_cs.o: src/acedata.h
src/get_nuc_tot_fis_cs.o: src/common.h
src/get_nuc_tot_fis_cs.o: src/global_fun.h
src/get_nuc_tot_fis_cs.o: src/nuclide.h
src/get_nuc_tot_fis_cs.o: src/get_nuc_tot_fis_cs.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/get_nuc_tot_fis_cs.o -c src/get_nuc_tot_fis_cs.c

src/get_scatt_cosine.o: src/RNG.h
src/get_scatt_cosine.o: src/acedata.h
src/get_scatt_cosine.o: src/common.h
src/get_scatt_cosine.o: src/nuclide.h
src/get_scatt_cosine.o: src/get_scatt_cosine.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/get_scatt_cosine.o -c src/get_scatt_cosine.c

src/get_tot_nu.o: src/acedata.h
src/get_tot_nu.o: src/common.h
src/get_tot_nu.o: src/nuclide.h
src/get_tot_nu.o: src/get_tot_nu.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/get_tot_nu.o -c src/get_tot_nu.c

src/global_fun.o: src/common.h
src/global_fun.o: src/global_fun.h
src/global_fun.o: src/global_fun.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/global_fun.o -c src/global_fun.c

src/init_fission_source.o: src/RNG.h
src/init_fission_source.o: src/common.h
src/init_fission_source.o: src/criticality.h
src/init_fission_source.o: src/particle_state.h
src/init_fission_source.o: src/sample_method.h
src/init_fission_source.o: src/vector.h
src/init_fission_source.o: src/init_fission_source.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/init_fission_source.o src/init_fission_source.c

src/interpolate_sab.o: src/acedata.h
src/interpolate_sab.o: src/common.h
src/interpolate_sab.o: src/global_fun.h
src/interpolate_sab.o: src/nuclide.h
src/interpolate_sab.o: src/interpolate_sab.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/interpolate_sab.o -c src/interpolate_sab.c

src/interpolate_xss_table.o: src/acedata.h
src/interpolate_xss_table.o: src/common.h
src/interpolate_xss_table.o: src/global_fun.h
src/interpolate_xss_table.o: src/nuclide.h
src/interpolate_xss_table.o: src/interpolate_xss_table.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/interpolate_xss_table.o -c src/interpolate_xss_table.c

src/locate_particle.o: src/cell.h
src/locate_particle.o: src/common.h
src/locate_particle.o: src/geometry.h
src/locate_particle.o: src/map.h
src/locate_particle.o: src/particle_state.h
src/locate_particle.o: src/surface.h
src/locate_particle.o: src/universe.h
src/locate_particle.o: src/vector.h
src/locate_particle.o: src/locate_particle.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/locate_particle.o -c src/locate_particle.c

src/main.o: src/IO_releated.h
src/main.o: src/RNG.h
src/main.o: src/acedata.h
src/main.o: src/calculation.h
src/main.o: src/cell.h
src/main.o: src/common.h
src/main.o: src/criticality.h
src/main.o: src/geometry.h
src/main.o: src/map.h
src/main.o: src/material.h
src/main.o: src/nuclide.h
src/main.o: src/particle_state.h
src/main.o: src/surface.h
src/main.o: src/universe.h
src/main.o: src/vector.h
src/main.o: src/main.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/main.o -c sr/main.c

src/map.o: src/map.h
src/map.o: src/map.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/map.o -c src/map.c

src/material.o: src/common.h
src/material.o: src/material.h
src/material.o: src/material.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/material.o -c src/material.c

src/nuclide.o: src/common.h
src/nuclide.o: src/nuclide.h
src/nuclide.o: src/nuclide.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/nuclide.o -c src/nuclide.c

src/output_ending.o: src/IO_releated.h
src/output_ending.o: src/common.h
src/output_ending.o: src/map.h
src/output_ending.o: src/universe.h
src/output_ending.o: src/output_ending.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/output_ending.o -c src/output_ending.c

src/output_heading.o: src/IO_releated.h
src/output_heading.o: src/common.h
src/output_heading.o: src/map.h
src/output_heading.o: src/universe.h
src/output_heading.o: src/output_heading.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/output_heading.o -c src/output_heading.c

src/output_mat_file.o: src/IO_releated.h
src/output_mat_file.o: src/common.h
src/output_mat_file.o: src/map.h
src/output_mat_file.o: src/material.h
src/output_mat_file.o: src/nuclide.h
src/output_mat_file.o: src/universe.h
src/output_mat_file.o: src/output_mat_file.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/output_mat_file.o -c src/output_mat_file.c

src/output_summary.o: src/IO_releated.h
src/output_summary.o: src/common.h
src/output_summary.o: src/criticality.h
src/output_summary.o: src/map.h
src/output_summary.o: src/particle_state.h
src/output_summary.o: src/universe.h
src/output_summary.o: src/vector.h
src/output_summary.o: src/output_summary.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/output_summary.o -c src/output_summary.c

src/process_cycle_end.o: src/IO_releated.h
src/process_cycle_end.o: src/common.h
src/process_cycle_end.o: src/criticality.h
src/process_cycle_end.o: src/map.h
src/process_cycle_end.o: src/particle_state.h
src/process_cycle_end.o: src/universe.h
src/process_cycle_end.o: src/vector.h
src/process_cycle_end.o: src/process_cycle_end.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/process_cycle_end.o -c src/process_cycle_end.c

src/react_by_law.o: src/RNG.h
src/react_by_law.o: src/acedata.h
src/react_by_law.o: src/common.h
src/react_by_law.o: src/nuclide.h
src/react_by_law.o: src/sample_method.h
src/react_by_law.o: src/react_by_law.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/react_by_law.o -c src/react_by_law.c

src/read_ace_data.o: src/IO_releated.h
src/read_ace_data.o: src/common.h
src/read_ace_data.o: src/map.h
src/read_ace_data.o: src/nuclide.h
src/read_ace_data.o: src/universe.h
src/read_ace_data.o: src/read_ace_data.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/read_ace_data.o -c src/read_ace_data.c

src/read_criticality_block.o: src/IO_releated.h
src/read_criticality_block.o: src/RNG.h
src/read_criticality_block.o: src/common.h
src/read_criticality_block.o: src/criticality.h
src/read_criticality_block.o: src/map.h
src/read_criticality_block.o: src/particle_state.h
src/read_criticality_block.o: src/universe.h
src/read_criticality_block.o: src/vector.h
src/read_criticality_block.o: src/read_criticality_block.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/read_criticality_block.o -c src/read_criticality_block.c

src/read_input_blocks.o: src/IO_releated.h
src/read_input_blocks.o: src/common.h
src/read_input_blocks.o: src/map.h
src/read_input_blocks.o: src/universe.h
src/read_input_blocks.o: src/read_input_blocks.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/read_input_blocks.o -c src/read_input_blocks.c

src/read_surf_block.o: src/IO_releated.h
src/read_surf_block.o: src/common.h
src/read_surf_block.o: src/map.h
src/read_surf_block.o: src/surface.h
src/read_surf_block.o: src/universe.h
src/read_surf_block.o: src/read_surf_block.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/read_surf_block.o -c src/read_surf_block.c

src/read_universe_block.o: src/IO_releated.h
src/read_universe_block.o: src/common.h
src/read_universe_block.o: src/map.h
src/read_universe_block.o: src/universe.h
src/read_universe_block.o: src/read_universe_block.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/read_universe_block.o -c src/read_universe_block.c

src/release_resource.o: src/IO_releated.h
src/release_resource.o: src/common.h
src/release_resource.o: src/criticality.h
src/release_resource.o: src/map.h
src/release_resource.o: src/particle_state.h
src/release_resource.o: src/universe.h
src/release_resource.o: src/vector.h
src/release_resource.o: src/release_resource.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/release_resource.o -c src/release_resource.c

src/rotate_dir.o: src/RNG.h
src/rotate_dir.o: src/common.h
src/rotate_dir.o: src/neutron_transport.h
src/rotate_dir.o: src/particle_state.h
src/rotate_dir.o: src/vector.h
src/rotate_dir.o: src/rotate_dir.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/rotate_dir.o -c src/rotate_dir.c

src/sample_col_nuclide.o: src/RNG.h
src/sample_col_nuclide.o: src/common.h
src/sample_col_nuclide.o: src/map.h
src/sample_col_nuclide.o: src/material.h
src/sample_col_nuclide.o: src/neutron_transport.h
src/sample_col_nuclide.o: src/nuclide.h
src/sample_col_nuclide.o: src/particle_state.h
src/sample_col_nuclide.o: src/vector.h
src/sample_col_nuclide.o: src/sample_col_nuclide.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/sample_col_nuclide.o -c src/sample_col_nuclide.c

src/sample_col_type.o: src/RNG.h
src/sample_col_type.o: src/acedata.h
src/sample_col_type.o: src/common.h
src/sample_col_type.o: src/map.h
src/sample_col_type.o: src/material.h
src/sample_col_type.o: src/neutron_transport.h
src/sample_col_type.o: src/nuclide.h
src/sample_col_type.o: src/particle_state.h
src/sample_col_type.o: src/vector.h
src/sample_col_type.o: src/sample_col_type.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/sample_col_type.o -c src/sample_col_type.c

src/sample_fission_source.o: src/cell.h
src/sample_fission_source.o: src/common.h
src/sample_fission_source.o: src/criticality.h
src/sample_fission_source.o: src/geometry.h
src/sample_fission_source.o: src/map.h
src/sample_fission_source.o: src/particle_state.h
src/sample_fission_source.o: src/surface.h
src/sample_fission_source.o: src/universe.h
src/sample_fission_source.o: src/vector.h
src/sample_fission_source.o: src/sample_fission_source.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/sample_fission_source.o -c src/sample_fission_source.c

src/sample_free_fly_dis.o: src/RNG.h
src/sample_free_fly_dis.o: src/acedata.h
src/sample_free_fly_dis.o: src/common.h
src/sample_free_fly_dis.o: src/map.h
src/sample_free_fly_dis.o: src/material.h
src/sample_free_fly_dis.o: src/neutron_transport.h
src/sample_free_fly_dis.o: src/nuclide.h
src/sample_free_fly_dis.o: src/particle_state.h
src/sample_free_fly_dis.o: src/vector.h
src/sample_free_fly_dis.o: src/sample_free_fly_dis.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/sample_free_fly_dis.o -c src/sample_free_fly_dis.c

src/sample_method.o: src/RNG.h
src/sample_method.o: src/common.h
src/sample_method.o: src/sample_method.h
src/sample_method.o: src/sample_method.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/sample_method.o -c src/sample_method.c

src/surface.o: src/common.h
src/surface.o: src/surface.h
src/surface.o: src/surface.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/surface.o -c src/surface.c

src/track_history.o: src/common.h
src/track_history.o: src/criticality.h
src/track_history.o: src/neutron_transport.h
src/track_history.o: src/particle_state.h
src/track_history.o: src/vector.h
src/track_history.o: src/track_history.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/track_history.o -c src/track_history.c

src/transform_cm_to_lab.o: src/acedata.h
src/transform_cm_to_lab.o: src/common.h
src/transform_cm_to_lab.o: src/nuclide.h
src/transform_cm_to_lab.o: src/transform_cm_to_lab.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/transform_cm_to_lab.o -c src/transform_cm_to_lab.c

src/treat_URR.o: src/RNG.h
src/treat_URR.o: src/acedata.h
src/treat_URR.o: src/common.h
src/treat_URR.o: src/global_fun.h
src/treat_URR.o: src/nuclide.h
src/treat_URR.o: src/treat_URR.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/treat_URR.o -c src/treat_URR.c

src/treat_fission.o: src/acedata.h
src/treat_fission.o: src/common.h
src/treat_fission.o: src/criticality.h
src/treat_fission.o: src/map.h
src/treat_fission.o: src/material.h
src/treat_fission.o: src/neutron_transport.h
src/treat_fission.o: src/nuclide.h
src/treat_fission.o: src/particle_state.h
src/treat_fission.o: src/vector.h
src/treat_fission.o: src/treat_fission.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/treat_fission.o -c src/treat_fission.c

src/treat_free_gas_model.o: src/RNG.h
src/treat_free_gas_model.o: src/common.h
src/treat_free_gas_model.o: src/neutron_transport.h
src/treat_free_gas_model.o: src/particle_state.h
src/treat_free_gas_model.o: src/vector.h
src/treat_free_gas_model.o: src/treat_free_gas_model.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/treat_free_gas_model.o -c src/treat_free_gas_model.c

src/treat_implicit_capure.o: src/RNG.h
src/treat_implicit_capure.o: src/common.h
src/treat_implicit_capure.o: src/map.h
src/treat_implicit_capure.o: src/material.h
src/treat_implicit_capure.o: src/neutron_transport.h
src/treat_implicit_capure.o: src/nuclide.h
src/treat_implicit_capure.o: src/particle_state.h
src/treat_implicit_capure.o: src/vector.h
src/treat_implicit_capure.o: src/treat_implicit_capure.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/treat_implicit_capure.o -c src/treat_implicit_capure.c

src/treat_sab_collision_type.o: src/RNG.h
src/treat_sab_collision_type.o: src/acedata.h
src/treat_sab_collision_type.o: src/common.h
src/treat_sab_collision_type.o: src/global_fun.h
src/treat_sab_collision_type.o: src/neutron_transport.h
src/treat_sab_collision_type.o: src/nuclide.h
src/treat_sab_collision_type.o: src/particle_state.h
src/treat_sab_collision_type.o: src/vector.h
src/treat_sab_collision_type.o: src/treat_sab_collision_type.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/treat_sab_collision_type.o -c src/treat_sab_collision_type.c

src/universe.o: src/common.h
src/universe.o: src/map.h
src/universe.o: src/universe.h
src/universe.o: src/universe.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/universe.o -c src/universe.c

src/vector.o: src/vector.h
src/vector.o: src/vector.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o src/vector.o -c src/vector.c

src/read_cell_card.cpp.o: src/IO_releated.h
src/read_cell_card.cpp.o: src/cell.h
src/read_cell_card.cpp.o: src/common.h
src/read_cell_card.cpp.o: src/map.h
src/read_cell_card.cpp.o: src/universe.h
src/read_cell_card.cpp.o: src/read_cell_card.cpp
	$(CXX_HOST) $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o src/read_cell_card.o -c src/read_cell_card.cpp

src/read_material_block.cpp.o: src/IO_releated.h
src/read_material_block.cpp.o: src/common.h
src/read_material_block.cpp.o: src/map.h
src/read_material_block.cpp.o: src/material.h
src/read_material_block.cpp.o: src/nuclide.h
src/read_material_block.cpp.o: src/universe.h
src/read_material_block.cpp.o: src/read_material_block.cpp
	$(CXX_HOST) $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o src/read_material_block.o -c src/read_material_block.cpp

.PHONY: clean
clean:
	-rm -rf $(TARGET) $(OBJS)
