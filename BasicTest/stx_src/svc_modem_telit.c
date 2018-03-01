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

#include "svc_modem.h"

#if MODEM_TELIT

#include "stx_timer10.h"
#include "stx_uart.h"
#include "stx_sys.h"

#include "svc_console.h"

#include "stx_watchdog.h"
#include "stx_clock.h"
#include "stx_queue.h"
#include "stx_util.h"

#include <string.h>
#include <stdlib.h>

/* ----------------------------------------------
 * Internal Structures
 * ---------------------------------------------- */

#define MODEM_DEBUG       (1 != 1)

#if MODEM_DEBUG
#define MODEM_LOG               util_print
#else /* !MODEM_DEBUG */
#define MODEM_LOG(...)          do {}while(0)
#endif /* MODEM_DEBUG */

#define SVC_MODEM_UART  STX_UART_PORT_1


typedef struct s_svc_modem_t {
	__IO modem_state_e state;
	__IO int32_t r_mode;

	char_t pno[16];
	__IO int64_t lastTick;

	__IO int64_t rts;
	__IO int32_t rcnt;
	char_t rbuf[MODEM_ITM_LEN];

	stx_q_t rq;
	char_t rqbuf[MODEM_BUF_LEN];
	stx_qd_t rqd[MODEM_RCQ_LEN];

	modem_req_t *req;

} svc_modem_t;

static svc_modem_t g_modem;

/* ----------------------------------------------
 * Internal Functions
 * ---------------------------------------------- */

/* Forward Declaration : Begin */
static void svc_modem_cb(char_t rb);
/* Forward Declaration : End */

static void svc_modem_cb(char_t rb) {
	svc_modem_t *handle = &g_modem;

	handle->rbuf[handle->rcnt++] = rb;
	handle->rts = stx_clock_tick();

	if (handle->r_mode == RECV_MODE_LINE) {
		int32_t tcnt = handle->rcnt;
		if ((rb == 0x0A) || (tcnt >= (MODEM_ITM_LEN - 1))) {
			if (handle->rcnt < MODEM_ITM_LEN) {
				handle->rbuf[handle->rcnt] = '\0';
			}
			stx_q_push(&handle->rq, handle->rbuf, handle->rcnt);
			handle->rcnt = 0;
		}
	} else {
		if (handle->rcnt >= (MODEM_ITM_LEN - 1)) {
			stx_q_push(&handle->rq, handle->rbuf, handle->rcnt);
			handle->rcnt = 0;
		}
	}
}

/* ----------------------------------------------
 * Interface Implementations
 * ---------------------------------------------- */

void svc_modem_init(void) {
	memset(&g_modem, 0, sizeof(g_modem));

	stx_uart_init(SVC_MODEM_UART);
	stx_timer10_init();
}

void svc_modem_start(int8_t fullCheck) {
	int32_t rc;
	__IO int32_t rt;
	modem_req_t mreq;

	svc_modem_t *handle = &g_modem;

	if (handle->state >= MODEM_S_READY) {
		util_print("Modem is already on!!!\r\n");
	} else {
		handle->state = MODEM_S_OFF;

		memset(&handle->pno, 0, sizeof(handle->pno));
		memset(&handle->rbuf, 0, sizeof(handle->rbuf));
		handle->rcnt = 0;

		handle->r_mode = RECV_MODE_LINE;

		stx_q_init(&handle->rq, handle->rqbuf, handle->rqd, MODEM_ITM_LEN,
		MODEM_RCQ_LEN, 1);

		util_print("Tuning on the modem...\r\n");
		handle->state = MODEM_S_BOOTING;

		stx_modem_power_set(1);
		stx_uart_start(SVC_MODEM_UART, &svc_modem_cb);
		stx_timer10_start(&svc_modem_process, NULL);

		if (fullCheck == 1) {
			rt = 36;
			do {
				stx_watchdog_ok();
				util_msleep(500);
				rt--;
			} while (rt > 0);

			rt = 5;
			do {
				util_print("  Check On...\r\n");
				svc_modem_init_req(&mreq, 1, 4, "AT\r\n", "OK", NULL);
				rc = svc_modem_send_req(&mreq);
				if (rc == 0) {
					handle->state = MODEM_S_ON;
					util_print("  Checking AT : %s", mreq.ret);
				}
				stx_watchdog_ok();
				rt--;
			} while ((rt > 0) && (rc != 0));

			rt = 2;
			do {
				util_print("  Check Echo...\r\n");
				svc_modem_init_req(&mreq, 1, 3, "ATE0\r\n", "OK", NULL);
				rc = svc_modem_send_req(&mreq);
				if (rc == 0) {
					handle->state = MODEM_S_CNUM;
					util_print("  Check Echo OK : %s", mreq.ret);
				}
				stx_watchdog_ok();
				rt--;
			} while ((rt > 0) && (rc != 0));

			rt = 5;
			do {
				util_print("  Check PhoneNo...\r\n");
				svc_modem_init_req(&mreq, 1, 3, "AT+CNUM\r\n", "+CNUM", NULL);
				rc = svc_modem_send_req(&mreq);
				if (rc == 0) {
					char_t *p = strstr(mreq.ret, "82");
					if (p != NULL) {
						strcpy(handle->pno, "KR0");
						memcpy(handle->pno + 3, p + 2, 10);
						util_print("  Check PhoneNo OK : %s\r\n", handle->pno);
						handle->state = MODEM_S_CREG;
					} else {
						rc = -1;
					}
				} else {
					int32_t to = 100;
					while (to > 0) {
						stx_watchdog_ok();
						util_msleep(30);
						to--;
					}
				}
				stx_watchdog_ok();
				rt--;
			} while ((rt > 0) && (rc != 0));

			rt = 3;
			do {
				util_print("  Check Network...\r\n");
				svc_modem_init_req(&mreq, 1, 3, "AT+CREG?\r\n", "+CREG: 0,1",
				NULL);
				rc = svc_modem_send_req(&mreq);
				if (rc == 0) {
					handle->state = MODEM_S_READY;
					util_print("  Checking Network OK : %s", mreq.ret);
				}
				stx_watchdog_ok();
				rt--;
			} while ((rt > 0) && (rc != 0));
		} else {
			util_print("Tuning on the modem... done (just power on)\r\n");
		}
	}
}

