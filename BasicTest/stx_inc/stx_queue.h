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

#ifndef STX_QUEUE_H
#define STX_QUEUE_H

/* -----------------------------------------
 * The Queue functions for STM32
 * ----------------------------------------- */

#include "misra.h"

/*
 * Start C++ Declaration
 */
#ifdef __cplusplus
extern "C" {
#endif

typedef struct s_stx_qd_t {
	int32_t dlen;
	char_t *data;
} stx_qd_t;

typedef struct s_stx_q_t {
	__IO uint32_t head;
	__IO uint32_t size;
	__IO uint32_t ulen;
	__IO uint32_t capacity;
	__IO int32_t useLock;
	char_t *buf;     /* buf will be partitioned by [ulen] */
	stx_qd_t *qd;    /* qd->data is the partitioned pointer */
} stx_q_t;

int32_t   /*@alt void@*/ stx_q_init(stx_q_t *q, char_t *buf /* buf-size = ulen * qlen */,
		                          stx_qd_t qarr[],
								  uint32_t ulen, /* unit length */
								  uint32_t qlen,  /* queue length */
								  int32_t useLock);
int32_t   stx_q_capacity(const stx_q_t *q);
int32_t   stx_q_size(const stx_q_t *q);
int32_t   stx_q_empty(const stx_q_t *q);
int32_t   stx_q_full(const stx_q_t *q);
stx_qd_t  /*@alt void@*/ *stx_q_pop(stx_q_t *q);
int32_t   /*@alt void@*/ stx_q_push(stx_q_t *q, void *d, uint32_t dlen);
int32_t   stx_q_push_str(stx_q_t *q, char_t *str);
int32_t   /*@alt void@*/ stx_q_clear(stx_q_t *q);

/*
 * End C++ Declaration
 */
#ifdef __cplusplus
}
#endif

#endif /* STX_QUEUE_H */
