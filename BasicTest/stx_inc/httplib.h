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

#ifndef HTTPLIB_H
#define HTTPLIB_H

/* -----------------------------------------
 * The HTTP Library
 * ----------------------------------------- */

#include "misra.h"

/*
 * Start C++ Declaration
 */
#ifdef __cplusplus
extern "C" {
#endif

#define HTTP_HEADER_MAX 16

typedef struct s_hl_t hl_t;

typedef enum e_hl_method_e {
	HTTP_METHOD_GET      = 0,
	HTTP_METHOD_POST     = 1,
	HTTP_METHOD_PUT      = 2,
	HTTP_METHOD_DELETE   = 3,
	HTTP_METHOD_OPTION   = 4,
	HTTP_METHOD_HEAD     = 5
} hl_method_e;

typedef enum e_hl_urlmode_e {
	HL_URL_SAFE          = 0,
	HL_URL_RFC3986       = 1,
	HL_URL_HTML5         = 2
} hl_urlmode_e;

typedef struct s_hl_entity_t {
	/*@unique@*/
	char_t key[64];
	/*@unique@*/
	char_t value[128];
} hl_entity_t;

typedef struct s_hl_url_t {
	/*@unique@*/
	char_t    protocol[16];
	/*@unique@*/
	char_t    host_name[256];
	/*@unique@*/
	char_t    host_ipv4[64];
	uint16_t  host_port;
	/*@unique@*/
	char_t    path[256];
	/*@unique@*/
	char_t    query_string[1024];
} hl_url_t;

typedef struct s_hl_response_t {
	char_t       protocol[16];
	int32_t      status;
	char_t       status_msg[8];
	uint16_t     header_count;
	hl_entity_t  headers[HTTP_HEADER_MAX];
	uint32_t     content_len;
	uint32_t     crc32;
	char_t      *content_body;
} hl_response_t;

hl_t *hl_open(void);

int32_t   hl_set_req_url(hl_t *handle, const char_t *url);
int32_t   hl_set_method(hl_t *handle, hl_method_e method);
int32_t   hl_set_header(hl_t *handle, char_t *key, char_t *value);
int32_t   hl_set_body(hl_t *handle, char_t *rawdata, uint32_t rdlen);

int32_t   hl_connect(hl_t *handle, uint16_t timeout); /* timeout is seconds */

int32_t   hl_write_cmd(hl_t *handle);
int32_t   hl_write_header(hl_t *handle);
int32_t   hl_write_body(hl_t *handle);

int32_t   hl_get_response(hl_t *handle, hl_response_t *res, void *waddr);

int32_t   hl_reset(hl_t *handle);

void      hl_close(hl_t *handle);

/* --------------------------------------------------------
 * utility functions
 * -------------------------------------------------------- */

int32_t hl_url_parse(const char_t *url, hl_url_t *out);

int32_t hl_url_encode(char_t *dst, uint32_t dstlen, char_t *src, hl_urlmode_e mode);
int32_t hl_url_decode(char_t *dst, uint32_t dstlen, char_t *src);

void    hl_print_response(hl_response_t *res);

/*@exposed@*/
const char_t *hl_get_method_string(hl_method_e method);

int32_t       hl_port_by_protocol(const char_t *protocol);
int32_t       hl_hostname_is_ipv4(char_t *hostname);
int32_t       hl_hostname_to_ipv4(const char_t *hostname, /*@unique@*/char_t *out, int32_t olen);


/*
 * End C++ Declaration
 */
#ifdef __cplusplus
}
#endif

#endif /* HTTPLIB_H */
