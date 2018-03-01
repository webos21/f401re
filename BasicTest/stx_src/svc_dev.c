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

#include "svc_dev.h"
#include "svc_base.h"

#include "stx_sys.h"
#include "stx_clock.h"
#include "stx_watchdog.h"
#include "stx_cpu.h"

#include "stx_util.h"

/* ----------------------------------------------
 * Internal Structures
 * ---------------------------------------------- */

/* ----------------------------------------------
 * Internal Functions
 * ---------------------------------------------- */

/* ----------------------------------------------
 * Interface Implementations
 * ---------------------------------------------- */

void svc_dev_vt_set(void) {
	stx_cpu_set_vector_tb(SVC_VECTOR_OFFSET);
}

void svc_dev_init(void) {
	stx_sys_init();
	stx_clock_init();
	stx_watchdog_init();
}

void svc_dev_destroy(void) {
	util_print("Device will be rebooted!!!\r\n");
	stx_cpu_reboot();
}
