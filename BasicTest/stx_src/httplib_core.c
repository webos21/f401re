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

#include "httplib.h"
#include "http_parser.h"
#include "csb.h"
#include "ringbuf.h"

#ifdef SVC_EXF
#include "svc_exf.h"
#endif
#include "svc_socket.h"

#include "stx_crc32.h"
#include "stx_watchdog.h"
#include "stx_util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ----------------------------------------------
 * Internal Structures
 * ---------------------------------------------- */

#define HTTPLIB_DEBUG    (1 != 1)

#if HTTPLIB_DEBUG
#define HTTPLIB_LOG               util_print
#else /* !HTTPLIB_DEBUG */
#define HTTPLIB_LOG(...)          do {}while(0)
#endif /* HTTPLIB_DEBUG */

#define CS_BUFFER_SIZE   (8 * 1024)
#define RB_BUFFER_SIZE   1024
#define IO_BUFFER_SIZE   256

struct s_hl_t {
	hl_url_t url;

	/*@unique@*/
	csb_t *csb;

	rb_t rb;

	int32_t fd;
	int32_t method;
	int32_t sf_cmd;
	int32_t sf_header;
	int32_t sf_body;
	int32_t rflag;
	int32_t header_len;
	hl_entity_t header_list[16];
	int32_t content_len;
	char_t *content_body;

	int32_t ilen;
	int32_t wlen;
	int32_t waddr;

	hl_response_t *res;

	http_parser_settings settings;
	http_parser parser;

	int32_t parser_msg_done;
	int32_t parser_last_was_value;

};

static hl_t g_hl;
static char_t g_csb_buf[CS_BUFFER_SIZE];
static char_t g_ringbuf[RB_BUFFER_SIZE];

/* ----------------------------------------------
 * Internal Functions
 * ---------------------------------------------- */

/* Forward Declaration : Begin */
static int32_t hl_parser_on_message_begin(http_parser* hp);
static int32_t hl_parser_on_message_complete(http_parser* hp);
static int32_t hl_parser_on_header_field(http_parser* hp, const char_t *at,
		uint32_t length);
static int32_t hl_parser_on_header_value(http_parser* hp, const char_t *at,
		uint32_t length);
static int32_t hl_parser_on_headers_complete(http_parser* hp);
static int32_t hl_parser_on_body(http_parser* hp, const char_t *at,
		uint32_t length);
static void hl_socket_close(hl_t *handle);
static int32_t hl_parser_init(hl_t *handle);
static int32_t hl_parse_data(hl_t *handle, char_t *buf, int32_t buflen);
static int32_t hl_send_request(hl_t *handle);
static int32_t hl_recv_done(hl_t *handle);
static int32_t hl_recv_response(hl_t *handle, hl_response_t *res);
/* Forward Declaration : End */

static int32_t hl_parser_on_message_begin(http_parser* hp) {
	hl_t *handle = hp->data;

	HTTPLIB_LOG("hl_parser_on_message_begin\r\n");

	handle->res->status = hp->status_code;

	return 0;
}

static int32_t hl_parser_on_message_complete(http_parser* hp) {
	char_t buf[IO_BUFFER_SIZE];
	hl_t *handle = hp->data;

	HTTPLIB_LOG("hl_parser_on_message_complete\r\n");

	if (handle->waddr != 0) {
		while (rb_used(&handle->rb) > IO_BUFFER_SIZE) {
			rb_pop(&handle->rb, buf, IO_BUFFER_SIZE);

			HTTPLIB_LOG("Write Address = 0x%08X / length = %d\r\n",
					(handle->waddr + handle->wlen), IO_BUFFER_SIZE);
			util_hexdump("Remained-Data#1", buf, IO_BUFFER_SIZE);

			stx_crc32_calc(buf, IO_BUFFER_SIZE);

#ifdef SVC_EXF
			svc_exf_write_page((handle->waddr + handle->wlen), buf);
#endif
			handle->wlen += IO_BUFFER_SIZE;

			stx_watchdog_ok();
		}

		if (rb_used(&handle->rb) > 0) {
			int32_t final_write_size = rb_used(&handle->rb);
			rb_pop(&handle->rb, buf, final_write_size);

			HTTPLIB_LOG("Write Address = 0x%08X / length = %d\r\n",
					(handle->waddr + handle->wlen), final_write_size);
			util_hexdump("Remained-Data#2", buf, final_write_size);

			stx_crc32_calc(buf, final_write_size);

#ifdef SVC_EXF
			svc_exf_write_buf((handle->waddr + handle->wlen), buf,
					final_write_size);
#endif
			handle->wlen += final_write_size;

			stx_watchdog_ok();
		}

		handle->res->content_body = (char_t *) handle->waddr;
		handle->res->crc32 = stx_crc32_close();
	} else {
		csb_info_t ci;
		csb_get_info(handle->csb, &ci);
		handle->res->content_body = (ci.cptr - ci.wlen);

		stx_watchdog_ok();
	}

	handle->res->status = hp->status_code;
	handle->res->content_len = handle->ilen;
	handle->parser_msg_done = 1;

	return 0;
}

