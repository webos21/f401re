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

#include "svc_http.h"

#include "httplib.h"

#include "stx_watchdog.h"
#include "stx_util.h"

#include <string.h>

/* ----------------------------------------------
 * Internal Structures
 * ---------------------------------------------- */

#define SVC_HTTP_DEBUG    (1 != 1)

#if SVC_HTTP_DEBUG
#define SVC_HTTP_LOG               util_print
#else /* !SVC_HTTP_DEBUG */
#define SVC_HTTP_LOG(...)          do {}while(0)
#endif /* SVC_HTTP_DEBUG */

/* ----------------------------------------------
 * Internal Functions
 * ---------------------------------------------- */

/* ----------------------------------------------
 * Interface Implementations
 * ---------------------------------------------- */

int32_t svc_http_post(hl_t *handle, hl_response_t *res, char_t *url,
		char_t *data, uint32_t dlen) {
	int32_t rc = 0L;

	if (rc >= 0) {
		SVC_HTTP_LOG("1) hl_reset\r\n");
		rc = hl_reset(handle);
		if (rc < 0) {
			util_err0("Cannot reset the HTTP Handle!!\r\n");
		}
	}

	if (rc >= 0) {
		SVC_HTTP_LOG("2) hl_set_req_url\r\n");
		rc = hl_set_req_url(handle, url);
		if (rc < 0) {
			util_err0("Cannot set the HTTP URL!!\r\n");
		}
	}

	if (rc >= 0) {
		SVC_HTTP_LOG("3) hl_set_method\r\n");
		rc = hl_set_method(handle, HTTP_METHOD_POST);
		if (rc < 0) {
			util_err0("Cannot set the HTTP Method!!\r\n");
		}
	}

	if (rc >= 0) {
		SVC_HTTP_LOG("4) hl_set_header\r\n");
		rc = hl_set_header(handle, "Content-Type", "application/json");
		if (rc < 0) {
			util_err0(
					"Cannot set the Header (Content-Type: application/json)!!\r\n");
		}
		rc = hl_set_header(handle, "Cache-Control", "no-cache");
		if (rc < 0) {
			util_err0("Cannot set the Header (Cache-Control: no-cache)!!\r\n");
		}
	}

	if (rc >= 0) {
		rc = hl_set_header(handle, "Connection", "close");
		if (rc < 0) {
			util_err0("Cannot set the Header (Connection: closed)!!\r\n");
		}
	}

	if (rc >= 0) {
		SVC_HTTP_LOG("5) hl_set_body\r\n");
		rc = hl_set_body(handle, data, dlen);
		if (rc < 0) {
			util_err0("Cannot set the HTTP Body!!\r\n");
		}
	}

	stx_watchdog_ok();

	if (rc >= 0) {
		SVC_HTTP_LOG("6) hl_get_response\r\n");
		rc = hl_get_response(handle, res, NULL);
		if (rc < 0) {
			util_err0("Cannot get the HTTP Response!!\r\n");
		}
	}

	stx_watchdog_ok();

	if (SVC_HTTP_DEBUG && (rc >= 0)) {
		SVC_HTTP_LOG("7) hl_print_response\r\n");
		hl_print_response(res);
	}

	return ((rc >= 0) ? (int32_t) res->content_len : rc);
}

