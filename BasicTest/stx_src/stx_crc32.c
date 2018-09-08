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

#include "stx_crc32.h"

#include "stx_driver.h"

#ifdef S_SPLINT_S
#include "stm32f401xe.h"
#include "stm32f4xx_hal_crc.h"
#endif


/* ----------------------------------------------
 * Internal Structures
 * ---------------------------------------------- */

#define CRC32_POLYNOMIAL   ((uint32_t) 0xEDB88320)

static uint8_t g_remain[4] = { 0, 0, 0, 0 };
static int32_t g_rlen = 0;

extern CRC_HandleTypeDef hcrc;

/* ----------------------------------------------
 * Interface Implementations
 * ---------------------------------------------- */

void stx_crc32_open(void) {
	g_remain[0] = 0;
	g_remain[1] = 0;
	g_remain[2] = 0;
	g_remain[3] = 0;
	g_rlen = 0;

	__HAL_CRC_DR_RESET(&hcrc);
}

void stx_crc32_calc(void *data, uint32_t len) {
	uint32_t* pBuffer = data;
	uint32_t BufferLength = len / 4;
	uint32_t index = 0;
	uint8_t *buf;

	buf = data;
	for (index = 0; index < BufferLength; index++) {
		CRC->DR = __RBIT(pBuffer[index]);
	}

	buf += (BufferLength * 4);
	index = len - (BufferLength * 4);
	while (index--) {
		g_remain[g_rlen++] = *buf++;
	}
}

uint32_t stx_crc32_close(void) {
	uint32_t ret;
	int32_t i, j;

	ret = __RBIT(CRC->DR);
	for (i = 0; i < g_rlen; i++) {
		ret ^= (uint32_t) g_remain[i];
		for (j = 0; j < 8; j++) {
			if ((ret & 0x1U) == 0x1U) {
				ret = (ret >> 1) ^ CRC32_POLYNOMIAL;
			} else {
				ret >>= 1;
			}
		}
	}

	return (ret ^ 0xFFFFFFFF);
}
