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

#ifndef STX_CLOCK_H
#define STX_CLOCK_H

/* -----------------------------------------
 * The Clock functions for STM32
 * ----------------------------------------- */

#include "misra.h"

/*
 * Start C++ Declaration
 */
#ifdef __cplusplus
extern "C" {
#endif

typedef struct s_stx_ts_t {
	int16_t year;
	int8_t mon;
	int8_t day;
	int8_t hour;
	int8_t min;
	int8_t sec;
	int16_t msec;
	int8_t wday;
	/*@observer@*/
	const char_t * tz;
} stx_ts_t;

void      stx_clock_init(void);

int32_t   stx_clock_sec(void);
int64_t   stx_clock_msec(void);
int64_t   stx_clock_tick(void);
int32_t   stx_clock_tick_sec(void);
int64_t   stx_clock_ts2msec(stx_ts_t *ts);
int32_t   stx_clock_ts2sec(stx_ts_t *ts);
int32_t   stx_clock_msec2ts(int64_t msec, stx_ts_t *ts);
int32_t   /*@alt void@*/ stx_clock_sec2ts(int32_t sec, stx_ts_t *ts);
int32_t   /*@alt void@*/ stx_clock_ts(stx_ts_t *ts);
int32_t   /*@alt void@*/ stx_clock_set(stx_ts_t *ts);
void      stx_clock_ts_print(const stx_ts_t *ts);

/* STM32 Interrupt Handler */
void      HAL_SYSTICK_Callback(void);

/*
 * End C++ Declaration
 */
#ifdef __cplusplus
}
#endif

#endif /* STX_CLOCK_H */