int32_t svc_http_info(hl_t *handle, hl_response_t *res, char_t *url) {
	int32_t rc = 0L;

	if (rc >= 0) {
		SVC_HTTP_LOG("1) hl_reset\r\n");
		rc = hl_reset(handle);
		if (rc < 0) {
			util_err0("Cannot reset the HTTP Handle!!\r\n");
		}
	}

	if (rc >= 0) {
		SVC_HTTP_LOG("2) hl_set_req_url\r\n");
		rc = hl_set_req_url(handle, url);
		if (rc < 0) {
			util_err0("Cannot set the HTTP URL!!\r\n");
		}
	}

	if (rc >= 0) {
		SVC_HTTP_LOG("3) hl_set_method\r\n");
		rc = hl_set_method(handle, HTTP_METHOD_GET);
		if (rc < 0) {
			util_err0("Cannot set the HTTP Method!!\r\n");
		}
	}

	if (rc >= 0) {
		SVC_HTTP_LOG("4) hl_set_header\r\n");
		rc = hl_set_header(handle, "Cache-Control", "no-cache");
		if (rc < 0) {
			util_err0("Cannot set the Header (Cache-Control: no-cache)!!\r\n");
		}
		rc = hl_set_header(handle, "Connection", "close");
		if (rc < 0) {
			util_err0("Cannot set the Header (Connection: closed)!!\r\n");
		}
	}

	if (rc >= 0) {
		SVC_HTTP_LOG("5) hl_set_body\r\n");
		rc = hl_set_body(handle, "", 0);
		if (rc < 0) {
			util_err0("Cannot set the HTTP Body!!\r\n");
		}
	}

	stx_watchdog_ok();

	if (rc >= 0) {
		SVC_HTTP_LOG("6) hl_get_response\r\n");
		rc = hl_get_response(handle, res, NULL);
		if (rc < 0) {
			util_err0("Cannot get the HTTP Response!!\r\n");
		}
	}

	stx_watchdog_ok();

	if (SVC_HTTP_DEBUG && (rc >= 0)) {
		SVC_HTTP_LOG("7) hl_print_response\r\n");
		hl_print_response(res);
	}

	return ((rc >= 0) ? (int32_t) res->content_len : rc);
}

int32_t svc_http_down(hl_t *handle, hl_response_t *res, char_t *url,
		uint32_t storage) {
	int32_t rc = 0;

	if (rc >= 0) {
		SVC_HTTP_LOG("1) hl_reset\r\n");
		rc = hl_reset(handle);
		if (rc < 0) {
			util_err0("Cannot reset the HTTP Handle!!\r\n");
		}
	}

	if (rc >= 0) {
		SVC_HTTP_LOG("2) hl_set_req_url\r\n");
		rc = hl_set_req_url(handle, url);
		if (rc < 0) {
			util_err0("Cannot set the HTTP URL!!\r\n");
		}
	}

	if (rc >= 0) {
		SVC_HTTP_LOG("3) hl_set_method\r\n");
		rc = hl_set_method(handle, HTTP_METHOD_GET);
		if (rc < 0) {
			util_err0("Cannot set the HTTP Method!!\r\n");
		}
	}

	if (rc >= 0) {
		SVC_HTTP_LOG("4) hl_set_header\r\n");
		rc = hl_set_header(handle, "Cache-Control", "no-cache");
		if (rc < 0) {
			util_err0("Cannot set the Header (Cache-Control: no-cache)!!\r\n");
		}
		rc = hl_set_header(handle, "Connection", "close");
		if (rc < 0) {
			util_err0("Cannot set the Header (Connection: closed)!!\r\n");
		}
	}

	if (rc >= 0) {
		SVC_HTTP_LOG("5) hl_set_body\r\n");
		rc = hl_set_body(handle, "", 0);
		if (rc < 0) {
			util_err0("Cannot set the HTTP Body!!\r\n");
		}
	}

	stx_watchdog_ok();

	if (rc >= 0) {
		SVC_HTTP_LOG("6) hl_get_response\r\n");
		rc = hl_get_response(handle, res, (void *) storage);
		if (rc < 0) {
			util_err0("Cannot get the HTTP Response!!\r\n");
		}
	}

	stx_watchdog_ok();

	if (SVC_HTTP_DEBUG && (rc >= 0)) {
		SVC_HTTP_LOG("7) hl_print_response\r\n");
		hl_print_response(res);
	}

	return ((rc >= 0) ? (int32_t) res->content_len : rc);
}