static int32_t hl_parser_on_header_field(http_parser* hp, const char_t *at,
		uint32_t length) {
	int32_t pos;
	hl_entity_t *hE;
	hl_t *handle = hp->data;

	if (handle->res->header_count < HTTP_HEADER_MAX) {

		if (handle->parser_last_was_value) {
			handle->res->header_count++;

			pos = handle->res->header_count;
			hE = &handle->res->headers[pos];

			memcpy(hE->key, at, length);
		} else {
			pos = handle->res->header_count;
			hE = &handle->res->headers[pos];

			pos = strlen(hE->key);
			memcpy(hE->key + pos, at, length);
		}

		handle->parser_last_was_value = 0;
	}

	return 0;
}

static int32_t hl_parser_on_header_value(http_parser* hp, const char_t *at,
		uint32_t length) {
	int32_t pos;
	hl_entity_t *hE;
	hl_t *handle = hp->data;

	if (handle->res->header_count < HTTP_HEADER_MAX) {
		pos = handle->res->header_count;
		hE = &handle->res->headers[pos];
		pos = (handle->parser_last_was_value) ? strlen(hE->value) : 0;
		memcpy(hE->value + pos, at, length);

		handle->parser_last_was_value = 1;
	}

	return 0;
}

static int32_t hl_parser_on_headers_complete(http_parser* hp) {
	hl_t *handle = hp->data;

	HTTPLIB_LOG("hl_parser_on_headers_complete\r\n");

	memset(handle->res->protocol, 0, sizeof(handle->res->protocol));
	(void) snprintf(handle->res->protocol, sizeof(handle->res->protocol),
			"HTTP/%d.%d", hp->http_major, hp->http_minor);
	handle->res->status = hp->status_code;

	HTTPLIB_LOG("Content-Length: %d\r\n", (int ) hp->content_length);

	handle->content_len = hp->content_length;

	return 0;
}

static int32_t hl_parser_on_body(http_parser* hp, const char_t *at,
		uint32_t length) {
	char_t buf[IO_BUFFER_SIZE];
	hl_t *handle = hp->data;

	handle->ilen += length;

	if (handle->waddr != 0) {
		rb_push(&handle->rb, at, length);

		while (rb_used(&handle->rb) > IO_BUFFER_SIZE) {
			rb_pop(&handle->rb, buf, IO_BUFFER_SIZE);

			stx_crc32_calc(buf, IO_BUFFER_SIZE);

#ifdef SVC_EXF
			svc_exf_write_page((handle->waddr + handle->wlen), buf);
#endif
			handle->wlen += IO_BUFFER_SIZE;

			stx_watchdog_ok();
		}
	} else {
		csb_append(handle->csb, at, length);
		stx_watchdog_ok();
	}

	return 0;
}

static void hl_socket_close(hl_t *handle) {
	if ((handle != NULL) && (handle->fd != -1)) {
		HTTPLIB_LOG("svc_socket_close()!!!!!!!\r\n");
		svc_socket_close(handle->fd);
		handle->fd = -1;
	}
}

static int32_t hl_parser_init(hl_t *handle) {
	int32_t rc = -1L;

	if (handle == NULL) {
		util_err1("Invalid arguments : handle = %p\r\n", handle);
	} else {
		memset(&handle->settings, 0, sizeof(handle->settings));

		handle->settings.on_message_begin = &hl_parser_on_message_begin;
		handle->settings.on_url = NULL;
		handle->settings.on_header_field = &hl_parser_on_header_field;
		handle->settings.on_header_value = &hl_parser_on_header_value;
		handle->settings.on_headers_complete = &hl_parser_on_headers_complete;
		handle->settings.on_body = &hl_parser_on_body;
		handle->settings.on_message_complete = &hl_parser_on_message_complete;

		http_parser_init(&handle->parser, HTTP_RESPONSE);
		handle->parser.data = handle;

		rc = 0L;
	}

	return rc;
}

