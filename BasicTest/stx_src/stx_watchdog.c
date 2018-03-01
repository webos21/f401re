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

#include "stx_watchdog.h"

#include "stx_driver.h"

#ifdef S_SPLINT_S
#include "stm32f1xx_hal_iwdg.h"
#endif

/* ----------------------------------------------
 * Internal Structures
 * ---------------------------------------------- */

extern IWDG_HandleTypeDef hiwdg;

static __IO int32_t g_wd_init = 0L;

/* ----------------------------------------------
 * Internal Functions
 * ---------------------------------------------- */

/* ----------------------------------------------
 * Interface Implementations
 * ---------------------------------------------- */

void stx_watchdog_init(void) {
	if (g_wd_init == 0L) {
		g_wd_init = 1L;
	}
}

void stx_watchdog_ok(void) {
	if (g_wd_init > 0L) {
		(void) HAL_IWDG_Refresh(&hiwdg);
	}
}