void svc_modem_stop(void) {
	int32_t rc;

	int32_t rt;
	modem_req_t mreq;

	svc_modem_t *handle = &g_modem;

	if (handle->state == MODEM_S_OFF) {
		util_print("Modem is already off!!!\r\n");
	} else {
		util_print("Tuning off the modem...\r\n");

		svc_modem_init_req(&mreq, 1, 3, "AT#SHDN\r\n", "OK", NULL);
		rc = svc_modem_send_req(&mreq);
		if (rc != 0) {
			util_print("  AT#SHDN timeout!!\r\n");
		} else {
			stx_watchdog_ok();

			rt = 24;
			do {
				util_msleep(500);
				stx_watchdog_ok();
				rt--;
			} while (rt > 0);
		}

		stx_timer10_stop();
		stx_uart_stop(SVC_MODEM_UART);

		util_msleep(10);
		stx_modem_power_set(0);

		rt = 30;
		do {
			util_msleep(500);
			stx_watchdog_ok();
			rt--;
		} while (rt > 0);

		stx_watchdog_ok();

		util_print("Tuning off the modem... done!\r\n");

		g_modem.state = MODEM_S_OFF;
	}
}

void svc_modem_status(void) {
	svc_modem_t *handle = &g_modem;

	util_print("[MODEM STAUS]\r\n");
	util_print("  * PhoneNo.: %s\r\n", handle->pno);
	util_print("  * State   : %s\r\n", svc_modem_state_string(handle->state));
	util_print("  * Mode    : %s\r\n", svc_modem_mode_string(handle->r_mode));
	util_print("  * R-Q     : %d\r\n", stx_q_size(&handle->rq));
}

