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
#                      STM32F4                       #
#----------------------------------------------------#
# File    : Makefile                                 #
# Version : 1.0.0                                    #
# Desc    : STM32F4 makefile.                        #
#----------------------------------------------------#
# History)                                           #
#   - 2016/08/11 : Created by cmjo                   #
######################################################

# directories
basedir = .
destdir = out

# shell commands
include $(basedir)/buildx/make/cmd.mk

# make targets
all: usage

linux:
	@$(MKDIR) -p "$(destdir)/$@"
	$(MAKE) -C Drivers -f buildx.mk TARGET=$@
	$(MAKE) -C stx_src -f buildx.mk TARGET=$@
	$(MAKE) -C Src -f buildx.mk TARGET=$@

stm32f4_app:
	@$(MKDIR) -p "$(destdir)/stm32f4"
	$(MAKE) -C Drivers -f buildx.mk TARGET=stm32f4  SVC_HOST=0
	$(MAKE) -C stx_src -f buildx.mk TARGET=stm32f4  SVC_HOST=0
	$(MAKE) -C Src -f buildx.mk TARGET=stm32f4  SVC_HOST=0

stm32f4_host:
	@$(MKDIR) -p "$(destdir)/stm32f4"
	$(MAKE) -C Drivers -f buildx.mk TARGET=stm32f4  SVC_HOST=1
	$(MAKE) -C stx_src -f buildx.mk TARGET=stm32f4  SVC_HOST=1
	$(MAKE) -C Src -f buildx.mk TARGET=stm32f4  SVC_HOST=1

clean:
	@$(RM) -rf $(basedir)/out/*

usage:
	@echo "########################################################"
	@echo "#                   STM32F1 Makefile                   #"
	@echo "#------------------------------------------------------#"
	@echo "# We only support below command.                       #"
	@echo "#                                                      #"
	@echo "#   - make linux        : make the linux binary        #"
	@echo "#   - make stm32f4_app  : make the stm32f4 APP binary  #"
	@echo "#   - make stm32f4_host : make the stm32f4 HOST binary #"
	@echo "#   - make clean        : clean the source tree        #"
	@echo "########################################################"
