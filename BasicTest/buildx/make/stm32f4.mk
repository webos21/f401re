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
#                        stm32f4                     #
#----------------------------------------------------#
# File    : stm32f4.mk                               #
# Version : 1.0.0                                    #
# Desc    : MK file for STM32F4 build.               #
#----------------------------------------------------#
# History)                                           #
#   - 2017/08/31 : Created by cmjo                   #
######################################################


########################
# Build Configuration
########################
build_cfg_target  = stm32f4
build_cfg_linux   = 1
build_cfg_posix   = 1


########################
# Build Toolchain
########################
build_tool_dir    = 
build_tool_prefix = arm-none-eabi-


########################
# Program Definition
########################
build_tool_cc      = $(build_tool_prefix)gcc
build_tool_cxx     = $(build_tool_prefix)g++
build_tool_linker  = $(build_tool_prefix)gcc
build_tool_ar      = $(build_tool_prefix)ar
build_tool_ranlib  = $(build_tool_prefix)ranlib
build_tool_strip   = $(build_tool_prefix)strip
build_tool_size    = $(build_tool_prefix)size
build_tool_objcopy = $(build_tool_prefix)objcopy


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

ifeq ($(SVC_HOST),1)
build_opt_c       = -g -Os -Wall -Wpedantic -Wstrict-prototypes -mlittle-endian -mthumb -mthumb-interwork -mcpu=cortex-m3 -fstack-usage -ffunction-sections -fdata-sections -fmessage-length=0 -fsingle-precision-constant -Wdouble-promotion -mfloat-abi=soft -ffreestanding -nostdlib -Wextra -Wdeclaration-after-statement -DSTM32F401xE -DUSE_HAL_DRIVER -DSVC_HOST=1
build_opt_cxx     = -g -Os -Wall -Wpedantic -Wstrict-prototypes -mlittle-endian -mthumb -mthumb-interwork -mcpu=cortex-m3 -fstack-usage -ffunction-sections -fdata-sections -fmessage-length=0 -fsingle-precision-constant -Wdouble-promotion -mfloat-abi=soft -ffreestanding -nostdlib -Wextra -Wdeclaration-after-statement -DSTM32F401xE -DUSE_HAL_DRIVER -DSVC_HOST=1
build_opt_ld      = -g -Os -Wall -T$(basedir)/buildx/make/stm32_host.ld -mlittle-endian -mthumb -mthumb-interwork -nostartfiles -nostdlib -mcpu=cortex-m3 -fsingle-precision-constant -Wdouble-promotion -Wl,--gc-sections -mfloat-abi=soft
else
build_opt_c       = -g -Os -Wall -Wpedantic -Wstrict-prototypes -mlittle-endian -mthumb -mthumb-interwork -mcpu=cortex-m3 -fstack-usage -ffunction-sections -fdata-sections -fmessage-length=0 -fsingle-precision-constant -Wdouble-promotion -mfloat-abi=soft -ffreestanding -nostdlib -Wextra -Wdeclaration-after-statement -DSTM32F401xE -DUSE_HAL_DRIVER
build_opt_cxx     = -g -Os -Wall -Wpedantic -Wstrict-prototypes -mlittle-endian -mthumb -mthumb-interwork -mcpu=cortex-m3 -fstack-usage -ffunction-sections -fdata-sections -fmessage-length=0 -fsingle-precision-constant -Wdouble-promotion -mfloat-abi=soft -ffreestanding -nostdlib -Wextra -Wdeclaration-after-statement -DSTM32F401xE -DUSE_HAL_DRIVER
build_opt_ld      = -g -Os -Wall -T$(basedir)/buildx/make/stm32_app.ld -mlittle-endian -mthumb -mthumb-interwork -nostartfiles -nostdlib -mcpu=cortex-m3 -fsingle-precision-constant -Wdouble-promotion -Wl,--gc-sections -mfloat-abi=soft
endif
build_opt_ld_noud = -Wl,--no-undefined

build_opt_fPIC    = -fPIC
build_opt_mnocyg  = 
build_opt_libgcc  =
build_opt_libgxx  = 

build_opt_inc     = $(basedir)/include


########################
# Build Flags
########################
build_ext_run_cmsis          = 1
build_ext_run_stm32f1xx      = 1