int32_t svc_modem_time_sync(int8_t timeout) {
	int32_t rc = -1L;

	char_t tbuf[8];
	stx_ts_t mts;

	modem_req_t mreq;
	svc_modem_t *handle = &g_modem;

	util_print("Time-Sync...\r\n");
	if (handle->state < MODEM_S_READY) {
		util_err0("Modem is not ready!!\r\n");
	} else {
		svc_modem_init_req(&mreq, 1, timeout, "AT+CCLK?\r\n", "+CCLK: \"",
		NULL);
		rc = svc_modem_send_req(&mreq);
		if (rc < 0) {
			util_err1("Time-Sync failed!! (ret = %d)\r\n", rc);
			rc = -2L;
		} else {
			tbuf[0] = mreq.ret[8];
			tbuf[1] = mreq.ret[9];
			tbuf[2] = '\0';
			mts.year = 2000 + strtol(tbuf, NULL, 10);

			tbuf[0] = mreq.ret[11];
			tbuf[1] = mreq.ret[12];
			tbuf[2] = '\0';
			mts.mon = strtol(tbuf, NULL, 10);

			tbuf[0] = mreq.ret[14];
			tbuf[1] = mreq.ret[15];
			tbuf[2] = '\0';
			mts.day = strtol(tbuf, NULL, 10);

			tbuf[0] = mreq.ret[17];
			tbuf[1] = mreq.ret[18];
			tbuf[2] = '\0';
			mts.hour = strtol(tbuf, NULL, 10);

			tbuf[0] = mreq.ret[20];
			tbuf[1] = mreq.ret[21];
			tbuf[2] = '\0';
			mts.min = strtol(tbuf, NULL, 10);

			tbuf[0] = mreq.ret[23];
			tbuf[1] = mreq.ret[24];
			tbuf[2] = '\0';
			mts.sec = strtol(tbuf, NULL, 10);
			mts.msec = 0;

			if (mts.year > 2010) {
				stx_clock_set(&mts);
				util_print(
						"Time-Sync... done! (%04d-%02d-%02d %02d:%02d:%02d)\r\n",
						mts.year, mts.mon, mts.day, mts.hour, mts.min, mts.sec);
				rc = 0L;
			} else {
				util_err0("Invalid time!!!\r\n");
				stx_clock_ts_print(&mts);
				rc = -1L;
			}
		}
	}

	return rc;
}

int32_t svc_modem_ppp_open(int8_t timeout) {
	int32_t rc = -1L;

	modem_req_t mreq;
	svc_modem_t *handle = &g_modem;

	util_print("PPP opening...\r\n");
	if (handle->state < MODEM_S_READY) {
		util_err0("PPP opening... failed! (Modem is not ready!!)\r\n");
	} else {
		if (handle->state >= MODEM_S_PPP_ON) {
			util_print("PPP opening... done! (already opened)\r\n");
			rc = 0L;
		} else {
			svc_modem_clear_q();

			svc_modem_init_req(&mreq, 1, timeout, "AT+CGATT=1\r\n", "OK", NULL);
			rc = svc_modem_send_req(&mreq);
			if (rc < 0) {
				util_err1("PPP opening... failed!! (ret = %d)\r\n", rc);
				rc = -2L;
			} else {
				svc_modem_init_req(&mreq, 1, 7, "AT#SGACT=1,1\r\n", "OK", NULL);
				rc = svc_modem_send_req(&mreq);
				if (rc < 0) {
					util_err1(
							"PPP opening... failed on CTX Activation!! (ret = %d)\r\n",
							rc);
					rc = -1L;
				} else {
					util_print("PPP opening... done!\r\n");
					handle->state = MODEM_S_PPP_ON;
					rc = 0L;
				}
			}
		}
	}

	return rc;
}

int32_t svc_modem_ppp_close(int8_t timeout) {
	int32_t rc = -1L;

	modem_req_t mreq;
	svc_modem_t *handle = &g_modem;

	util_print("PPP closing...\r\n");
	if (handle->state < MODEM_S_PPP_ON) {
		util_err0("PPP closing... failed! (PPP is not opened!!)\r\n");
	} else {
		svc_modem_init_req(&mreq, 1, 3, "AT#SGACT=1,0\r\n", "OK", NULL);
		rc = svc_modem_send_req(&mreq);
		if (rc < 0) {
			util_err1(
					"PPP closing... failed on CTX Deactivation (ret = %d)\r\n",
					rc);
			rc = -1L;
		} else {
			svc_modem_init_req(&mreq, 1, timeout, "AT+CGATT=0\r\n", "OK", NULL);
			rc = svc_modem_send_req(&mreq);
			if (rc < 0) {
				util_err1("PPP closing... failed!! (ret = %d)\r\n", rc);
				rc = -2L;
			} else {
				util_print("PPP closing... done!\r\n");
				handle->state = MODEM_S_READY;
			}
		}
	}

	return rc;
}

void svc_modem_mode_set(int8_t mode) {
	svc_modem_t *handle = &g_modem;
	handle->r_mode = mode;
	handle->rcnt = 0;
	stx_q_clear(&handle->rq);
	if (RECV_MODE_LINE == mode) {
		handle->state = MODEM_S_PPP_ON;
	}
}

int32_t svc_modem_mode_get(void) {
	svc_modem_t *handle = &g_modem;
	return handle->r_mode;
}

char_t * svc_modem_mode_string(int8_t mode) {
	char_t *rv;

	switch (mode) {
	case RECV_MODE_LINE:
		rv = "RECV_MODE_LINE";
		break;
	case RECV_MODE_BUFF:
		rv = "RECV_MODE_BUFF";
		break;
	default:
		rv = "Unknown";
		break;
	}

	return rv;
}

