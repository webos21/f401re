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

#include "csb.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

/* ----------------------------------------------
 * Internal Structures
 * ---------------------------------------------- */

struct s_csb_t {
	csb_info_t info;
	char_t *buf;
};

/* ----------------------------------------------
 * Internal Functions
 * ---------------------------------------------- */

/* ----------------------------------------------
 * Interface Implementations
 * ---------------------------------------------- */

csb_t *csb_init(void *mem, uint32_t mem_size) {
	char_t *cmem = mem;
	csb_t *ret = (csb_t *) mem;

	memset(ret, 0, sizeof(csb_t));

	ret->info.size = mem_size - sizeof(csb_t);
	ret->info.wlen = 0U;
	ret->buf = &(cmem[sizeof(csb_t)]);
	ret->info.cptr = ret->buf;
	memset(ret->buf, 0, ret->info.size);

	return ret;
}

int32_t csb_get_info(csb_t *handle, csb_info_t *out) {
	int32_t rc = -1;
	if ((NULL != handle) && (NULL != out)) {
		memcpy(out, &handle->info, sizeof(csb_info_t));
		rc = (int32_t) handle->info.wlen;
	}
	return rc;
}

int32_t csb_append(csb_t *handle, const char_t *str, uint32_t slen) {
	int32_t rc = -1;
	if ((NULL != handle) && (NULL != str) && (slen > 0U)) {
		strncpy(handle->info.cptr, str, slen);

		handle->info.cptr += slen;
		handle->info.wlen += slen;

		rc = (int32_t) slen;
	}
	return rc;
}

int32_t csb_sprintf(csb_t *handle, const char_t *format, ...) {
	va_list va;
	int32_t addlen;
	int32_t rc = -1;

	if ((NULL != handle) && (NULL != format)) {
		va_start(va, format);
		addlen = vsnprintf(handle->info.cptr,
				(handle->info.size - handle->info.wlen), format, va);
		va_end(va);

		handle->info.cptr += addlen;
		handle->info.wlen += (uint32_t) addlen;

		rc = addlen;
	}

	return rc;
}

char_t *csb_tostring(csb_t *handle, /*@out@*/int32_t *out) {
	char_t *rc = NULL;
	if ((NULL != handle) && (NULL != out)) {
		(*out) = (int32_t) handle->info.wlen;
		rc = handle->buf;
	}
	return rc;
}

int32_t csb_reset(csb_t *handle) {
	int32_t rc = -1;
	if ((NULL != handle)) {

		memset(handle->buf, 0, handle->info.size);

		handle->info.wlen = 0U;
		handle->info.cptr = handle->buf;

		rc = (int32_t) handle->info.wlen;
	}
	return rc;
}

void csb_destroy(csb_t *handle) {
	if ((NULL != handle)) {
		if ((NULL != handle->buf)) {
			memset(handle->buf, 0, handle->info.size);
		}
		memset(handle, 0, sizeof(csb_t));
	}
}
