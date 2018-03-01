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

#include "stx_uart.h"

#include "stx_driver.h"

#include "stx_cpu.h"
#include "stx_util.h"

#include <string.h>

#ifdef S_SPLINT_S
#include "stm32f1xx_hal_dma.h"
#include "stm32f1xx_hal_uart.h"
#include "stm32f1xx_hal_cortex.h"
#endif

/* ----------------------------------------------
 * Internal Structures
 * ---------------------------------------------- */
#define STX_UART_TX_BUF_SIZE   4096
#define STX_UART_RX_BUF_SIZE   1024

#define STX_UART_TIMEOUT       0x1000

typedef struct s_stx_uart_t {
	uint8_t port;

	__IO int8_t tx_wait;
	uint8_t tx_one;
	uint8_t rx_one;

	__IO int16_t tx_head;
	__IO int16_t tx_tail;
	__IO int16_t tx_size;
	__IO int16_t tx_full_count; /* TX Buffer Overflow */
	__IO uint32_t tx_bytes;
	__IO uint32_t tx_lock;

	__IO int16_t rx_head;
	__IO int16_t rx_tail;
	__IO int16_t rx_size;
	__IO int16_t rx_full_count; /* RX Buffer Overflow */
	__IO uint32_t rx_bytes;
	__IO uint32_t rx_lock;

	__IO uint8_t rx_buf[STX_UART_RX_BUF_SIZE];
	__IO uint8_t tx_buf[STX_UART_TX_BUF_SIZE];

	__IO uint16_t isr_count;
	__IO uint16_t err_count;
	__IO uint16_t err_last;

	stx_cb_byte cb_fn;
	UART_HandleTypeDef *huart;
	int32_t irqNo;
} stx_uart_t;

static stx_uart_t g_uart[STX_UART_NUM_PORT];
static __IO int32_t g_uart_init = 0;

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

/* ----------------------------------------------
 * Internal Functions
 * ---------------------------------------------- */

/* Forward Declaration : Begin */
static void stx_uart_nvic_set(const stx_uart_t *handle, int32_t enable);
static stx_uart_t *stx_uart_get(const UART_HandleTypeDef *huart);
static stx_uart_t *stx_uart_get_by_port(uint8_t port);
static void stx_uart_push_rxc(stx_uart_t *handle);
static void stx_uart_push_txc(stx_uart_t *handle, char_t data);
/* Forward Declaration : End */

static void stx_uart_nvic_set(const stx_uart_t *handle, int32_t enable) {
#if 0
	HAL_NVIC_SetPriority((IRQn_Type) handle->irqNo, 1, 0);
#endif
	if (enable) {
		HAL_NVIC_EnableIRQ((IRQn_Type) handle->irqNo);
	} else {
		HAL_NVIC_DisableIRQ((IRQn_Type) handle->irqNo);
	}
}

static stx_uart_t *stx_uart_get(const UART_HandleTypeDef *huart) {
	stx_uart_t *handle = NULL;

	if (huart != NULL) {
		if (huart->Instance == USART1) {
			handle = &g_uart[STX_UART_PORT_1];
		} else if (huart->Instance == USART2) {
			handle = &g_uart[STX_UART_PORT_2];
		} else {
			/* Nothing to do */
		}
	}

	return handle;
}

static stx_uart_t *stx_uart_get_by_port(uint8_t port) {
	stx_uart_t *handle = NULL;

	if (port < STX_UART_NUM_PORT) {
		handle = &g_uart[port];
	}

	return handle;
}

static void stx_uart_push_rxc(stx_uart_t *handle) {
	__IO uint32_t primask;

	primask = stx_cpu_disable_interrupt();
	if (handle->rx_size < STX_UART_RX_BUF_SIZE) {
		handle->rx_buf[handle->rx_tail] = handle->rx_one;
		handle->rx_tail++;
		if (handle->rx_tail >= STX_UART_RX_BUF_SIZE) {
			handle->rx_tail = 0;
		}
		handle->rx_size++;
	} else {
		handle->rx_full_count++;
	}
	stx_cpu_restore_interrupt(primask);
}