int32_t svc_modem_state_get(void) {
	svc_modem_t *handle = &g_modem;
	return handle->state;
}

char_t *svc_modem_state_string(modem_state_e state) {
	char_t *rv;

	switch (state) {
	case MODEM_S_OFF:
		rv = "MODEM_S_OFF";
		break;
	case MODEM_S_BOOTING:
		rv = "MODEM_S_BOOTING";
		break;
	case MODEM_S_ATTATCH:
		rv = "MODEM_S_ATTATCH";
		break;
	case MODEM_S_ON:
		rv = "MODEM_S_ON";
		break;
	case MODEM_S_CREG:
		rv = "MODEM_S_CREG";
		break;
	case MODEM_S_CNUM:
		rv = "MODEM_S_CNUM";
		break;
	case MODEM_S_READY:
		rv = "MODEM_S_READY";
		break;
	case MODEM_S_PPP_ON:
		rv = "MODEM_S_PPP_ON";
		break;
	case MODEM_S_SENDING:
		rv = "MODEM_S_SENDING";
		break;
	case MODEM_S_RECEVING:
		rv = "MODEM_S_RECEVING";
		break;
	default:
		rv = "Unknown";
		break;
	}

	return rv;
}

char_t *svc_modem_phone_no(void) {
	svc_modem_t *handle = &g_modem;
	return handle->pno;
}

void svc_modem_clear_q(void) {
	svc_modem_t *handle = &g_modem;
	stx_q_clear(&handle->rq);
}

int32_t svc_modem_write(void *dptr, int16_t len) {
	return stx_uart_write(SVC_MODEM_UART, dptr, len);
}

void svc_modem_init_req(modem_req_t *req, int8_t sync, int8_t timeout,
		char_t *cmd, char_t *filterStr, const modem_filter_fn filterFunc) {
	if (req != NULL) {
		req->sync = sync;
		req->timeout = timeout;

		memset(req->cmd, 0, sizeof(req->cmd));
		if (cmd != NULL) {
			req->cmdLen = strlen(cmd);
			if (req->cmdLen > (int) sizeof(req->cmd)) {
				util_err1("Given COMMAND is too long(%d)\r\n", req->cmdLen);
			} else {
				strncpy(req->cmd, cmd, sizeof(req->cmd));
			}
		} else {
			req->cmdLen = 0;
		}

		memset(req->filterStr, 0, sizeof(req->filterStr));
		if (filterStr != NULL) {
			req->filterLen = strlen(filterStr);
			if (req->filterLen > (int) sizeof(req->filterStr)) {
				util_err1("Given Filter-String is too long(%d)\r\n",
						req->filterLen);
			} else {
				strncpy(req->filterStr, filterStr, sizeof(req->filterStr));
			}
		} else {
			req->filterLen = 0;
		}
		if (filterFunc != NULL) {
			req->filterFunc = filterFunc;
		}
		memset(req->ret, 0, sizeof(req->ret));
		req->retLen = 0;
	} else {
		util_err0("Given request is (nil)\r\n");
	}
}

int32_t svc_modem_send_req(modem_req_t *req) {
	int32_t rc = -1L;

	svc_modem_t *handle = &g_modem;

	if (req != NULL) {
		req->done = 0;
		handle->req = req;

		if (req->sync == 1) {
			int32_t ct;
			int32_t timeout;

			ct = stx_clock_tick_sec();
			timeout = ct + req->timeout;

			MODEM_LOG("ctime : %d / timeout = %d\r\n", ct, timeout);

			if (req->cmdLen > 0) {
				MODEM_LOG("before cmd (%d) : %s \r\n", req->cmdLen, req->cmd);
				svc_modem_write(req->cmd, req->cmdLen);
				MODEM_LOG("after  cmd\r\n");
			}
			MODEM_LOG("before waiting\r\n");
			while ((req->done == 0) && (timeout > ct)) {
				util_msleep(100);
				ct = stx_clock_tick_sec();
			}
			MODEM_LOG("after waiting\r\n");
			if ((req->done == 0) || (timeout < ct)) {
				util_err4("(Timeout:%d / Time:%d) Result : %s, Length = %d\r\n",
						timeout, ct, req->ret, req->retLen);
				rc = -2L; /* timeout */
			} else {
				if (req->done < 0) {
					util_print("ERROR\r\n");
					rc = -1L;
				} else {
					MODEM_LOG("done (Time:%d)\r\n", ct);
					rc = 0L;
				}
			}
		} else {
			if (req->cmdLen > 0) {
				MODEM_LOG("before cmd : %s (%d)\r\n", req->cmd, req->cmdLen);
				svc_modem_write(req->cmd, req->cmdLen);
				MODEM_LOG("after  cmd\r\n");
			}
			rc = 0L;
		}
	} else {
		util_err0("Given request is (nil)\r\n");
	}

	return rc;
}

