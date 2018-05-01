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
C_FLAGS := -O1 -OPT:IEEE_arith=1

# Set the C defines
C_DEFINES := -DUNIX -DCODE_VERSION=\"SW-0.1.5\"

# Set the C header files search path
C_INCLUDES := -I/usr/sw-mpp/mpi2/include

# Set the CXX compiler flags
CXX_FLAGS := -O1 -OPT:IEEE_arith=1

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

# Objects file path
MPE_C_OBJ_PATH := build/objs/mpe/c
MPE_CXX_OBJ_PATH := build/objs/mpe/cxx
CPE_C_OBJ_PATH := build/objs/cpe

# Source file path
SRC_PATH := src

# Object files for target criti
MPE_C_OBJECTS = \
$(MPE_C_OBJ_PATH)/RNG.o \
$(MPE_C_OBJ_PATH)/build_neighbor_list.o \
$(MPE_C_OBJ_PATH)/calc_col_nuc_cs.o \
$(MPE_C_OBJ_PATH)/calc_dist_to_bound.o \
$(MPE_C_OBJ_PATH)/calc_dist_to_lat.o \
$(MPE_C_OBJ_PATH)/calc_dist_to_surf.o \
$(MPE_C_OBJ_PATH)/calc_erg_mu.o \
$(MPE_C_OBJ_PATH)/calc_surf_sense.o \
$(MPE_C_OBJ_PATH)/calc_therm_Gfun.o \
$(MPE_C_OBJ_PATH)/cell.o \
$(MPE_C_OBJ_PATH)/check_ce_ace_block.o \
$(MPE_C_OBJ_PATH)/convert_mat_nuc_den.o \
$(MPE_C_OBJ_PATH)/doppler_broaden.o \
$(MPE_C_OBJ_PATH)/find_lat_index.o \
$(MPE_C_OBJ_PATH)/find_neighbor_cell.o \
$(MPE_C_OBJ_PATH)/find_next_cell.o \
$(MPE_C_OBJ_PATH)/get_ce_exit_state.o \
$(MPE_C_OBJ_PATH)/get_delayed_nu.o \
$(MPE_C_OBJ_PATH)/get_erg_func_value.o \
$(MPE_C_OBJ_PATH)/get_law_type.o \
$(MPE_C_OBJ_PATH)/get_nuc_abs_scatt_cs.o \
$(MPE_C_OBJ_PATH)/get_nuc_mt_cs.o \
$(MPE_C_OBJ_PATH)/get_nuc_tot_fis_cs.o \
$(MPE_C_OBJ_PATH)/get_scatt_cosine.o \
$(MPE_C_OBJ_PATH)/get_surf_norm_vec.o \
$(MPE_C_OBJ_PATH)/get_tot_nu.o \
$(MPE_C_OBJ_PATH)/global_fun.o \
$(MPE_C_OBJ_PATH)/interpolate_sab.o \
$(MPE_C_OBJ_PATH)/interpolate_xss_table.o \
$(MPE_C_OBJ_PATH)/locate_particle.o \
$(MPE_C_OBJ_PATH)/load_balance.o \
$(MPE_C_OBJ_PATH)/main.o \
$(MPE_C_OBJ_PATH)/map.o \
$(MPE_C_OBJ_PATH)/material.o \
$(MPE_C_OBJ_PATH)/move_to_origin_lat.o \
$(MPE_C_OBJ_PATH)/nuclide.o \
$(MPE_C_OBJ_PATH)/offset_neighbor_lat.o \
$(MPE_C_OBJ_PATH)/output_ending.o \
$(MPE_C_OBJ_PATH)/output_heading.o \
$(MPE_C_OBJ_PATH)/output_mat_file.o \
$(MPE_C_OBJ_PATH)/particle_is_in_cell.o \
$(MPE_C_OBJ_PATH)/preprocess_geometry.o \
$(MPE_C_OBJ_PATH)/process_cycle_end.o \
$(MPE_C_OBJ_PATH)/react_by_law.o \
$(MPE_C_OBJ_PATH)/read_ace_data.o \
$(MPE_C_OBJ_PATH)/read_criticality_block.o \
$(MPE_C_OBJ_PATH)/read_fixed_src_block.o \
$(MPE_C_OBJ_PATH)/read_input_blocks.o \
$(MPE_C_OBJ_PATH)/read_surf_block.o \
$(MPE_C_OBJ_PATH)/read_universe_block.o \
$(MPE_C_OBJ_PATH)/reflect_par.o \
$(MPE_C_OBJ_PATH)/release_resource.o \
$(MPE_C_OBJ_PATH)/rotate_dir.o \
$(MPE_C_OBJ_PATH)/sample_col_nuclide.o \
$(MPE_C_OBJ_PATH)/sample_free_fly_dis.o \
$(MPE_C_OBJ_PATH)/sample_maxwell.o \
$(MPE_C_OBJ_PATH)/sample_watt.o \
$(MPE_C_OBJ_PATH)/set_RNG_paras.o \
$(MPE_C_OBJ_PATH)/surface.o \
$(MPE_C_OBJ_PATH)/trans_univ_coord.o \
$(MPE_C_OBJ_PATH)/trans_univ_dir.o \
$(MPE_C_OBJ_PATH)/treat_URR.o \
$(MPE_C_OBJ_PATH)/treat_fission.o \
$(MPE_C_OBJ_PATH)/treat_free_gas_model.o \
$(MPE_C_OBJ_PATH)/treat_sab_collision_type.o \
$(MPE_C_OBJ_PATH)/universe.o \
$(MPE_C_OBJ_PATH)/vector.o

