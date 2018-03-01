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

/* ----------------------------------------------
 * Headers
 * ---------------------------------------------- */

#include "stx_cpu.h"

#include "stx_driver.h"

/* ----------------------------------------------
 * Internal Structures
 * ---------------------------------------------- */

#define CPU_STM32F40x

/* ----------------------------------------------
 * Interface Implementations
 * ---------------------------------------------- */

void stx_cpu_id(uint32_t cpuId[/*3*/]) {
	cpuId[0] = 0;
	cpuId[1] = 0;
	cpuId[2] = 0;

#if defined(CPU_STM32F207) || defined(CPU_STM32F40x) || defined(CPU_STM32F0xx)
	cpuId[0] = *(unsigned int*) (0x1fff7a10);
	cpuId[1] = *(unsigned int*) (0x1fff7a14);
	cpuId[2] = *(unsigned int*) (0x1fff7a18);
#endif

#if (defined(STM32F105xC) || defined(STM32F10X_CL))
	cpuId[0] = *(unsigned int*) (0x1ffff7e8);
	cpuId[1] = *(unsigned int*) (0x1ffff7ec);
	cpuId[2] = *(unsigned int*) (0x1ffff7f0);
#endif
}

uint16_t stx_cpu_flash_size(void) {
#if defined(CPU_STM32F207) || defined(CPU_STM32F40x) || defined(CPU_STM32F0xx)
	return (*(uint16_t *) (0X1fff7a22));
#endif

#if (defined(STM32F105xC) || defined(STM32F10X_CL))
	return (*(uint16_t *) (0x1ffff7e0));
#endif
}

void stx_cpu_set_vector_tb(uint32_t offset) {
#if defined(STM32F105xC)
	/* see the "system_stm32f1xx.c" file */
	if (offset != 0U) {
		/* Nothing to do */
	}
#endif
#if defined(STM32F10X_CL) || defined(CPU_STM32F207)
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, offset);
#endif
}

uint32_t stx_cpu_enable_interrupt(void) {
	__IO uint32_t primask;

	primask = __get_PRIMASK();
	__enable_irq();

	return (primask);
}

uint32_t stx_cpu_disable_interrupt(void) {
	__IO uint32_t primask;

	primask = __get_PRIMASK();
	__disable_irq();

	return (primask);
}

void stx_cpu_restore_interrupt(uint32_t primask) {
	if (primask) {
		__disable_irq();
	} else {
		__enable_irq();
	}
}

void stx_cpu_reboot(void) {
	NVIC_SystemReset();
}