int32_t svc_modem_data_recv(void *dptr, int16_t len) {
	int32_t rc = -1L;

	stx_qd_t *qd;
	svc_modem_t *handle = &g_modem;

	if (len > MODEM_ITM_LEN) {
		util_err0("recv r-buffer underflow!!\r\n");
	} else if (handle->r_mode != RECV_MODE_BUFF) {
		util_err0("recv-mode is not buffer-mode!!\r\n");
	} else {
		rc = 0L;

		if (handle->state != MODEM_S_RECEVING) {
			handle->state = MODEM_S_RECEVING;
		}
		qd = stx_q_pop(&handle->rq);
		if (qd != NULL) {
			memcpy(dptr, qd->data, qd->dlen);
			rc = qd->dlen;
		}
	}

	return rc;
}

int32_t svc_modem_data_send(void *dptr, int16_t len) {
	int32_t rc = -1L;

	svc_modem_t *handle = &g_modem;

	if (handle->state < MODEM_S_PPP_ON) {
		util_err0("PPP is not ON!!\r\n");
	} else if (len > MODEM_ITM_LEN) {
		util_err0("send q-buffer overflow!!\r\n");
	} else {
		if (handle->state != MODEM_S_SENDING) {
			handle->state = MODEM_S_SENDING;
		}
		svc_modem_write(dptr, len);
		rc = len;
	}

	return rc;
}

void svc_modem_process(void *args) {
	stx_qd_t *qd;
	svc_modem_t *handle = &g_modem;

	if (args != NULL) {
		/* Nothing to do */
	}

	if ((RECV_MODE_BUFF == handle->r_mode)) {
		if (handle->rcnt > 0) {
			__IO int32_t rdiff = stx_clock_tick() - handle->rts;
			if (rdiff > 199) {
				svc_console_puts("buffer receiving is timeout...\r\n");
				if (MODEM_DEBUG) {
					util_hexdump("TIMEOUT", handle->rbuf, handle->rcnt);
				}
				stx_q_push(&handle->rq, handle->rbuf, handle->rcnt);
				handle->rcnt = 0;
			}
		}
	}

	if (handle->r_mode == RECV_MODE_LINE) {
		while ((stx_q_size(&handle->rq) > 0) && (handle->req != NULL)) {
			stx_modem_led_set(1);

			qd = stx_q_pop(&handle->rq);
			if (strncmp("ERROR\r\n", qd->data, 7) == 0) {
				handle->req->retLen = qd->dlen;
				memcpy(handle->req->ret, qd->data, qd->dlen);
				handle->req->done = -1;
				if ((handle->req->sync == 0) && (handle->req->cb != NULL)) {
					handle->req->cb(handle->req);
				}
				handle->req = NULL;
			} else if (handle->req->filterLen > 0) {
				if (strncmp(qd->data, handle->req->filterStr,
						handle->req->filterLen) == 0) {
					handle->req->retLen = qd->dlen;
					memcpy(handle->req->ret, qd->data, qd->dlen);
					handle->req->done = 1;
					if ((handle->req->sync == 0) && (handle->req->cb != NULL)) {
						handle->req->cb(handle->req);
					}
					handle->req = NULL;
				} else {
					if (MODEM_DEBUG) {
						svc_console_puts("StringFilter : not matched\r\n");
					}
					svc_console_write(qd->data, qd->dlen);
				}
			} else if (handle->req->filterFunc != NULL) {
				if (handle->req->filterFunc(qd->data, qd->dlen) == 1) {
					handle->req->retLen = qd->dlen;
					memcpy(handle->req->ret, qd->data, qd->dlen);
					handle->req->done = 1;
					if ((handle->req->sync == 0) && (handle->req->cb != NULL)) {
						handle->req->cb(handle->req);
					}
					handle->req = NULL;
				} else {
					if (MODEM_DEBUG) {
						svc_console_puts("FunctionFilter : not matched\r\n");
					}
					svc_console_write(qd->data, qd->dlen);
				}
			} else {
				svc_console_puts("Error : cannot be here!!!\r\n");
				svc_console_write(qd->data, qd->dlen);
			}
		}

		stx_modem_led_set(0);
	}

}

#else /* !MODEM_TELIT */
typedef int compiler_happy_pedantic;
#endif /* MODEM_TELIT */
