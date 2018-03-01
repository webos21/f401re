# Copyright 2016 Cheolmin Jo (webos21@gmail.com)
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

######################################################
#               BuildX : STM32F4-Main                #
#----------------------------------------------------#
# File    : buildx.mk                                #
# Version : 1.0.0                                    #
# Desc    : For building the STM32F4-Main.           #
#----------------------------------------------------#
# History)                                           #
#   - 2017/10/14 : Created by cmjo                   #
######################################################

# PREPARE : set base
basedir = ..
destdir = out

# PREPARE : shell commands
include $(basedir)/buildx/make/cmd.mk

# PREPARE : Check Environment
ifeq ($(TARGET),)
need_warning = "Warning : you are here without proper command!!!!"
include $(basedir)/buildx/make/project.mk
include $(basedir)/buildx/make/$(project_def_target).mk
TARGET = $(project_def_target)
else
need_warning = ""
include $(basedir)/buildx/make/project.mk
include $(basedir)/buildx/make/$(TARGET).mk
endif

# PREPARE : Directories
# Base
current_dir_abs        = $(CURDIR)
current_dir_rel        = $(notdir $(current_dir_abs))
# Base
module_dir_target      = $(basedir)/$(destdir)/$(build_cfg_target)/$(current_dir_rel)
module_dir_object      = $(module_dir_target)/object
# Output
module_dir_output_base = $(basedir)/$(destdir)/$(build_cfg_target)/emul
module_dir_output_bin  = $(module_dir_output_base)/bin
module_dir_output_inc  = $(module_dir_output_base)/include
module_dir_output_lib  = $(module_dir_output_base)/lib
module_dir_output_res  = $(module_dir_output_base)/res
module_dir_output_test = $(module_dir_output_base)/test

# PREPARE : Build Options
module_build_src_bin   = main.c
module_build_src_mk    = $(wildcard *.c)
module_build_src_ex    = 
module_build_cflags    = \
	-I$(basedir)/Drivers/CMSIS/Include \
	-I$(basedir)/Drivers/CMSIS/Device/ST/STM32F4xx/Include \
	-I$(basedir)/Drivers/STM32F4xx_HAL_Driver/Inc \
	-I$(basedir)/stx_inc \
	-I$(basedir)/Inc
module_build_ldflags   = \
	-L$(module_dir_output_lib) \
	-lstm32f4drv \
	-lgcc \
	-lc_nano
module_build_target_a  = $(build_opt_a_pre)stm32f4usr.$(build_opt_a_ext)
module_build_target_so = $(build_opt_so_pre)stm32f4usr.$(build_opt_so_ext)
module_build_target_ex = BasicTest.elf
ifeq ($(TARGET),stm32f4)
module_build_target_tx = BasicTest.bin
module_build_target_tr = BasicTest.hex
endif

# PREPARE : Set VPATH!!
vpath
vpath %.c $(current_dir_abs)

