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

#include "stx_queue.h"

#include "stx_cpu.h"
#include "stx_util.h"

#include <string.h>

/* ----------------------------------------------
 * Internal Structures
 * ---------------------------------------------- */

/* ----------------------------------------------
 * Internal Functions
 * ---------------------------------------------- */

/* ----------------------------------------------
 * Interface Implementations
 * ---------------------------------------------- */

int32_t stx_q_init(stx_q_t *q, char_t *buf, stx_qd_t qarr[], uint32_t ulen, uint32_t qlen, int32_t useLock) {
	__IO uint32_t i = 0;

	int32_t rc = -1L;

	if ((q != NULL) && (buf != NULL) && (qarr != NULL) && (ulen >= 1) && (qlen >= 1)) {
		q->head = 0;
		q->size = 0;
		q->ulen = ulen;
		q->capacity = qlen;
		q->useLock = useLock;
		q->buf = buf;
		q->qd = qarr;

		for (i = 0; i < qlen; i++) {
			uint32_t pos = i * ulen;
			q->qd[i].dlen = 0;
			q->qd[i].data = &buf[pos];
		}
		rc = 0L;
	} else {
		util_err0("Cannot initialize the QUEUE!!\r\n");
	}

	return rc;
}

int32_t stx_q_capacity(const stx_q_t *q) {
	return q ? ((int32_t) q->capacity) : -1;
}

int32_t stx_q_size(const stx_q_t *q) {
	return q ? ((int32_t) q->size) : -1;
}

int32_t stx_q_empty(const stx_q_t *q) {
	return q ? (q->size == 0) : 0;
}

int32_t stx_q_full(const stx_q_t *q) {
	int32_t rc = 0L;
	if (q) {
		uint32_t cmpVal = q->size;
		rc = (cmpVal == q->capacity);
	}
	return rc;
}

stx_qd_t *stx_q_pop(stx_q_t *q) {
	stx_qd_t* ret = NULL;

	if ((q != NULL) && (q->qd != NULL)) {
		__IO uint32_t primask;

		if (q->useLock != 0) {
			primask = stx_cpu_disable_interrupt();
		}
		if (q->size > 0) {
			uint32_t qh = (q->head + 1);
			uint32_t qc = (q->capacity - 1);

			ret = &q->qd[q->head];
			q->head = ((qh) & (qc));
			q->size--;
		} else {
			/* Queue is empty!! */
		}
		if (q->useLock != 0) {
			stx_cpu_restore_interrupt(primask);
		}

	} else {
		util_err0("Queue is invalid!!\r\n");
	}

	return ret;
}

int32_t stx_q_push(stx_q_t *q, void *d, uint32_t dlen) {
	int32_t index = -1L;

	if ((q != NULL) && (q->qd != NULL)) {
		__IO uint32_t primask;
		uint32_t cmpVal = q->size;

		if (q->useLock != 0) {
			primask = stx_cpu_disable_interrupt();
		}
		if (cmpVal < q->capacity) {
			uint32_t qh = (q->head + cmpVal);
			uint32_t qc = (q->capacity - 1);

			index = ((qh) & (qc));
			memcpy(q->qd[index].data, d, dlen);
			if (dlen < q->ulen) {
				q->qd[index].data[dlen] = '\0';
			}
			q->qd[index].dlen = dlen;
			q->size++;
		} else {
			util_hexdump("Q-FULL", d, dlen);
			index = -2L;
		}
		if (q->useLock != 0) {
			stx_cpu_restore_interrupt(primask);
		}

	} else {
		util_err0("Queue is invalid!!\r\n");
	}

	return index;
}

int32_t stx_q_push_str(stx_q_t *q, char_t *str) {
	int32_t rc = -1L;

	if (str != NULL) {
		uint32_t slen = strlen(str);
		rc = stx_q_push(q, str, slen);
	}

	return rc;
}

int32_t stx_q_clear(stx_q_t *q) {
	q->head = 0;
	q->size = 0;
	return 0L;
}