static int32_t hl_parse_data(hl_t *handle, char_t *buf, int32_t buflen) {
	int32_t rc = -1L;

	if (handle == NULL) {
		util_err1("Invalid arguments : handle = %p\r\n", handle);
	} else {
		rc = http_parser_execute(&handle->parser, &handle->settings, buf,
				buflen);
		if ((rc == 1) && (buflen != 1)) {
			util_err1("http-parser error (ret = %d)!\r\n", handle->parser.http_errno);
			rc = -1L;
		}
	}

	return rc;
}

static int32_t hl_send_request(hl_t *handle) {
	int32_t rc = -1L;

	int32_t wbyte;
	char_t *sptr;

	if (handle == NULL) {
		util_err1("Invalid arguments : handle = %p\r\n", handle);
	} else {
		sptr = csb_tostring(handle->csb, &rc);
		if (sptr == NULL) {
			util_err1("Cannot get the send data (dlen=%d)\r\n", rc);
			rc = -1L;
		} else {
			HTTPLIB_LOG("Send - %d)\r\n%s\r\n", rc, sptr);

			wbyte = svc_socket_send(handle->fd, sptr, rc);
			if ((wbyte < 0) || (wbyte != rc)) {
				util_err1("Cannot get the send data (ret=%d)\r\n", wbyte);
				rc = -1L;
			} else {
				rc = wbyte;
			}
		}
	}

	return rc;
}

static int32_t hl_recv_done(hl_t *handle) {
	return (handle->parser_msg_done == 1) ?
			1 : http_body_is_final(&handle->parser);
}

static int32_t hl_recv_response(hl_t *handle, hl_response_t *res) {
	int32_t rc = -1L;

	int32_t rbyte;
	int32_t tlen;

	char_t rbuf[IO_BUFFER_SIZE];

	if (handle == NULL) {
		util_err1("Invalid arguments : handle = %p\r\n", handle);
	} else {
		rc = csb_reset(handle->csb);
		if (rc < 0) {
			util_err1("Cannot reset the CSB (ret=%d)\r\n", rc);
		} else {
			rc = hl_parser_init(handle);
			if (rc < 0) {
				util_err0("Cannot init the http-parser!\r\n");
			} else {
				memset(res, 0, sizeof(hl_response_t));
				handle->res = res;

				tlen = 0;
				rc = 0;

				while ((hl_recv_done(handle) == 0) && (rc >= 0)) {
					memset(rbuf, 0, sizeof(rbuf));
					rbyte = svc_socket_recv(handle->fd, rbuf, IO_BUFFER_SIZE);
					if (rbyte < 0) {
						util_err1(
								"Cannot receive the response data(ret = %d)\r\n",
								rbyte);
						hl_socket_close(handle);
						rc = tlen;
					} else if (rbyte == 0) {
						handle->rflag++;
						util_msleep(30);
						stx_watchdog_ok();
						if (handle->rflag > 100) {
							util_err1("Receive timeout!!!! (recv-size: %d)\r\n",
									handle->ilen);
							rc = -1L;
						}
					} else {
						tlen += rbyte;

						stx_watchdog_ok();

						handle->rflag = 1;
						rc = hl_parse_data(handle, rbuf, rbyte);
						if (rc < 0) {
							util_err0("Cannot parse the response data\r\n");
						}
					}
				}

				hl_socket_close(handle);

				if (rc == 0) {
					rc = tlen;
				}
			}
		}
	}

	return rc;
}

/* ----------------------------------------------
 * Interface Implementations
 * ---------------------------------------------- */

hl_t *hl_open(void) {
	csb_t *csb;
	hl_t *ret = NULL;

	csb = csb_init(g_csb_buf, CS_BUFFER_SIZE);
	if (csb == NULL) {
		util_err0("Cannot allocation the send buffer!!\r\n");
	} else {
		ret = &g_hl;
		memset(ret, 0, sizeof(hl_t));
		ret->csb = csb;
		rb_open(&ret->rb, g_ringbuf, RB_BUFFER_SIZE);
		ret->fd = -1;
	}

	return ret;
}

int32_t hl_set_req_url(hl_t *handle, const char_t *url) {
	int32_t rc = -1L;

	if ((NULL == handle) || (NULL == url)) {
		util_err2("Invalid arguments : handle = %p / url = %s\r\n", handle,
				url);
	} else {
		rc = hl_url_parse(url, &handle->url);
	}

	return rc;
}