MPE_CXX_OBJECTS = \
$(MPE_CXX_OBJ_PATH)/read_cell_card.o \
$(MPE_CXX_OBJ_PATH)/read_material_block.o

CPE_OBJECTS = \
$(CPE_C_OBJ_PATH)/RNG.o \
$(CPE_C_OBJ_PATH)/calc_col_nuc_cs.o \
$(CPE_C_OBJ_PATH)/calc_dist_to_bound.o \
$(CPE_C_OBJ_PATH)/calc_dist_to_lat.o \
$(CPE_C_OBJ_PATH)/calc_dist_to_surf.o \
$(CPE_C_OBJ_PATH)/calc_erg_mu.o \
$(CPE_C_OBJ_PATH)/calc_surf_sense.o \
$(CPE_C_OBJ_PATH)/do_calc.o \
$(CPE_C_OBJ_PATH)/find_lat_index.o \
$(CPE_C_OBJ_PATH)/find_neighbor_cell.o \
$(CPE_C_OBJ_PATH)/find_next_cell.o \
$(CPE_C_OBJ_PATH)/geometry_tracking.o \
$(CPE_C_OBJ_PATH)/get_ce_exit_state.o \
$(CPE_C_OBJ_PATH)/get_delayed_nu.o \
$(CPE_C_OBJ_PATH)/get_erg_func_value.o \
$(CPE_C_OBJ_PATH)/get_exit_state.o \
$(CPE_C_OBJ_PATH)/get_fis_neu_state.o \
$(CPE_C_OBJ_PATH)/get_law_type.o \
$(CPE_C_OBJ_PATH)/get_nuc_abs_scatt_cs.o \
$(CPE_C_OBJ_PATH)/get_nuc_mt_cs.o \
$(CPE_C_OBJ_PATH)/get_nuc_tot_fis_cs.o \
$(CPE_C_OBJ_PATH)/get_scatt_cosine.o \
$(CPE_C_OBJ_PATH)/get_surf_norm_vec.o \
$(CPE_C_OBJ_PATH)/get_tot_nu.o \
$(CPE_C_OBJ_PATH)/global_fun.o \
$(CPE_C_OBJ_PATH)/interpolate_sab.o \
$(CPE_C_OBJ_PATH)/interpolate_xss_table.o \
$(CPE_C_OBJ_PATH)/locate_particle.o \
$(CPE_C_OBJ_PATH)/move_to_origin_lat.o \
$(CPE_C_OBJ_PATH)/offset_neighbor_lat.o \
$(CPE_C_OBJ_PATH)/particle_is_in_cell.o \
$(CPE_C_OBJ_PATH)/react_by_law.o \
$(CPE_C_OBJ_PATH)/reflect_par.o \
$(CPE_C_OBJ_PATH)/rotate_dir.o \
$(CPE_C_OBJ_PATH)/sample_col_nuclide.o \
$(CPE_C_OBJ_PATH)/sample_col_type.o \
$(CPE_C_OBJ_PATH)/sample_free_fly_dis.o \
$(CPE_C_OBJ_PATH)/sample_maxwell.o \
$(CPE_C_OBJ_PATH)/sample_watt.o \
$(CPE_C_OBJ_PATH)/trans_univ_coord.o \
$(CPE_C_OBJ_PATH)/trans_univ_dir.o \
$(CPE_C_OBJ_PATH)/treat_URR.o \
$(CPE_C_OBJ_PATH)/treat_fission.o \
$(CPE_C_OBJ_PATH)/treat_free_gas_model.o \
$(CPE_C_OBJ_PATH)/treat_implicit_capture.o \
$(CPE_C_OBJ_PATH)/treat_sab_collision_type.o