static void stx_uart_push_txc(stx_uart_t *handle, char_t data) {
	if (handle->tx_size < STX_UART_TX_BUF_SIZE) {
		handle->tx_buf[handle->tx_tail] = data;
		handle->tx_tail++;
		if (handle->tx_tail >= STX_UART_TX_BUF_SIZE) {
			handle->tx_tail = 0;
		}
		handle->tx_size++;
	} else {
		handle->tx_full_count++;
	}
}

/* ----------------------------------------------
 * Interface Implementations
 * ---------------------------------------------- */

void stx_uart_init(uint8_t port) {
	stx_uart_t *handle = stx_uart_get_by_port(port);
	if (handle != NULL) {
		handle->port = port;

		handle->tx_wait = 0;

		handle->tx_head = 0;
		handle->tx_tail = 0;
		handle->tx_size = 0;
		handle->tx_bytes = 0;
		handle->tx_lock = 0;
		handle->tx_full_count = 0;

		handle->rx_head = 0;
		handle->rx_tail = 0;
		handle->rx_size = 0;
		handle->rx_bytes = 0;
		handle->rx_lock = 0;
		handle->rx_full_count = 0;

		handle->isr_count = 0;
		handle->err_count = 0;

		handle->cb_fn = NULL;

		if (port == STX_UART_PORT_1) {
			handle->huart = &huart1;
			handle->irqNo = USART1_IRQn;
			g_uart_init = 1;
		} else if (port == STX_UART_PORT_2) {
			handle->huart = &huart2;
			handle->irqNo = USART2_IRQn;
		} else {
			/* Nothing to do */
		}

		stx_uart_stop(port);
	}
}

void stx_uart_start(uint8_t port, stx_cb_byte fn) {
	stx_uart_t *handle = stx_uart_get_by_port(port);
	if (handle != NULL) {
		stx_uart_nvic_set(handle, 1);
		handle->cb_fn = fn;
		__HAL_UART_ENABLE_IT(handle->huart, UART_IT_RXNE);
		__HAL_UART_ENABLE_IT(handle->huart, UART_IT_TC);
		(void) HAL_UART_Receive_IT(handle->huart, &handle->rx_one, 1);
	}
}

void stx_uart_stop(uint8_t port) {
	stx_uart_t *handle = stx_uart_get_by_port(port);
	if (handle != NULL) {
		__HAL_UART_DISABLE_IT(handle->huart, UART_IT_RXNE);
		__HAL_UART_DISABLE_IT(handle->huart, UART_IT_TC);
		stx_uart_nvic_set(handle, 0);
	}
}

int32_t stx_uart_write(uint8_t port, const char_t *data, int32_t len) {
	int32_t rc = -1L;
	__IO uint32_t primask;

	stx_uart_t *handle = stx_uart_get_by_port(port);
	if (handle != NULL) {
		int32_t i;

		primask = stx_cpu_disable_interrupt();
		for (i = 0; i < len; i++) {
			stx_uart_push_txc(handle, data[i]);
		}
		stx_cpu_restore_interrupt(primask);

		if (handle->tx_wait == 0) {
			handle->tx_wait = 1;
			stx_uart_pop_txc(port, &handle->tx_one);
			(void) HAL_UART_Transmit_IT(handle->huart, &handle->tx_one, 1);
		}
		rc = len;
	}

	return rc;
}

int32_t stx_uart_putc(uint8_t port, char_t data) {
	int32_t rc = -1L;
	__IO uint32_t primask;

	stx_uart_t *handle = stx_uart_get_by_port(port);
	if (handle != NULL) {
		primask = stx_cpu_disable_interrupt();
		stx_uart_push_txc(handle, data);
		stx_cpu_restore_interrupt(primask);

		if (handle->tx_wait == 0) {
			handle->tx_wait = 1;
			stx_uart_pop_txc(port, &handle->tx_one);
			(void) HAL_UART_Transmit_IT(handle->huart, &handle->tx_one, 1);
		}
		rc = data;
	}

	return rc;
}