int32_t hl_set_method(hl_t *handle, hl_method_e method) {
	int32_t rc = -1L;

	if (NULL == handle) {
		util_err1("Invalid arguments : handle = %p\r\n", handle);
	} else {
		handle->method = method;
		rc = 0L;
	}

	return rc;
}

int32_t hl_set_header(hl_t *handle, char_t *key, char_t *value) {
	int32_t rc = -1L;

	hl_entity_t *he;

	if ((NULL == handle) || (NULL == key) || (NULL == value)) {
		util_err3("Invalid arguments : handle = %p / key = %s / value = %s\r\n",
				handle, key, value);
	} else {
		he = &handle->header_list[handle->header_len];
		strcpy(he->key, key);
		strcpy(he->value, value);
		handle->header_len++;

		rc = handle->header_len;
	}

	return rc;
}

int32_t hl_set_body(hl_t *handle, char_t *rawdata, uint32_t rdlen) {
	int32_t rc = -1L;

	if (handle == NULL) {
		util_err1("Invalid arguments : handle = %p\r\n", handle);
	} else {
		handle->content_len = rdlen;
		handle->content_body = rawdata;

		rc = rdlen;
	}

	return rc;
}

int32_t hl_connect(hl_t *handle, uint16_t timeout) {
	int32_t rc = -1L;

	if (handle == NULL) {
		util_err2("Invalid arguments : handle = %p, timeout = %d\r\n", handle,
				timeout);
	} else {
		int32_t hostLen = strlen(handle->url.host_ipv4);
		if ((handle->url.host_port <= 20) || (hostLen < 5)) {
			util_err2("Invalid connection info : ip = %s, port = %d\r\n",
					handle->url.host_ipv4, handle->url.host_port);
		} else {
			handle->fd = svc_socket_open();
			if (handle->fd < 0) {
				util_err1("Cannot open a socket! (ret = %d)\r\n", handle->fd);
			} else {
				HTTPLIB_LOG("svc_socket_open()  : fd = %d\r\n", handle->fd);
				HTTPLIB_LOG("svc_socket_connect(%d, %s, %d, %d)\r\n",
						handle->fd, handle->url.host_name,
						handle->url.host_port, timeout);

				rc = svc_socket_connect(handle->fd, handle->url.host_name,
						handle->url.host_port, timeout);
				if (rc < 0) {
					util_err3("Cannot connect to %s:%d (ret = %d)\r\n",
							handle->url.host_ipv4, handle->url.host_port, rc);
					hl_socket_close(handle);
				}
			}
		}
	}

	return rc;
}

int32_t hl_write_cmd(hl_t *handle) {
	int32_t rc = -1L;
	const char_t *method;

	if (handle == NULL) {
		util_err1("Invalid arguments : handle = %p\r\n", handle);
	} else {
		if (handle->sf_cmd != 0) {
			util_err0("HTTP Command is already sent!!\r\n");
		} else {
			handle->sf_cmd = 1;

			method = hl_get_method_string((hl_method_e) handle->method);

			if ((handle->url.host_port == 80)
					|| (handle->url.host_port == 443)) {
				rc = csb_sprintf(handle->csb,
						"%s %s%s HTTP/1.1\r\nHost: %s\r\n", method,
						handle->url.path, handle->url.query_string,
						handle->url.host_name);
			} else {
				rc = csb_sprintf(handle->csb,
						"%s %s%s HTTP/1.1\r\nHost: %s:%d\r\n", method,
						handle->url.path, handle->url.query_string,
						handle->url.host_name, handle->url.host_port);
			}
			if (rc < 0) {
				util_err1("Cannot send the HTTP Command!! (ret = %d)\r\n", rc);
				util_err3("  > method = %s / path = %s / host = %s\r\n", method,
						handle->url.path, handle->url.host_name);
			}
		}
	}

	return rc;
}

int32_t hl_write_header(hl_t *handle) {
	int32_t rc = -1L;
	int32_t i;
	int32_t wlen;

	if (handle == NULL) {
		util_err1("Invalid arguments : handle = %p\r\n", handle);
	} else {
		if (handle->sf_header != 0) {
			util_err0("HTTP Headers are already sent!!\r\n");
		} else {
			handle->sf_header = 1;
			wlen = 0;
			for (i = 0; i < handle->header_len; i++) {
				hl_entity_t *he = &handle->header_list[i];
				rc = csb_sprintf(handle->csb, "%s: %s\r\n", he->key, he->value);
				if (rc < 0) {
					util_err1("Cannot send the HTTP Headers!! (ret = %d)\r\n",
							rc);
					util_err2("  > key = %s / value = %s\r\n", he->key,
							he->value);
					break;
				} else {
					wlen += rc;
				}
			}
			if (rc == 0) {
				rc = wlen;
			}
		}
	}

	return rc;
}

