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

#ifndef STX_UTIL_H
#define STX_UTIL_H

/* -----------------------------------------
 * The Utility functions for STM32
 * ----------------------------------------- */

#include "misra.h"

/*
 * Start C++ Declaration
 */
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __ICCARM__
/*
 * for using "PROMPT_LOG", "PROMPT_ERR", "util_print"
 *
 * Pm064 : (MISRA C 2004 rule 16.1) functions with variable number of arguments shall not be used
 * Pm154 : (MISRA C 2004 rule 19.10) function-like macro, each instance of a parameter shall be enclosed
 */
#pragma diag_suppress=Pm064,Pm154
#endif

#ifdef __CSTAT__
#pragma cstat_disable="MISRAC2004-19.10"
#endif
#define PROMPT_LOG(fmt) ("L|%s|%d|" fmt)
#define PROMPT_ERR(fmt) ("\r\nE|%s|%d|" fmt)
#ifdef __CSTAT__
#pragma cstat_enable="MISRAC2004-19.10"
#endif
#define UTIL_FILE       (util_basename(__FILE__))

#define LOG_LINE_MAX    256 /* WARN : "util_print" print the maximum 256 characters */

#define UTIL_LOG_ON

#ifdef UTIL_LOG_ON
#define util_log0(fmt)                   (util_print(PROMPT_LOG(fmt), (UTIL_FILE), (__LINE__)))
#define util_log1(fmt, a1)               (util_print(PROMPT_LOG(fmt), (UTIL_FILE), (__LINE__), (a1)))
#define util_log2(fmt, a1, a2)           (util_print(PROMPT_LOG(fmt), (UTIL_FILE), (__LINE__), (a1), (a2)))
#define util_log3(fmt, a1, a2, a3)       (util_print(PROMPT_LOG(fmt), (UTIL_FILE), (__LINE__), (a1), (a2), (a3)))
#define util_log4(fmt, a1, a2, a3, a4)   (util_print(PROMPT_LOG(fmt), (UTIL_FILE), (__LINE__), (a1), (a2), (a3), (a4)))
#else /* !UTIL_LOG_ON */
#define util_log0(fmt)                   do {}while(0)
#define util_log1(fmt, a1)               do {}while(0)
#define util_log2(fmt, a1, a2)           do {}while(0)
#define util_log3(fmt, a1, a2, a3)       do {}while(0)
#define util_log4(fmt, a1, a2, a3, a4)   do {}while(0)
#endif /* UTIL_LOG_ON */

#define util_err0(fmt)                   (util_print(PROMPT_ERR(fmt), (UTIL_FILE), (__LINE__)))
#define util_err1(fmt, a1)               (util_print(PROMPT_ERR(fmt), (UTIL_FILE), (__LINE__), (a1)))
#define util_err2(fmt, a1, a2)           (util_print(PROMPT_ERR(fmt), (UTIL_FILE), (__LINE__), (a1), (a2)))
#define util_err3(fmt, a1, a2, a3)       (util_print(PROMPT_ERR(fmt), (UTIL_FILE), (__LINE__), (a1), (a2), (a3)))
#define util_err4(fmt, a1, a2, a3, a4)   (util_print(PROMPT_ERR(fmt), (UTIL_FILE), (__LINE__), (a1), (a2), (a3), (a4)))

char_t *util_basename(/*@returned@*/char_t *path);

int32_t /*@alt void@*/util_print(const char_t *fmt, ...);

void util_hexdump(char_t *desc, void *addr, int32_t len);
void util_hexdump2(void *addr, int32_t len, int32_t offset);

void util_msleep(__IO int32_t msec);
void util_usleep(__IO int32_t usec);
void util_nsleep(__IO int32_t n100sec);

/*
 * End C++ Declaration
 */
#ifdef __cplusplus
}
#endif

#endif /* STX_UTIL_H */
