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

#include "stx_timer11.h"

#include "stx_driver.h"

#ifdef S_SPLINT_S
#include "stm32f4xx_hal_dma.h"
#include "stm32f4xx_hal_tim.h"
#include "stm32f4xx_hal_cortex.h"
#endif

/* ----------------------------------------------
 * Internal Structures
 * ---------------------------------------------- */

extern TIM_HandleTypeDef htim11;

/*@null@*/static __IO stx_cb_args g_cb = NULL;
/*@null@*/static void *g_dat = NULL;

/* ----------------------------------------------
 * Internal Functions
 * ---------------------------------------------- */

/* Forward Declaration : Begin */
static void stx_timer11_nvic_set(int32_t enabled);
void HAL_TIM11_Callback(TIM_HandleTypeDef *htim);
/* Forward Declaration : End */

static void stx_timer11_nvic_set(int32_t enabled) {
#if 0
	HAL_NVIC_SetPriority(TIM1_TRG_COM_TIM11_IRQn, 2, 0);
#endif
	if (enabled) {
		HAL_NVIC_EnableIRQ(TIM1_TRG_COM_TIM11_IRQn);
	} else {
		HAL_NVIC_DisableIRQ(TIM1_TRG_COM_TIM11_IRQn);
	}
}

/* ----------------------------------------------
 * Interface Implementations
 * ---------------------------------------------- */

void stx_timer11_init(void) {
	g_cb = NULL;
	g_dat = NULL;

	stx_timer11_stop();
}

void stx_timer11_start(stx_cb_args fn, void *args) {
	g_cb = fn;
	g_dat = args;

	stx_timer11_nvic_set(1);
	(void) HAL_TIM_Base_Start_IT(&htim11);
}

void stx_timer11_stop(void) {
	g_cb = NULL;
	g_dat = NULL;

	HAL_TIM_Base_Stop_IT(&htim11);
	stx_timer11_nvic_set(0);
}

/* ------------------------------------------------
 * The interrupt handler of STM32F2xx
 * ------------------------------------------------ */

void HAL_TIM11_Callback(TIM_HandleTypeDef *htim) {
	if (g_cb != NULL) {
		g_cb(g_dat);
	}
	(void) HAL_TIM_Base_Start_IT(htim);
}
