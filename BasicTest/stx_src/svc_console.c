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

#include "svc_console.h"
#include "svc_main.h"

#include "stx_uart.h"
#include "stx_util.h"

#include <string.h>
#include <stdlib.h>

/* ----------------------------------------------
 * Internal Structures
 * ---------------------------------------------- */

#define SVC_CONSOLE_UART    STX_UART_PORT_2

#define SVC_CB_BUF_SIZE     256

#define CMD_HISTORY_COUNT     1
#define CMD_BUF_SIZE        512

static __IO uint32_t g_cmd_rcnt = 0;
static char_t g_cmd_rbuf[CMD_BUF_SIZE];

static char_t g_cmd_history[CMD_HISTORY_COUNT][CMD_BUF_SIZE];
static __IO uint8_t g_cmd_history_idx = 0;
static __IO uint8_t g_cmd_history_cnt = 0;

/* ----------------------------------------------
 * Internal Functions
 * ---------------------------------------------- */

/* Forward Declaration : Begin */
static void svc_console_history_add(char *cmd);
static void svc_console_history_get(void);
/* Forward Declaration : End */

static void svc_console_history_add(char *cmd) {
	if (g_cmd_history_idx < CMD_HISTORY_COUNT) {
		strncpy(g_cmd_history[g_cmd_history_idx], cmd, CMD_BUF_SIZE - 1);
	}

	if (g_cmd_history_cnt < CMD_HISTORY_COUNT) {
		g_cmd_history_cnt++;
	}
}

static void svc_console_history_get(void) {
	uint16_t i;

	for (i = 0; (i < g_cmd_rcnt) && (i < CMD_BUF_SIZE); i++) {
		stx_uart_putc(SVC_CONSOLE_UART, 0x08);
		stx_uart_putc(SVC_CONSOLE_UART, ' ');
		stx_uart_putc(SVC_CONSOLE_UART, 0x08);
	}

	strncpy(g_cmd_rbuf, g_cmd_history[g_cmd_history_idx], CMD_BUF_SIZE - 1);
	g_cmd_rcnt = strlen(g_cmd_rbuf);

	for (i = 0; (i < g_cmd_rcnt) && (i < CMD_BUF_SIZE); i++) {
		stx_uart_putc(SVC_CONSOLE_UART, g_cmd_rbuf[i]);
	}
}

/* ----------------------------------------------
 * Interface Implementations
 * ---------------------------------------------- */

void svc_console_init(void) {
	stx_uart_init(SVC_CONSOLE_UART);
}

void svc_console_start(void) {
	g_cmd_rcnt = 0;
	memset(g_cmd_rbuf, 0, sizeof(g_cmd_rbuf));
	stx_uart_start(SVC_CONSOLE_UART, NULL);
}

void svc_console_stop(void) {
	stx_uart_stop(SVC_CONSOLE_UART);
}

void svc_console_process(void) {
	static int32_t init_state = 1;
	static int32_t esc_state = 0;
	uint8_t data;
	int32_t rc;

	if (init_state) {
		g_cmd_rcnt = 0;
		memset(g_cmd_rbuf, 0, sizeof(g_cmd_rbuf));
		init_state = 0;
	}

	rc = stx_uart_pop_rxc(SVC_CONSOLE_UART, &data);
	if (rc > 0) {
		if (g_cmd_rcnt < (sizeof(g_cmd_rbuf) - 1)) {
			if (('\r' == data) || ('\n' == data)) {
				g_cmd_rbuf[g_cmd_rcnt] = 0;
				svc_main_push_cmd(g_cmd_rbuf, g_cmd_rcnt);
				init_state = 1;
				esc_state = 0;
				if (g_cmd_rcnt > 0) {
					svc_console_history_add(g_cmd_rbuf);
				}
				stx_uart_putc(SVC_CONSOLE_UART, '\r');
				stx_uart_putc(SVC_CONSOLE_UART, '\n');
			} else if ((0x08 == data) || (0x7F == data)) {
				/* Backspace */
				if (g_cmd_rcnt > 0) {
					g_cmd_rcnt--;
					g_cmd_rbuf[g_cmd_rcnt] = 0;
					stx_uart_putc(SVC_CONSOLE_UART, 0x08);
					stx_uart_putc(SVC_CONSOLE_UART, ' ');
					stx_uart_putc(SVC_CONSOLE_UART, 0x08);
				}
			} else if (0x1B == data) {
				/* ESC */
				esc_state = 1;
			} else {
				if (1 == esc_state) {
					if (0x5B == data) {
						esc_state = 2;
					} else {
						esc_state = 0;
					}
				} else if (2 == esc_state) {
					if (0x41 == data) {
						if (g_cmd_history_cnt > 0) {
							svc_console_history_get();
						}
					} else if (0x42 == data) {
						if (g_cmd_history_cnt > 0) {
							svc_console_history_get();
						}
					} else {
						/* Nothing to do */
					}
					esc_state = 0;
				} else {
					g_cmd_rbuf[g_cmd_rcnt++] = data;
					stx_uart_putc(SVC_CONSOLE_UART, data);
					esc_state = 0;
				}
			}
		}
	}
}

int32_t svc_console_write(void *dptr, int32_t len) {
	return stx_uart_write(SVC_CONSOLE_UART, dptr, len);
}

int32_t svc_console_puts(char_t *str) {
	return stx_uart_puts(SVC_CONSOLE_UART, str);
}
