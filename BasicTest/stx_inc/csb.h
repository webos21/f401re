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

#ifndef CSB_H
#define CSB_H

/* -----------------------------------------
 * The C-Style String Buffer API
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
 * for using the variable arguments
 *
 * Pm064 : (MISRA C 2004 rule 16.1) functions with variable number of arguments shall not be used
 */
#pragma diag_suppress=Pm064
#endif

typedef struct s_csb_t csb_t;

typedef struct s_csb_info_t {
	uint32_t size;
	uint32_t wlen;
	char_t *cptr;  /** Current Position Pointer */
} csb_info_t;

csb_t  *csb_init(/*@returned@*/void *mem, uint32_t mem_size);
int32_t /*@alt void@*/ csb_get_info(csb_t *handle, csb_info_t *out);
int32_t /*@alt void@*/ csb_append(csb_t *handle, const char_t *str, uint32_t slen);
int32_t csb_sprintf(csb_t *handle, const char_t *format, ...);
char_t *csb_tostring(csb_t *handle, /*@out@*/int32_t *out);
int32_t csb_reset(csb_t *handle);
void    csb_destroy(csb_t *handle);

/*
 * End C++ Declaration
 */
#ifdef __cplusplus
}
#endif

#endif /* CSB_H */
