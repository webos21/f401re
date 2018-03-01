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
#               BuildX : STM32F4-stx_src             #
#----------------------------------------------------#
# File    : buildx.mk                                #
# Version : 1.0.0                                    #
# Desc    : For building the STM32F4-stx_src.        #
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
module_build_src_mk    = $(wildcard *.c)
module_build_src_ex    = 
module_build_cflags    = \
	-I$(basedir)/Drivers/CMSIS/Include \
	-I$(basedir)/Drivers/CMSIS/Device/ST/STM32F4xx/Include \
	-I$(basedir)/Drivers/STM32F4xx_HAL_Driver/Inc \
	-I$(basedir)/stx_inc \
	-I$(basedir)/Inc
ifeq ($(TARGET),stm32f4)
module_build_src_mk   += GCC/startup_stm32f401xe.S
endif

# PREPARE : Set VPATH!!
vpath
vpath %.c $(current_dir_abs)

# PREPARE : Build Targets
ifeq ($(build_run_a),1)
module_objs_a_tmpS     = $(patsubst %.S,%.o,$(module_build_src_mk))
module_objs_static     = $(patsubst %.c,%.o,$(module_objs_a_tmpS))
module_link_a_tmp1     = $(notdir $(module_objs_static))
module_link_static     = $(addprefix $(module_dir_object)/,$(module_link_a_tmp1))
endif
ifeq ($(build_run_so),1)
module_objs_so_tmp0    = $(patsubst %.S,%.lo,$(module_build_src_mk))
module_objs_shared     = $(patsubst %.c,%.lo,$(module_objs_so_tmp0))
module_link_so_tmp1    = $(notdir $(module_objs_shared))
module_link_shared     = $(addprefix $(module_dir_object)/,$(module_link_so_tmp1))
endif


###################
# build-targets
###################

all: prepare build_only post

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
	@echo "                         STM32F4-CodeSrc"
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
	@echo "module_build_src_mk     : $(module_build_src_mk)"	
	@echo "module_build_cflags     : $(module_build_cflags)"	
	@echo "================================================================"

prepare: prepare_mkdir_base prepare_mkdir_output prepare_result

build_only: $(module_link_static) $(module_link_shared)

post:
	@echo "================================================================"
	@echo "OUTPUT : $(current_dir_abs)"
	@echo "----------------------------------------------------------------"
	@echo $(module_link_static) $(module_link_shared)
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
