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

#ifndef SVC_MAIN_H
#define SVC_MAIN_H

/* -----------------------------------------
 * The MAIN definitions for SERVICE
 * ----------------------------------------- */

#include "stx_cb.h"
#include "httplib.h"
#include "stx_queue.h"
#include "svc_modem.h"

/*
 * Start C++ Declaration
 */
#ifdef __cplusplus
extern "C" {
#endif

#define CMD_ITM_LEN   128  /* queue item length        */
#define CMD_QUE_LEN   4    /* must - power of two      */
#define CMD_BUF_LEN   512  /* must - ITM_LEN * QUE_LEN */

typedef enum e_main_state {
	MAIN_S_UNKNOWN = 0, /* State : UNKNOWN */
	MAIN_S_CAN = 1, /* State : CAN check */
	MAIN_S_MODEM = 2, /* State : Modem check */
	MAIN_S_RUN = 3 /* State : Normal Run */
} main_state_e;

typedef struct s_main_t {
	__IO main_state_e state;

	int64_t lastTick;

	stx_cb_args rom_fn; /* the function of run-on-main */
	void *rom_args;     /* the argument of run-on-main */

	stx_q_t q;
	char_t qbuf[CMD_BUF_LEN];
	stx_qd_t qd[CMD_QUE_LEN];

	stx_qd_t *aqd;

	hl_t *hl;
	hl_response_t hres;

	__IO int32_t ev_type;
	__IO int32_t car_state;
	__IO int32_t try_count;

	char_t sdbuf[1024];

	modem_req_t mreq;
} main_t;


void svc_main_init(void);
void svc_main_car_on(void);
void svc_main_car_off(void);
void svc_main_process(void);
void svc_main_push_cmd(void *cmd, int32_t cmdlen);
void svc_main_run_on_main(const stx_cb_args fn, void *args);
void svc_main_destroy(void);

/*@exposed@*/
main_t *svc_main_get(void);

/*
 * End C++ Declaration
 */
#ifdef __cplusplus
}
#endif

#endif /* SVC_MAIN_H */
