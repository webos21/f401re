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

#include "ringbuf.h"

#include "stx_util.h"

#include <string.h>

/* ----------------------------------------------
 * Internal Structures
 * ---------------------------------------------- */

/* ----------------------------------------------
 * Interface Implementations
 * ---------------------------------------------- */

int32_t rb_open(rb_t *handle, void *buf, int32_t blen) {
	int32_t rc = -1L;

	if (handle != NULL) {
		handle->capacity = blen;
		handle->buf = buf;
		handle->head = handle->tail = handle->buf;

		rc = 0;
	} else {
		util_err0("Given Handle is NULL!!!\r\n");
	}

	return rc;
}

int32_t rb_capacity(const rb_t *handle) {
	int32_t rc = -1L;

	if ((handle != NULL)) {
		if (handle->capacity > 0L) {
			rc = handle->capacity;
		}
	} else {
		util_err0("Invalid Handle!!!\r\n");
	}

	return rc;
}

int32_t rb_remain(const rb_t *handle) {
	int32_t rc = -1L;

	if ((handle != NULL)) {
		int32_t cmpVal = handle->capacity;
		if (cmpVal > 0L) {
			rc = (cmpVal - handle->used);
		}
	} else {
		util_err0("Invalid Handle!!!\r\n");
	}

	return rc;
}

int32_t rb_used(const rb_t *handle) {
	int32_t rc = -1L;

	if ((handle != NULL)) {
		if (handle->capacity > 0L) {
			rc = handle->used;
		}
	} else {
		util_err0("Invalid Handle!!!\r\n");
	}

	return rc;
}

int32_t rb_push(rb_t *handle, const void *wdat, int32_t wlen) {
	int32_t rc = -1L;

	if ((handle != NULL)) {
		int32_t cmpVal = handle->capacity;
		if (cmpVal > 0L) {
			if ((handle->used + wlen) > cmpVal) {
				util_err0("Overflow!!\r\n");
				rc = -2L;
			} else {
				if ((((uint8_t *) handle->tail) + wlen)
						> (((uint8_t *) handle->buf) + handle->capacity)) {
					int32_t wrap = (((uint8_t *) handle->buf) + handle->capacity
							- ((uint8_t *) handle->tail));
					memcpy(handle->tail, wdat, wrap);
					memcpy(handle->buf, (((const uint8_t *) wdat) + wrap),
							(wlen - wrap));
					handle->tail = ((uint8_t *) handle->buf) + wlen - wrap;
				} else {
					memcpy(handle->tail, wdat, wlen);
					handle->tail = ((uint8_t *) handle->tail) + wlen;
				}
				handle->used += wlen;
				rc = 0L;
			}
		}
	} else {
		util_err0("Invalid Handle!!!\r\n");
	}

	return rc;
}

int32_t rb_pop(rb_t *handle, void *rdat, int32_t rlen) {
	int32_t rc = -1L;

	if ((handle != NULL)) {
		if (handle->capacity > 0L) {
			if (handle->used - rlen < 0) {
				util_err0("Underflow!!\r\n");
				rc = -2L;
			} else {
				if ((((uint8_t *) handle->head) + rlen)
						> (((uint8_t *) handle->buf) + handle->capacity)) {
					int32_t wrap = (((uint8_t *) handle->buf) + handle->capacity
							- ((uint8_t *) handle->head));
					memcpy(rdat, handle->head, wrap);
					memcpy((((uint8_t *) rdat) + wrap), handle->buf,
							(rlen - wrap));
					handle->head = ((uint8_t *) handle->buf) + rlen - wrap;
				} else {
					memcpy(rdat, handle->head, rlen);
					handle->head = ((uint8_t *) handle->head) + rlen;
				}
				handle->used -= rlen;
				rc = 0L;
			}
		}
	} else {
		util_err0("Invalid Handle!!!\r\n");
	}

	return rc;
}

int32_t rb_peek(const rb_t *handle, void *rdat, int32_t rlen) {
	int32_t rc = -1L;

	if ((handle != NULL)) {
		if (handle->capacity > 0L) {
			if (handle->used - rlen < 0) {
				util_err0("Underflow!!\r\n");
				rc = -2L;
			} else {
				if ((((uint8_t *) handle->head) + rlen)
						> (((uint8_t *) handle->buf) + handle->capacity)) {
					int32_t wrap = (((uint8_t *) handle->buf) + handle->capacity
							- ((uint8_t *) handle->tail));
					memcpy(rdat, handle->head, wrap);
					memcpy((((uint8_t *) rdat) + wrap), handle->buf,
							(rlen - wrap));
				} else {
					memcpy(rdat, handle->head, rlen);
				}
				rc = 0L;
			}
		}
	} else {
		util_err0("Invalid Handle!!!\r\n");
	}

	return rc;
}

int32_t rb_clear(rb_t *handle) {
	int32_t rc = -1L;

	if ((handle != NULL)) {
		if (handle->capacity > 0L) {
			handle->tail = handle->head = handle->buf;
			handle->used = 0L;
			rc = 0L;
		}
	} else {
		util_err0("Invalid Handle!!!\r\n");
	}

	return rc;
}

int32_t rb_close(rb_t *handle) {
	int32_t rc = -1L;

	if ((handle != NULL)) {
		if (handle->capacity > 0L) {
			memset(handle, 0, sizeof(rb_t));
			rc = 0L;
		}
	} else {
		util_err0("Invalid Handle!!!\r\n");
	}

	return rc;
}
