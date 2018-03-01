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

#include "stx_util.h"

#include "stx_uart.h"

#include "stx_driver.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

/* ----------------------------------------------
 * Internal Structures
 * ---------------------------------------------- */

#define UTIL_PRINT_PORT       STX_UART_PORT_2

/* ----------------------------------------------
 * Internal Functions
 * ---------------------------------------------- */

static void util_delay(__IO int32_t delay_count);

static void util_delay(__IO int32_t delay_count) {
	for (; delay_count > 0; delay_count--) {
		__NOP();
	}
}

/* ----------------------------------------------
 * Interface Implementations
 * ---------------------------------------------- */

int32_t util_print(const char_t *fmt, ...) {
	char_t msg[LOG_LINE_MAX];

	va_list va;

	/* Boost Variables */
	int32_t msglen;

	memset(msg, 0, sizeof(msg));

	va_start(va, fmt);
	msglen = vsnprintf(msg, (sizeof(msg) - 1U), fmt, va);
	va_end(va);

	return stx_uart_write(UTIL_PRINT_PORT, msg, msglen);
}

void util_hexdump(char_t *desc, void *addr, int32_t len) {
	int32_t i;
	uint8_t buff[32];
	uint8_t *pc = (uint8_t *) addr;

	/* Output description if given. */
	if (desc != NULL) {
		util_print("%s:\r\n", desc);
	}

	if (len > 0L) {
		/* Process every byte in the data. */
		for (i = 0; i < len; i++) {
			/* Multiple of 16 means new line (with line offset). */

			if ((i % 16) == 0) {
				/* Just don't print ASCII for the zeroth line. */
				if (i != 0) {
					util_print("  %s\r\n", buff);
				}

				/* Output the offset. */
				util_print("  %06x ", i);
			}

			/* Now the hex code for the specific character. */
			util_print(" %02x", pc[i]);

			/* And store a printable ASCII character for later. */
			if ((pc[i] < 0x20) || (pc[i] > 0x7e)) {
				buff[i % 16] = '.';
			} else {
				buff[i % 16] = pc[i];
			}
			buff[(i % 16) + 1] = '\0';
		}

		/* Pad out last line if not exactly 16 characters. */
		while ((i % 16) != 0L) {
			util_print("   ");
			i++;
		}

		/* And print the final ASCII bit. */
		util_print("  %s\r\n", buff);
	} else if (len == 0L) {
		util_err0("  ZERO LENGTH\r\n");
	} else {
		util_err1("  NEGATIVE LENGTH: %i\r\n", len);
	}
}

void util_hexdump2(void *addr, int32_t len, int32_t offset) {
	int32_t i;
	uint8_t buff[32];
	uint8_t *pc = (uint8_t *) addr;

	if (len > 0L) {
		/* Process every byte in the data. */
		for (i = 0; i < len; i++) {
			/* Multiple of 16 means new line (with line offset). */

			if ((i % 16) == 0) {
				/* Just don't print ASCII for the zeroth line. */
				if (i != 0) {
					util_print("  %s\r\n", buff);
				}

				/* Output the offset. */
				util_print("  %06x ", (i + offset));
			}

			/* Now the hex code for the specific character. */
			util_print(" %02x", pc[i]);

			/* And store a printable ASCII character for later. */
			/* 0x20 == ' ' && 0x7e == '~' */
			if ((pc[i] < 0x20) || (pc[i] > 0x7e)) {
				buff[i % 16] = '.';
			} else {
				buff[i % 16] = pc[i];
			}
			buff[(i % 16) + 1] = '\0';
		}

		/* Pad out last line if not exactly 16 characters. */
		while ((i % 16) != 0) {
			util_print("   ");
			i++;
		}

		/* And print the final ASCII bit. */
		util_print("  %s\r\n", buff);
	} else if (len == 0) {
		util_err0("  ZERO LENGTH\r\n");
	} else {
		util_err1("  NEGATIVE LENGTH: %i\r\n", len);
	}
}

char_t *util_basename(/*@returned@*/char_t *path) {
	int32_t pos;
	char_t *rc = NULL;

	if (path) {
		pos = (int32_t) (strlen(path) - 1U);
		if (pos < 0L) {
			rc = path;
		} else {
			while ((path[pos] == '/') || (path[pos] == '\\')) {
				path[pos] = '\0';
				pos--;
			}
			while ((pos >= 0L) && (path[pos] != '/') && (path[pos] != '\\')) {
				pos--;
			}

			if (pos > 0L) {
				rc = &path[pos + 1];
			} else {
				rc = path;
			}
		}
	}

	return rc;
}

void util_msleep(__IO int32_t msec) {
	do {
		util_usleep(1000);
		msec--;
	} while (msec > 0);
}

void util_usleep(__IO int32_t usec) {
	do {
#ifdef __ICCARM__
		util_delay(5);
#else /* !__ICCARM__ */
		util_delay(12);
#endif /* __ICCARM__ */
		usec--;
	} while (usec > 0);
}

void util_nsleep(__IO int32_t n100sec) {
	do {
		util_delay(4);
		n100sec--;
	} while (n100sec > 0);
}
