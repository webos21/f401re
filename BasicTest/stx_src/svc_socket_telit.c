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

#include "svc_socket.h"

#include "svc_modem.h"

#if MODEM_TELIT

#include "stx_util.h"
#include "stx_clock.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* ----------------------------------------------
 * Internal Structures
 * ---------------------------------------------- */

#define SVC_SOCKET_DEBUG (1 != 1)

typedef struct s_sck_t {
	int32_t opened;
	/*@unique@*/
	char_t hostname[64];
	uint16_t port;
	int32_t state;
} sck_t;

static sck_t g_sockets[SVC_SOCKETS_MAX];

/* ----------------------------------------------
 * Internal Functions
 * ---------------------------------------------- */

/* ----------------------------------------------
 * Interface Implementations
 * ---------------------------------------------- */

int32_t svc_socket_open(void) {
	int32_t rc = -1L;

	if (svc_modem_state_get() != MODEM_S_PPP_ON) {
		util_err0("PPP is not opened!!!\r\n");
	} else {
		int32_t i;

		for (i = 0; i < SVC_SOCKETS_MAX; i++) {
			if (g_sockets[i].opened == 0) {
				g_sockets[i].opened = 1;
				rc = i;
				break;
			}
		}
		if (rc < 0) {
			util_err0("All socket is used!!!\r\n");
		}
	}

	return rc;
}

int32_t svc_socket_connect(int32_t fd, const char_t *hostname, uint16_t port,
		int32_t timeout) {
	int32_t rc = -1L;

	sck_t *handle;
	modem_req_t mreq;

	char_t cmdBuf[64];

	if ((fd < SVC_SOCKET_FD_MIN) || (fd > SVC_SOCKET_FD_MAX)) {
		util_err1("Invalid FD : %d\r\n", fd);
	} else {
		handle = &g_sockets[fd];

		memset(handle->hostname, 0, sizeof(handle->hostname));
		strcpy(handle->hostname, hostname);

		handle->port = port;

		svc_modem_clear_q();

		memset(cmdBuf, 0, sizeof(cmdBuf));
		(void) snprintf(cmdBuf, sizeof(cmdBuf), "AT#SD=%ld,0,%d,%s\r\n",
				(fd + 1), handle->port, handle->hostname);

		if (SVC_SOCKET_DEBUG) {
			util_log1("cmdBuf Length = %d\r\n", strlen(cmdBuf));
			util_log2("svc_modem_init_req(&mreq, 1, %d, %s, AT#SD: 0)\r\n",
					timeout, cmdBuf);
		}

		svc_modem_init_req(&mreq, 1, timeout, cmdBuf, "CONNECT", NULL);
		rc = svc_modem_send_req(&mreq);
		if (rc < 0) {
			util_err1("socket open failed on ADDR : %d\r\n", rc);
		} else {
			if (SVC_SOCKET_DEBUG) {
				util_log1("%s", mreq.ret);
			}
			handle->state = 1;

			svc_modem_mode_set(RECV_MODE_BUFF);
			rc = 0L;
		}
	}

	return rc;
}

int32_t svc_socket_recv(int32_t fd, void *buf, int32_t rlen) {
	int32_t rc = -1L;

	if ((fd < SVC_SOCKET_FD_MIN) || (fd > SVC_SOCKET_FD_MAX)) {
		util_err1("Invalid FD : %d\r\n", fd);
	} else if (rlen < 128) {
		util_err1("Given Buffer is too small : %d\r\n", rlen);
	} else {
		sck_t *handle = &g_sockets[fd];
		if (handle->state != 1) {
			util_err1("Socket is not connected : %d\r\n", fd);
		} else {
			int32_t rt = 5;
			do {
				rc = svc_modem_data_recv(buf, rlen);
				if (SVC_SOCKET_DEBUG) {
					util_log1("Received Length = %d\r\n", rc);
				}
				util_msleep(10);
				rt--;
			} while ((rt > 0) && (rc == 0));
		}
	}

	return rc;
}

int32_t svc_socket_send(int32_t fd, void *buf, int32_t wlen) {
	int32_t rc = -1L;

	if ((fd < SVC_SOCKET_FD_MIN) || (fd > SVC_SOCKET_FD_MAX)) {
		util_err1("Invalid FD : %d\r\n", fd);
	} else {
		sck_t *handle = &g_sockets[fd];
		if (handle->state != 1) {
			util_err1("Socket is not connected : %d\r\n", fd);
		} else {
			rc = svc_modem_write(buf, wlen);
		}
	}

	return rc;
}

int32_t svc_socket_close(int32_t fd) {
	int32_t rc = -1L;

	sck_t *handle;
	modem_req_t mreq;

	char_t cmdBuf[64];

	if ((fd < SVC_SOCKET_FD_MIN) || (fd > SVC_SOCKET_FD_MAX)) {
		util_err1("Invalid FD : %d\r\n", fd);
	} else {
		handle = &g_sockets[fd];
		handle->state = 0;

		svc_modem_write("+++", 3);
		svc_modem_mode_set(RECV_MODE_LINE);

		handle->opened = 0;

		memset(cmdBuf, 0, sizeof(cmdBuf));
		(void) snprintf(cmdBuf, sizeof(cmdBuf), "AT#SH=%ld\r\n", (fd + 1));

		svc_modem_init_req(&mreq, 1, 3, cmdBuf, "OK", NULL);
		rc = svc_modem_send_req(&mreq);
		if (rc < 0) {
			util_err1("socket close failed on AT#SH : %d\r\n", rc);
		}
		handle->opened = 0;
	}

	return rc;
}

#else /* !MODEM_TELIT */
typedef int compiler_happy_pedantic;
#endif /* MODEM_TELIT */