int32_t hl_write_body(hl_t *handle) {
	int32_t rc = -1L;

	if (handle == NULL) {
		util_err1("Invalid arguments : handle = %p\r\n", handle);
	} else {
		if (handle->sf_body != 0) {
			util_err0("HTTP Body is already sent!!\r\n");
		} else {
			handle->sf_body = 1;

			if ((handle->content_len == 0)
					|| (handle->method == HTTP_METHOD_GET)) {
				rc = csb_sprintf(handle->csb, "\r\n");
			} else {
				rc = csb_sprintf(handle->csb, "Content-Length: %d\r\n\r\n",
						handle->content_len);
				if (rc < 0) {
					util_err1("Cannot send the Content-Length!! (ret = %d)\r\n",
							rc);
					util_err1("  > Content-Length = %d\r\n",
							handle->content_len);
				} else {
					if (handle->content_len > 0) {
						rc = csb_sprintf(handle->csb, "%s",
								handle->content_body);
						if (rc < 0) {
							util_err1(
									"Cannot send the Content-Body!! (ret = %d)\r\n",
									rc);
							util_err1("  > %s\r\n", handle->content_body);
						}
					}
				}
			}
		}
	}

	return rc;
}

int32_t hl_get_response(hl_t *handle, hl_response_t *res, void *waddr) {
	int32_t rc = -1L;

	if ((NULL == handle) || (NULL == res)) {
		util_err2("Invalid arguments : handle = %p / res = %p\r\n", handle,
				res);
	} else {
		if (handle->rflag != 0) {
			util_err0("HTTP Response is already received!!\r\n");
		} else {
			if (handle->fd < 0) {
				rc = hl_connect(handle, 5);
				if (rc < 0) {
					util_err1("Cannot connect (ret=%d)!!\r\n", rc);
				} else {
					if (waddr != NULL) {
						handle->waddr = (int) waddr;
#ifdef SVC_EXF
						svc_exf_erase_block(handle->waddr, 3);
#endif
						res->crc32 = 0;
						stx_crc32_open();
					} else {
						handle->waddr = 0;
						res->crc32 = 0;
					}

					if (handle->sf_cmd == 0) {
						rc = hl_write_cmd(handle);
					}
					if ((rc > 0) && (handle->sf_header == 0)) {
						rc = hl_write_header(handle);
					}
					if ((rc > 0) && (handle->sf_body == 0)) {
						rc = hl_write_body(handle);
					}
					if (rc > 0) {
						rc = hl_send_request(handle);
						if (rc > 0) {
							handle->rflag = 1;

							rc = hl_recv_response(handle, res);
							if (rc < 0) {
								util_err0("Cannot get the header data!!\r\n");
							}
						} else {
							util_err0("Cannot send and receive the data!!\r\n");
						}
					}
				}
			}
		}
	}

	return rc;
}

int32_t hl_reset(hl_t *handle) {
	int32_t rc = -1L;

	if (handle == NULL) {
		util_err1("Invalid arguments : handle = %p\r\n", handle);
	} else {
		memset(&handle->url, 0, sizeof(hl_url_t));
		handle->waddr = 0;
		handle->wlen = 0;
		handle->ilen = 0;

		rc = csb_reset(handle->csb);
		if (rc < 0) {
			util_err0("Cannot reset the send buffer!!\r\n");
			/* Keep Going... */
		}

		rc = rb_clear(&handle->rb);
		if (rc < 0) {
			util_err0("Cannot reset the ring buffer!!\r\n");
			/* Keep Going... */
		}

		hl_socket_close(handle);

		handle->method = -1;
		handle->sf_cmd = 0;
		handle->sf_header = 0;
		handle->sf_body = 0;
		handle->rflag = 0;
		handle->header_len = 0;
		memset(handle->header_list, 0, sizeof(handle->header_list));
		handle->content_len = 0;
		handle->content_body = NULL;

		handle->parser_msg_done = 0;
		handle->parser_last_was_value = 0;

		handle->res = NULL;
	}

	return 0;
}

void hl_close(hl_t *handle) {
	if (handle != NULL) {
		hl_socket_close(handle);
		if (handle->csb != NULL) {
			csb_destroy(handle->csb);
		}
		memset(handle, 0, sizeof(hl_t));
	}
}
