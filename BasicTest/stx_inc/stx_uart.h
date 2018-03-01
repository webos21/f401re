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

#ifndef STX_UART_H
#define STX_UART_H

/* -----------------------------------------
 * The UART functions for STM32
 * ----------------------------------------- */

#include "stx_cb.h"

/*
 * Start C++ Declaration
 */
#ifdef __cplusplus
extern "C" {
#endif

#define STX_UART_NUM_PORT      2

#define STX_UART_PORT_1        0
#define STX_UART_PORT_2        1

void      stx_uart_init(uint8_t port);
void      stx_uart_start(uint8_t port, stx_cb_byte fn);
void      stx_uart_stop(uint8_t port);
int32_t   stx_uart_write(uint8_t port, const char_t *data, int32_t len);
int32_t   /*@alt void@*/ stx_uart_putc(uint8_t port, char_t data);
int32_t   stx_uart_puts(uint8_t port, char_t *data);
int32_t   stx_uart_pop_rxc(uint8_t port, uint8_t *data);
int32_t   /*@alt void@*/ stx_uart_pop_txc(uint8_t port, uint8_t *data);

void      stx_uart_status(void);

/*
 * End C++ Declaration
 */
#ifdef __cplusplus
}
#endif

#endif /* STX_UART_H */
