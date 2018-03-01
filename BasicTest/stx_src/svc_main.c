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

#include "svc_main.h"

#include "svc_console.h"
#include "svc_dev.h"
#include "svc_cfg.h"
#include "svc_http.h"

#include "svc_cmd_proc.h"
#include "svc_modem.h"

#include "stx_cpu.h"
#include "stx_clock.h"
#include "stx_math.h"
#include "stx_queue.h"
#include "stx_watchdog.h"
#include "stx_util.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* ----------------------------------------------
 * Internal Structures
 * ---------------------------------------------- */

#define TX_INTERVAL 9995

typedef int32_t (*svc_main_fn)(main_t *handle);

static int32_t svc_main_unknown(main_t *handle);
static int32_t svc_main_check_can(main_t *handle);
static int32_t svc_main_check_modem(main_t *handle);
static int32_t svc_main_normal_run(main_t *handle);

static svc_main_fn g_main_fn[] = { &svc_main_unknown, &svc_main_check_can,
		&svc_main_check_modem, &svc_main_normal_run };

static main_t g_main;

/* ----------------------------------------------
 * Internal Functions
 * ---------------------------------------------- */

/* Forward Declaration : Begin */
static int32_t /*@alt void@*/svc_main_fn_cmd_process(main_t *handle);
static void svc_main_trans(main_t *handle);
/* Forward Declaration : End */

static int32_t svc_main_fn_cmd_process(main_t *handle) {
	while ((handle->aqd = stx_q_pop(&handle->q)) != NULL) {
		svc_cmd_proc(handle->aqd->data, handle->aqd->dlen);
	}
	return 0;
}

static int32_t svc_main_unknown(main_t *handle) {
	int32_t rc = MAIN_S_CAN;

	handle->ev_type = svc_cfg_car_get();
	handle->car_state = 0;
	handle->try_count = 0;

	svc_cmd_proc_show_summary();
	util_print("F401RE> ");

	return rc;
}

static int32_t svc_main_check_can(main_t *handle) {
	return MAIN_S_MODEM;
}

static int32_t svc_main_check_modem(main_t *handle) {
	return MAIN_S_RUN;
}

static int32_t svc_main_normal_run(main_t *handle) {
	return MAIN_S_RUN;
}

static void svc_main_trans(main_t *handle) {
	handle->state = (main_state_e) g_main_fn[(int32_t) handle->state](handle);
}

/* ----------------------------------------------
 * Interface Implementations
 * ---------------------------------------------- */

void svc_main_init(void) {

	svc_dev_vt_set();

	svc_dev_init();

	memset(&g_main, 0, sizeof(g_main));

	svc_console_init();
	svc_console_start();

	svc_cfg_init();

	svc_modem_init();

	stx_q_init(&g_main.q, g_main.qbuf, g_main.qd, CMD_ITM_LEN, CMD_QUE_LEN, 0);
	g_main.hl = hl_open();

	util_print("\r\n>>> F401RE Starting....\r\n");
}

void svc_main_car_on(void) {
	main_t *handle = &g_main;
	if (handle->car_state == 0) {
		handle->car_state = 1;
		util_log0("CAR is ON  !!!!!!!!!!!\r\n");
		svc_modem_start(1);
	}
}

void svc_main_car_off(void) {
	main_t *handle = &g_main;
	if (handle->car_state == 1) {
		handle->car_state = 0;
		util_log0("CAR is OFF !!!!!!!!!!!\r\n");
		svc_modem_stop();
	}
}

void svc_main_process(void) {
	main_t *handle = &g_main;

	stx_watchdog_ok();

	svc_console_process();
	svc_main_fn_cmd_process(handle);

	svc_main_trans(handle);

	if (handle->rom_fn != NULL) {
		handle->rom_fn(handle->rom_args);
		handle->rom_fn = NULL;
		handle->rom_args = NULL;
	}
}

void svc_main_push_cmd(void *cmd, int32_t cmdlen) {
	if (cmdlen > CMD_ITM_LEN) {
		util_err2(
				"the length of received command is too long : \r\n\t%s (%d)\r\n",
				(char_t * ) cmd, cmdlen);
	} else {
		stx_q_push(&g_main.q, cmd, cmdlen);
	}
}

void svc_main_run_on_main(const stx_cb_args fn, void *args) {
	main_t *handle = &g_main;

	handle->rom_fn = fn;
	handle->rom_args = args;
}

void svc_main_destroy(void) {
	svc_dev_destroy();
}

main_t *svc_main_get(void) {
	return &g_main;
}
