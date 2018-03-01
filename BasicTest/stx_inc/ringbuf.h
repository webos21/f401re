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

#ifndef RINGBUF_H
#define RINGBUF_H

/* -----------------------------------------
 * The Ring Buffer
 * ----------------------------------------- */

#include "misra.h"

/*
 * Start C++ Declaration
 */
#ifdef __cplusplus
extern "C" {
#endif

typedef struct s_rb_t {
	__IO int32_t capacity;
	__IO int32_t used;
	void *head;
	void *tail;
	void *buf;
} rb_t;

int32_t /*@alt void@*/ rb_open(rb_t *handle, void *buf, int32_t blen);
int32_t rb_capacity(const rb_t *handle);
int32_t rb_remain(const rb_t *handle);
int32_t rb_used(const rb_t *handle);
int32_t /*@alt void@*/ rb_push(rb_t *handle, const void *wdat, int32_t wlen);
int32_t /*@alt void@*/ rb_pop(rb_t *handle, void *rdat, int32_t rlen);
int32_t rb_peek(const rb_t *handle, void *rdat, int32_t rlen);
int32_t rb_clear(rb_t *handle);
int32_t rb_close(rb_t *handle);

/*
 * End C++ Declaration
 */
#ifdef __cplusplus
}
#endif

#endif /* RINGBUF_H */
