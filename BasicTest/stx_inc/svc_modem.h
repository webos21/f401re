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

#ifndef SVC_MODEM_H
#define SVC_MODEM_H

/* -----------------------------------------
 * The MODEM functions for SERVICE
 * ----------------------------------------- */

#include "stx_cb.h"

/*
 * Start C++ Declaration
 */
#ifdef __cplusplus
extern "C" {
#endif

#define MODEM_TELADIN   0
#define MODEM_TECHPLEX  0
#define MODEM_TELIT     1

#define MODEM_ITM_LEN   256   /* queue item length        */
#define MODEM_RCQ_LEN   16    /* must - power of two      */
#define MODEM_BUF_LEN   4096  /* must - ITM_LEN * QUE_LEN */

#define RECV_MODE_LINE  0
#define RECV_MODE_BUFF  1

typedef enum e_modem_state_e {
	MODEM_S_OFF = 0,        /* MODEM STATE : OFF */
	MODEM_S_BOOTING = 1,    /* MODEM STATE : BOOTING */
	MODEM_S_ATTATCH = 2,    /* MODEM STATE : ATTATCHED */
	MODEM_S_ON = 3,         /* MODEM STATE : ON */
	MODEM_S_CREG = 4,       /* MODEM STATE : NETWORK REGISTRATION */
	MODEM_S_CNUM = 5,       /* MODEM STATE : GETTING PHONE NUMBER */
	MODEM_S_READY = 6,      /* MODEM STATE : READY */
	MODEM_S_PPP_ON = 7,     /* MODEM STATE : PPP Open */
	MODEM_S_SENDING = 8,    /* MODEM STATE : SENDING */
	MODEM_S_RECEVING = 9    /* MODEM STATE : RECEIVING */
} modem_state_e;

typedef int (*modem_filter_fn)(char_t *msg, int16_t msglen);

typedef struct s_modem_req_t {
	__IO int8_t   sync;                /* async = 0, sync = 1 */
	__IO int8_t   timeout;             /* seconds */
	__IO int8_t   done;                /* when it is finished, flag is set to 1 */
	/*@unique@*/
	char_t   cmd[128];            /* AT command */
	__IO int16_t  cmdLen;
	/*@unique@*/
	char_t   filterStr[32];       /* Filter String : if the result start with 'xxxx' */
	__IO int16_t  filterLen;
	modem_filter_fn filterFunc;   /* Filter Function : 0 = false, 1 = true */
	char_t   ret[MODEM_ITM_LEN];  /* Result String */
	__IO int16_t  retLen;
	stx_cb_args cb;               /* Async Call-back */
} modem_req_t;

void svc_modem_init(void);

void svc_modem_start(int8_t fullCheck);
void svc_modem_stop(void);

void svc_modem_status(void);

int32_t  /*@alt void@*/ svc_modem_time_sync(int8_t timeout);

int32_t  /*@alt void@*/ svc_modem_ppp_open(int8_t timeout);
int32_t  /*@alt void@*/ svc_modem_ppp_close(int8_t timeout);

void     svc_modem_mode_set(int8_t mode);
int32_t  svc_modem_mode_get(void);

/*@observer@*/
char_t  *svc_modem_mode_string(int8_t mode);

int32_t  svc_modem_state_get(void);

/*@observer@*/
char_t  *svc_modem_state_string(modem_state_e state);

/*@exposed@*/
char_t  *svc_modem_phone_no(void);

void     svc_modem_clear_q(void);

int32_t  /*@alt void@*/ svc_modem_write(void *dptr, int16_t len);
void     svc_modem_init_req(modem_req_t *req, int8_t sync, int8_t timeout, char_t *cmd,
		char_t *filterStr, const modem_filter_fn filterFunc);
int32_t  /*@alt void@*/ svc_modem_send_req(modem_req_t *req);

int32_t  svc_modem_data_recv(void *dptr, int16_t len);
int32_t  svc_modem_data_send(void *dptr, int16_t len);

void svc_modem_process(void *args);

/*
 * End C++ Declaration
 */
#ifdef __cplusplus
}
#endif

#endif /* SVC_MODEM_H */
