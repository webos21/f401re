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
#             BuildX - CMSIS for STM32F4xx           #
#----------------------------------------------------#
# File    : buildx.mk                                #
# Version : 1.0.0                                    #
# Desc    : For building the CMSIS for STM32F4xx.    #
#----------------------------------------------------#
# History)                                           #
#   - 2017/08/31 : Created by cmjo                   #
######################################################

# PREPARE : set base
basedir = ../..
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
module_dir_target      = $(basedir)/$(destdir)/$(build_cfg_target)/Drivers/$(current_dir_rel)
module_dir_object      = $(module_dir_target)/object
# Output
module_dir_output_base = $(basedir)/$(destdir)/$(build_cfg_target)/emul
module_dir_output_bin  = $(module_dir_output_base)/bin
module_dir_output_inc  = $(module_dir_output_base)/include/CMSIS
module_dir_output_lib  = $(module_dir_output_base)/lib
module_dir_output_res  = $(module_dir_output_base)/res
module_dir_output_test = $(module_dir_output_base)/test


###################
# build-targets
###################

all: prepare post

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
	@echo "                     CMSIS for STM32F4xx"
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
	@echo "================================================================"

prepare: prepare_mkdir_base prepare_mkdir_output prepare_result

post:
	@echo "================================================================"
	@echo "OUTPUT : $(current_dir_abs)"
	@echo "----------------------------------------------------------------"
	$(CP) $(current_dir_abs)/Include/*.h $(module_dir_output_inc)
	$(CP) $(current_dir_abs)/Device/ST/STM32F4xx/Include/*.h $(module_dir_output_inc)
	@echo "================================================================"


clean: prepare
	$(RM) -rf "$(module_dir_target)"


###################
# build-rules
###################

$(module_dir_object)/%.o: %.c
	$(build_tool_cc) $(build_opt_c) $(module_build_cflags) -c -o $@ $<

$(module_dir_object)/%.lo: %.c
	$(build_tool_cc) $(build_opt_c) $(build_opt_fPIC) $(module_build_cflags) -c -o $@ $<