int32_t stx_uart_puts(uint8_t port, char_t *data) {
	int32_t len = strlen(data);
	return stx_uart_write(port, data, len);
}

int32_t stx_uart_pop_rxc(uint8_t port, uint8_t *data) {
	int32_t rc = 0L;
	__IO uint32_t primask;

	stx_uart_t *handle = stx_uart_get_by_port(port);
	if (handle != NULL) {
		primask = stx_cpu_disable_interrupt();
		if (0 < handle->rx_size) {
			*data = handle->rx_buf[handle->rx_head];
			handle->rx_head++;
			if (handle->rx_head >= STX_UART_RX_BUF_SIZE) {
				handle->rx_head = 0;
			}
			handle->rx_size--;
			rc = 1L;
		}
		stx_cpu_restore_interrupt(primask);
	}

	return rc;
}

int32_t stx_uart_pop_txc(uint8_t port, uint8_t *data) {
	int32_t rc = 0L;
	__IO uint32_t primask;

	stx_uart_t *handle = stx_uart_get_by_port(port);
	if (handle != NULL) {
		primask = stx_cpu_disable_interrupt();
		if (0 < handle->tx_size) {
			*data = handle->tx_buf[handle->tx_head];
			handle->tx_head++;
			if (handle->tx_head >= STX_UART_TX_BUF_SIZE) {
				handle->tx_head = 0;
			}
			handle->tx_size--;
			rc = 1L;
		}
		stx_cpu_restore_interrupt(primask);
	}

	return rc;
}

void stx_uart_status(void) {
	int32_t i;
	stx_uart_t *handle = NULL;

	util_print("[UART]\r\n");
	for (i = 0; i < STX_UART_NUM_PORT; i++) {
		handle = stx_uart_get_by_port(i);
		util_print("  * PORT-%d\r\n", (i + 1));
		util_print("    - rx    : bytes = %d", handle->rx_bytes);
		util_print(" / queue = %d", handle->rx_size);
		util_print(" / q-full = %d\r\n", handle->rx_full_count);
		util_print("    - tx    : bytes = %d", handle->tx_bytes);
		util_print(" / queue = %d", handle->tx_size);
		util_print(" / q-full = %d\r\n", handle->tx_full_count);
		util_print("    - error : count = %d", handle->err_count);
		util_print(" / last-error = %d\r\n", handle->err_last);
	}
}

/* ------------------------------------------------
 * The interrupt handler of STM32
 * ------------------------------------------------ */

#ifdef __ICCARM__
#pragma diag_suppress=Pm011,Pm049,Pm141,Pm150
int fputc(int ch, FILE *f) {
	(void)(f);
	if (g_uart_init != 0) {
		stx_uart_putc(STX_UART_PORT_1, ch);
	} else {
		(void) HAL_UART_Transmit(&huart1, (uint8_t *) &ch, 1, STX_UART_TIMEOUT);
	}
	return ch;
}
#pragma diag_default=Pm011,Pm049,Pm141,Pm150
#endif /* __ICCARM__ */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	stx_uart_t *handle = stx_uart_get(huart);
	if (handle != NULL) {
		if (handle->cb_fn != NULL) {
			handle->rx_bytes++;
			handle->cb_fn(handle->rx_one);
		} else {
			stx_uart_push_rxc(handle);
		}
		(void) HAL_UART_Receive_IT(huart, &handle->rx_one, 1);
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	stx_uart_t *handle = stx_uart_get(huart);
	if (handle != NULL) {
		int32_t rc = stx_uart_pop_txc(handle->port, &handle->tx_one);
		if (rc == 1) {
			handle->tx_wait = 1;
			handle->tx_bytes++;
			(void) HAL_UART_Transmit_IT(handle->huart, &handle->tx_one, 1);
		} else {
			handle->tx_wait = 0;
		}
	}
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
	stx_uart_t *handle = stx_uart_get(huart);
	if (handle != NULL) {
		handle->err_count++;
		handle->err_last = (uint16_t) huart->ErrorCode;
	}
}
