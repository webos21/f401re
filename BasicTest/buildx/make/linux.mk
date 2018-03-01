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
#                        Linux                       #
#----------------------------------------------------#
# File    : linux.mk                                 #
# Version : 1.0.0                                    #
# Desc    : MK file for LINUX build.                 #
#----------------------------------------------------#
# History)                                           #
#   - 2016/02/01 : Created by cmjo                   #
######################################################


########################
# Build Configuration
########################
build_cfg_target  = linux
build_cfg_linux   = 1
build_cfg_posix   = 1


########################
# Build Toolchain
########################
build_tool_dir    = 
build_tool_prefix = 


########################
# Program Definition
########################
build_tool_cc     = gcc
build_tool_cxx    = g++
build_tool_linker = g++
build_tool_ar     = ar
build_tool_ranlib = ranlib


########################
# Compile Flags
########################
build_run_a       = 1
build_run_so      =

build_opt_a_pre   = lib
build_opt_a_ext   = a
build_opt_so_pre  = lib
build_opt_so_ext  = so
build_opt_exe_ext =

build_opt_c       = -g -Wall -Wextra -Wdeclaration-after-statement -O3 -D_REENTRANT -D_THREAD_SAFE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64
build_opt_cxx     = -g -Wall -Wextra -O3 -D_REENTRANT -D_THREAD_SAFE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64
build_opt_ld      = -g -Wl,--no-undefined
build_opt_ld_noud = -Wl,--no-undefined

build_opt_fPIC    = -fPIC
build_opt_mnocyg  = 
build_opt_libgcc  =
build_opt_libgxx  = 

build_opt_inc     = $(basedir)/include


########################
# Build Flags
########################
build_ext_run_stm32f2xx      = 1
