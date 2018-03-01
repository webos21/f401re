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

#ifndef SVC_CMD_PROC_H
#define SVC_CMD_PROC_H

/* -----------------------------------------
 * The COMMAND PROCEDURE for SERVICE
 * ----------------------------------------- */

#include "svc_cmd_parser.h"

/*
 * Start C++ Declaration
 */
#ifdef __cplusplus
extern "C" {
#endif

void svc_cmd_proc_help(void);
void svc_cmd_proc_show_summary(void);

void svc_cmd_proc(char_t *cmd, int32_t cmdlen);

/*
 * End C++ Declaration
 */
#ifdef __cplusplus
}
#endif

#endif /* SVC_CMD_PROC_H */
