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

#ifndef STX_SYS_H
#define STX_SYS_H

/* -----------------------------------------
 * The SYS functions for STM32
 * ----------------------------------------- */

#include "misra.h"

/*
 * Start C++ Declaration
 */
#ifdef __cplusplus
extern "C" {
#endif

void    stx_sys_init(void);

void    stx_obd_led_set(int32_t onOff);
void    stx_data_led_toggle(void);

void    stx_modem_power_set(int32_t onOff);
void    stx_modem_led_set(int32_t onOff);
void    stx_modem_led_toggle(void);
int32_t stx_modem_sim_read(void);

void    stx_gps_power_set(int32_t onOff);

int32_t stx_tact_sw_read(void);


/*
 * End C++ Declaration
 */
#ifdef __cplusplus
}
#endif

#endif /* STX_SYS_H */
