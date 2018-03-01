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

#ifndef SVC_CONSOLE_H
#define SVC_CONSOLE_H

/* -----------------------------------------
 * The CONSOLE for SERVICE
 * ----------------------------------------- */

#include "misra.h"

/*
 * Start C++ Declaration
 */
#ifdef __cplusplus
extern "C" {
#endif

void    svc_console_init(void);
void    svc_console_start(void);
void    svc_console_stop(void);
int32_t /*@alt void@*/ svc_console_write(void *dptr, int32_t len);
int32_t /*@alt void@*/ svc_console_puts(char_t *str);
void    svc_console_process(void);

/*
 * End C++ Declaration
 */
#ifdef __cplusplus
}
#endif

#endif /* SVC_CONSOLE_H */
