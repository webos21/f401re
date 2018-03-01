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

#include "stx_timer10.h"

#include "stx_driver.h"

#ifdef S_SPLINT_S
#include "stm32f4xx_hal_dma.h"
#include "stm32f4xx_hal_tim.h"
#include "stm32f4xx_hal_cortex.h"
#endif

/* ----------------------------------------------
 * Internal Structures
 * ---------------------------------------------- */

extern TIM_HandleTypeDef htim10;

/*@null@*/static __IO stx_cb_args g_cb = NULL;
/*@null@*/static void *g_dat = NULL;

extern void HAL_TIM11_Callback(TIM_HandleTypeDef *htim);

/* ----------------------------------------------
 * Internal Functions
 * ---------------------------------------------- */

/* Forward Declaration : Begin */
static void stx_timer10_nvic_set(int32_t enabled);
void HAL_TIM10_Callback(TIM_HandleTypeDef *htim);
/* Forward Declaration : End */

static void stx_timer10_nvic_set(int32_t enabled) {
#if 0
	HAL_NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 2, 0);
#endif
	if (enabled) {
		HAL_NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
	} else {
		HAL_NVIC_DisableIRQ(TIM1_UP_TIM10_IRQn);
	}
}

/* ----------------------------------------------
 * Interface Implementations
 * ---------------------------------------------- */

void stx_timer10_init(void) {
	g_cb = NULL;
	g_dat = NULL;

	stx_timer10_stop();
}

void stx_timer10_start(stx_cb_args fn, void *args) {
	g_cb = fn;
	g_dat = args;

	stx_timer10_nvic_set(1);
	(void) HAL_TIM_Base_Start_IT(&htim10);
}

void stx_timer10_stop(void) {
	g_cb = NULL;
	g_dat = NULL;

	HAL_TIM_Base_Stop_IT(&htim10);
	stx_timer10_nvic_set(0);
}

/* ------------------------------------------------
 * The interrupt handler of STM32F2xx
 * ------------------------------------------------ */

void HAL_TIM10_Callback(TIM_HandleTypeDef *htim) {
	if (g_cb != NULL) {
		g_cb(g_dat);
	}
	(void) HAL_TIM_Base_Start_IT(htim);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM10) {
		HAL_TIM10_Callback(htim);
	} else if (htim->Instance == TIM11) {
		HAL_TIM11_Callback(htim);
	} else {
		/* Nothing to do */
	}
}
