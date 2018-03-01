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
#                  BuildX - Drivers                  #
#----------------------------------------------------#
# File    : buildx.mk                                #
# Version : 1.0.0                                    #
# Desc    : For building the external modules.       #
#----------------------------------------------------#
# History)                                           #
#   - 2016/03/21 : Created by cmjo                   #
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

# PREPARE : get current directory
bx_external_abs   = $(CURDIR)
bx_external_rel   = $(notdir $(bx_external_abs))

# PREPARE : set target directory
bx_external_target = $(basedir)/$(destdir)/$(build_cfg_target)/$(bx_external_rel)


###################
# make targets
###################

all: prepare do_build

prepare:
	@echo $(need_warning)
	@$(MKDIR) -p "$(bx_external_target)"
	@echo "================================================================"
	@echo "                       Drivers Build"
	@echo "================================================================"
	@echo "TARGET                  : $(TARGET)"
	@echo "----------------------------------------------------------------"
	@echo "bx_external_abs         : $(bx_external_abs)"
	@echo "bx_external_rel         : $(bx_external_rel)"
	@echo "----------------------------------------------------------------"
	@echo "bx_external_target      : $(bx_external_target)"	
	@echo "================================================================"


do_build:
	$(TEST_VAR) "$(build_ext_run_cmsis)" $(TEST_EQ) "1" $(TEST_THEN) \
		$(MAKE) -C CMSIS -f buildx.mk TARGET=$(TARGET) \
	$(TEST_END)
	$(TEST_VAR) "$(build_ext_run_stm32f1xx)" $(TEST_EQ) "1" $(TEST_THEN) \
		$(MAKE) -C STM32F4xx_HAL_Driver -f buildx.mk TARGET=$(TARGET) \
	$(TEST_END)

clean: prepare
	$(RM) -rf "$(bx_external_target)"
