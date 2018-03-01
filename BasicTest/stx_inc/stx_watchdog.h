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

#ifndef STX_WATCHDOG_H
#define STX_WATCHDOG_H

/* -----------------------------------------
 * The WATCHDOG functions for STM32
 * ----------------------------------------- */

/*
 * Start C++ Declaration
 */
#ifdef __cplusplus
extern "C" {
#endif

void stx_watchdog_init(void);
void stx_watchdog_ok(void);

/*
 * End C++ Declaration
 */
#ifdef __cplusplus
}
#endif

#endif /* STX_WATCHDOG_H */