# KSRC objects
KSRC_OBJECTS = \
$(MPE_C_OBJ_PATH)/calc_criticality.o \
$(MPE_C_OBJ_PATH)/geometry_tracking.o \
$(MPE_C_OBJ_PATH)/get_exit_state.o \
$(MPE_C_OBJ_PATH)/get_fis_neu_state.o \
$(MPE_C_OBJ_PATH)/init_fission_source.o \
$(MPE_C_OBJ_PATH)/output_summary.o \
$(MPE_C_OBJ_PATH)/sample_col_type.o \
$(MPE_C_OBJ_PATH)/treat_implicit_capture.o


# FSRC objects
# FSRC_OBJECTS = \
# $(MPE_C_OBJ_PATH)/calc_fixed_src.o \
# $(MPE_C_OBJ_PATH)/geometry_tracking_fixed.o \
# $(MPE_C_OBJ_PATH)/get_exit_state_fixed.o \
# $(MPE_C_OBJ_PATH)/get_fis_neu_state_fixed.o \
# $(MPE_C_OBJ_PATH)/init_external_src.o \
# $(MPE_C_OBJ_PATH)/output_summary_fixed.o \
# $(MPE_C_OBJ_PATH)/sample_col_type_fixed.o \
# $(MPE_C_OBJ_PATH)/treat_implicit_capture_fixed.o


# External object files for target criti
criti_EXTERNAL_OBJECTS =


.PHONY: all clean ksrc fsrc


# Default build
all: ksrc

ksrc: C_DEFINES += -DKSRC
ksrc: CXX_DEFINES += -DKSRC
ksrc: $(KSRC_TARGET)

$(KSRC_TARGET): $(MPE_C_OBJECTS) $(MPE_CXX_OBJECTS) $(KSRC_OBJECTS) $(CPE_OBJECTS)
	$(LINKER) $^ -lm_slave -o $@


$(MPE_C_OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	$(CC_HOST) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o $@ -c $<


$(MPE_CXX_OBJ_PATH)/%.o: $(SRC_PATH)/%.cpp
	$(CXX_HOST) $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o $@ -c $<


$(CPE_C_OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	$(CC_SLAVE) $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o $@ -c $<


# fsrc: C_DEFINES += -DFSRC
# fsrc: CXX_DEFINES += -DFSRC
# fsrc: $(FSRC_TARGET)

# $(FSRC_TARGET): $(MPE_C_OBJECTS) $(MPE_CXX_OBJECTS) $(FSRC_OBJECTS) $(CPE_OBJECTS)
# 	$(LINKER) $^ -lm_slave -o $@


clean:
	-rm -rf build/objs/mpe/c/* build/objs/mpe/cxx/* build/objs/cpe/*
