/*
 * Copyright 2016 Cheolmin Jo (webos21@gmail.com)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef STX_CPU_H
#define STX_CPU_H

/* -----------------------------------------
 * The CPU functions for STM32
 * ----------------------------------------- */

#include "misra.h"

/*
 * Start C++ Declaration
 */
#ifdef __cplusplus
extern "C" {
#endif

void     stx_cpu_id(uint32_t cpuId[/*3*/]);
uint16_t stx_cpu_flash_size(void);
void     stx_cpu_set_vector_tb(uint32_t offset);

uint32_t stx_cpu_enable_interrupt(void);
uint32_t stx_cpu_disable_interrupt(void);
void     stx_cpu_restore_interrupt(uint32_t primask);

void     stx_cpu_reboot(void);

/*
 * End C++ Declaration
 */
#ifdef __cplusplus
}
#endif

#endif /* STX_CPU_H */