# PREPARE : Build Targets
ifeq ($(build_run_a),1)
module_prebuilt_o      = $(wildcard $(basedir)/$(destdir)/$(build_cfg_target)/stx_src/object/*.o)

module_objs_a_tmp0     = $(patsubst $(module_build_src_bin),,$(module_build_src_mk))
module_objs_static     = $(patsubst %.c,%.o,$(module_objs_a_tmp0))
module_link_a_tmp1     = $(notdir $(module_objs_static))
module_link_static     = $(addprefix $(module_dir_object)/,$(module_link_a_tmp1))
module_target_static   = $(module_build_target_a)
endif
ifeq ($(build_run_so),1)
module_prebuilt_so     = $(wildcard $(basedir)/$(destdir)/$(build_cfg_target)/stx_src/object/*.lo)

module_objs_so_tmp0    = $(patsubst $(module_build_src_bin),,$(module_build_src_mk))
module_objs_shared     = $(patsubst %.c,%.lo,$(module_objs_so_tmp0))
module_link_so_tmp1    = $(notdir $(module_objs_shared))
module_link_shared     = $(addprefix $(module_dir_object)/,$(module_link_so_tmp1))
module_target_shared   = $(module_build_target_so)
endif
ifeq (1,1)
module_objs_exe_tmp0   = 
module_objs_exe        = $(patsubst %.c,%.o,$(module_build_src_bin))
module_link_exe_tmp1   = $(notdir $(module_objs_exe))
module_link_exe        = $(addprefix $(module_dir_object)/,$(module_link_exe_tmp1))
module_target_exe      = $(module_build_target_ex)
endif


###################
# build-targets
###################

ifeq ($(TARGET),stm32f4)
all: prepare $(module_target_static) $(module_target_shared) $(module_target_exe) $(module_build_target_tr) post
else
all: prepare $(module_target_static) $(module_target_shared) $(module_target_exe) post
endif

prepare_mkdir_base:
	@$(MKDIR) -p "$(module_dir_target)"
	@$(MKDIR) -p "$(module_dir_object)"

prepare_mkdir_output:
	@$(MKDIR) -p "$(module_dir_output_base)"
	@$(MKDIR) -p "$(module_dir_output_bin)"
	@$(MKDIR) -p "$(module_dir_output_inc)"
	@$(MKDIR) -p "$(module_dir_output_lib)"
	@$(MKDIR) -p "$(module_dir_output_res)"
	@$(MKDIR) -p "$(module_dir_output_test)"

prepare_result:
	@echo "$(need_warning)"
	@echo "================================================================"
	@echo "                          STM32F1-Main"
	@echo "================================================================"
	@echo "TARGET                  : $(TARGET)"
	@echo "----------------------------------------------------------------"
	@echo "current_dir_abs         : $(current_dir_abs)"
	@echo "current_dir_rel         : $(current_dir_rel)"
	@echo "----------------------------------------------------------------"
	@echo "module_dir_target       : $(module_dir_target)"	
	@echo "module_dir_object       : $(module_dir_object)"	
	@echo "module_dir_test         : $(module_dir_test)"	
	@echo "----------------------------------------------------------------"
	@echo "module_dir_output_base  : $(module_dir_output_base)"	
	@echo "module_dir_output_bin   : $(module_dir_output_bin)"	
	@echo "module_dir_output_inc   : $(module_dir_output_inc)"	
	@echo "module_dir_output_lib   : $(module_dir_output_lib)"	
	@echo "module_dir_output_res   : $(module_dir_output_res)"	
	@echo "module_dir_output_test  : $(module_dir_output_test)"	
	@echo "----------------------------------------------------------------"
	@echo "module_build_src_bin    : $(module_build_src_bin)"	
	@echo "module_build_src_mk     : $(module_build_src_mk)"	
	@echo "module_build_src_ex     : $(module_build_src_ex)"	
	@echo "module_build_cflags     : $(module_build_cflags)"	
	@echo "module_build_ldflags    : $(module_build_ldflags)"	
	@echo "module_build_target_a   : $(module_build_target_a)"	
	@echo "module_build_target_so  : $(module_build_target_so)"	
	@echo "module_build_target_ex  : $(module_build_target_ex)"	
	@echo "module_build_target_tr  : $(module_build_target_tr)"	
	@echo "================================================================"

prepare: prepare_mkdir_base prepare_mkdir_output prepare_result


$(module_build_target_a): $(module_link_static)
	@echo "================================================================"
	@echo "BUILD : $(module_build_target_a)"
	@echo "----------------------------------------------------------------"
	$(build_tool_ar) rcu $(module_dir_target)/$(module_build_target_a) $(module_link_static) $(module_prebuilt_o)
	$(build_tool_ranlib) $(module_dir_target)/$(module_build_target_a)
	@echo "================================================================"


$(module_build_target_so): $(module_link_shared)
	@echo "================================================================"
	@echo "BUILD : $(module_build_target_so)"
	@echo "----------------------------------------------------------------"
	$(build_tool_linker) \
		$(build_opt_ld) \
		$(build_opt_ld_so)$(module_build_target_so) \
		-o $(module_dir_target)/$(module_build_target_so) \
		$(module_link_shared) \
		$(module_prebuilt_so) \
		$(module_build_ldflags) \
		$(build_opt_ld_mgwcc)
	@echo "================================================================"


$(module_build_target_ex): $(module_link_exe) $(module_link_static)
	@echo "================================================================"
	@echo "BUILD : $(module_build_target_ex)"
	@echo "----------------------------------------------------------------"
	$(build_tool_linker) \
		$(build_opt_ld) \
		-Wl,-Map=$(module_dir_target)/$(module_build_target_ex).map \
		-o $(module_dir_target)/$(module_build_target_ex) \
		$(module_link_exe) $(module_link_static) $(module_prebuilt_o) \
		$(module_build_ldflags) \
		$(build_opt_ld_mgwcc)
	@echo "================================================================"

ifeq ($(TARGET),stm32f4)
$(module_build_target_tr): $(module_build_target_ex)
	@echo "================================================================"
	@echo "OBJCOPY : $(module_build_target_tr)"
	@echo "----------------------------------------------------------------"
	$(build_tool_objcopy) \
		-O ihex \
		$(module_dir_target)/$(module_build_target_ex) \
		$(module_dir_target)/$(module_build_target_tr)
	$(build_tool_objcopy) \
		-O binary \
		$(module_dir_target)/$(module_build_target_ex) \
		$(module_dir_target)/$(module_build_target_tx)
	$(build_tool_size) \
		$(module_dir_target)/$(module_build_target_ex)
	@echo "================================================================"
endif


post:
	@echo "================================================================"
	@echo "OUTPUT : $(current_dir_abs)"
	@echo "----------------------------------------------------------------"
	$(TEST_FILE) $(module_dir_target)/$(module_build_target_a) $(TEST_THEN) \
		$(CP) $(module_dir_target)/$(module_build_target_a) $(module_dir_output_lib) \
	$(TEST_END)
	$(TEST_FILE) $(module_dir_target)/$(module_build_target_ex) $(TEST_THEN) \
		$(CP) $(module_dir_target)/$(module_build_target_ex) $(module_dir_output_bin) \
	$(TEST_END)
	$(TEST_FILE) $(module_dir_target)/$(module_build_target_tr) $(TEST_THEN) \
		$(CP) $(module_dir_target)/$(module_build_target_tr) $(module_dir_output_bin) \
	$(TEST_END)
	$(TEST_FILE) $(module_dir_target)/$(module_build_target_tx) $(TEST_THEN) \
		$(CP) $(module_dir_target)/$(module_build_target_tx) $(module_dir_output_bin) \
	$(TEST_END)
	@echo "================================================================"


clean: prepare
	$(RM) -rf "$(module_dir_target)"


###################
# build-rules
###################

$(module_dir_object)/%.o: GCC/%.S
	$(build_tool_cc) $(build_opt_c) $(module_build_cflags) -c -o $@ $<

$(module_dir_object)/%.o: %.c
	$(build_tool_cc) $(build_opt_c) $(module_build_cflags) -c -o $@ $<

$(module_dir_object)/%.lo: %.c
	$(build_tool_cc) $(build_opt_c) $(build_opt_fPIC) $(module_build_cflags) -c -o $@ $<
