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

#include "svc_modem.h"

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

/*@observer@*/
static const char_t *g_methods[] = { "GET", "POST", "PUT", "DELETE", "OPTIONS",
		"HEAD" };

static const char_t HEX2DEC[256] = {
/* 0 1 2 3 4 5 6 7 8 9 A B C D E F */
/* 0 */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* 1 */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* 2 */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* 3 */0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1, -1,
/* 4 */-1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* 5 */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* 6 */-1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* 7 */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* 8 */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* 9 */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* A */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* B */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* C */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* D */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* E */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* F */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const char_t SAFE[256] = {
/* -- 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F */
/* 0 */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 1 */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 2 */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 3 */1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
/* 4 */0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* 5 */1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
/* 6 */0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* 7 */1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
/* 8 */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 9 */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* A */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* B */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* C */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* D */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* E */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* F */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

static const char_t RFC3986[256] = {
/* -- 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F */
/* 0 */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 1 */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 2 */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
/* 3 */1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
/* 4 */0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* 5 */1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
/* 6 */0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* 7 */1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
/* 8 */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 9 */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* A */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* B */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* C */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* D */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* E */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* F */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

static const char_t HTML5[256] = {
/* -- 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F */
/* 0 */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 1 */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 2 */1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
/* 3 */1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
/* 4 */0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* 5 */1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
/* 6 */0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* 7 */1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
/* 8 */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 9 */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* A */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* B */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* C */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* D */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* E */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* F */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

static const char_t DEC2HEX[16 + 1] = "0123456789ABCDEF";

/* if you want to query the dns, change the value to 1 */
#define MODEM_TECHPLEX_DNS    0

#if MODEM_TECHPLEX_DNS
typedef struct _st_dnsq_t {
	uint16_t tid; /* Transaction ID */
	uint16_t flags; /* Standard Query : 0x0100 */
	uint16_t qlen; /* Normally 1 Entry */
	uint16_t rr_answer; /* Answer RRs : 0 */
	uint16_t rr_auth; /* Authority RRs : 0 */
	uint16_t rr_add; /* Additional RRs : 0 */
	char_t name[64];
	uint16_t qtype; /* HOST : 0x01 */
	uint16_t qclass; /* INET : 0x01 */
}dnsq_t;

typedef struct _st_dnsr_t {
	uint16_t tid; /* Transaction ID */
	uint16_t flags; /* Standard Query : 0x0100 */
	uint16_t qlen; /* Normally 1 Entry */
	uint16_t rr_answer; /* Answer RRs : 0 */
	uint16_t rr_auth; /* Authority RRs : 0 */
	uint16_t rr_add; /* Additional RRs : 0 */
	char_t name[64];
	uint16_t qtype; /* HOST : 0x01 */
	uint16_t qclass; /* INET : 0x01 */

}dnsr_t;

static uint16_t _g_tid = 0;

#endif /* MODEM_TECHPLEX_DNS */

/* ----------------------------------------------
 * Internal Functions
 * ---------------------------------------------- */

/* ----------------------------------------------
 * Interface Implementations
 * ---------------------------------------------- */

int32_t hl_url_parse(const char_t *url, hl_url_t *out) {
	char_t *cptr;
	char_t *rptr;
	char_t buf[64];

	int32_t rc = -1L;

	if ((NULL != url) && (NULL != out)) {
		memset(out, 0, sizeof(hl_url_t));

		cptr = strstr(url, "://");
		if ((NULL == cptr)) {
			util_err1("cannot find protocol : %s\n", url);
		} else {
			strncpy(out->protocol, url, (cptr - url));
			cptr += 3; /* skip ":\/\/" */

			out->host_port = hl_port_by_protocol(out->protocol);
			if ((0xFFFF == out->host_port)) {
				util_err1("invalid protocol : %s\n", out->protocol);
			} else {
				rptr = strchr(cptr, '/');
				if (NULL == rptr) {
					util_err1("cannot find (host / port) : %s\n", url);
				} else {
					memset(buf, 0, sizeof(buf));
					strncpy(buf, cptr, rptr - cptr);

					cptr = rptr;
					rptr = strchr(buf, ':');
					if (NULL == rptr) {
						strcpy(out->host_name, buf);
					} else {
						strncpy(out->host_name, buf, (rptr - buf));
						rptr++; /* skip ':' */
						out->host_port = strtol(rptr, NULL, 10);
					}

					rptr = strchr(cptr, '?');
					if (NULL == rptr) {
						(void) snprintf(out->path, sizeof(out->path), "%s",
								cptr);
					} else {
						strncpy(out->path, cptr, rptr - cptr);
						cptr = rptr;
						rptr = strchr(cptr, '#');
						if (NULL == rptr) {
							strcpy(out->query_string, cptr);
						} else {
							strncpy(out->query_string, cptr, rptr - cptr);
						}
					}

					rc = hl_hostname_to_ipv4(out->host_name, out->host_ipv4,
							sizeof(out->host_ipv4));
					if (rc < 0) {
						util_err1("Invalid Host : %s\n", out->host_name);
						rc = -1L;
					} else {
						HTTPLIB_LOG("#################################\r\n");
						HTTPLIB_LOG("URL = %s\r\n", url);
						HTTPLIB_LOG("#################################\r\n");
						HTTPLIB_LOG("URL > Protocol     = %s\r\n",
								out->protocol);
						HTTPLIB_LOG("URL > Host         = %s\r\n",
								out->host_name);
						HTTPLIB_LOG("URL > IP           = %s\r\n",
								out->host_ipv4);
						HTTPLIB_LOG("URL > Port         = %d\r\n",
								out->host_port);
						HTTPLIB_LOG("URL > Path         = %s\r\n", out->path);
						HTTPLIB_LOG("URL > Query-String = %s\r\n",
								out->query_string);
					}
				}
			}
		}
	}

	return rc;
}

int32_t hl_url_encode(char_t *dst, uint32_t dstlen, char_t *src,
		hl_urlmode_e mode) {
	int32_t SRC_LEN;
	char_t *pSrc;

	char_t *pStart;
	char_t *pEnd;
	char_t *SRC_END;

	int32_t cnt;
	int32_t cmplen;

	const char_t *tbl;

	pSrc = src;
	SRC_LEN = strlen(src);
	pStart = dst;
	pEnd = pStart;
	SRC_END = pSrc + SRC_LEN;

	cnt = 0;
	cmplen = dstlen - 1;

	switch (mode) {
	case HL_URL_RFC3986:
		tbl = RFC3986;
		break;
	case HL_URL_HTML5:
		tbl = HTML5;
		break;
	case HL_URL_SAFE:
	default:
		tbl = SAFE;
		break;
	}

	for (cnt = 0; (cnt < cmplen) && (pSrc < SRC_END); ++pSrc) {
		uint32_t tmp = (*pSrc);
		if (tbl[(tmp & 0xFF)] != 0) {
			*pEnd++ = ((mode == HL_URL_HTML5) && (tmp == ' ')) ? '+' : tmp;
			cnt++;
		} else {
			if (cnt + 3 >= cmplen) {
				break;
			}
			/* escape this char */
			*pEnd++ = '%';
			*pEnd++ = DEC2HEX[(tmp >> 4) & 0x0F];
			*pEnd++ = DEC2HEX[tmp & 0x0F];
			cnt += 3;
		}
	}

	return cnt;
}

int32_t hl_url_decode(char_t *dst, uint32_t dstlen, char_t *src) {
	char_t c;
	uint8_t v1;
	uint8_t v2;

	char_t *out;
	char_t *in;
	char_t *beg;

	int32_t cnt;
	int32_t cmplen;

	uint32_t tmp;

	out = dst;
	in = src;

	beg = out;

	cnt = 0;
	cmplen = dstlen - 1;

	c = *in++;
	while ((cnt < cmplen) && (c != '\0')) {
		if (c == '+') {
			c = ' ';
		}
		if (c == '%') {
			v1 = *in++;
			v2 = *in++;

			tmp = (HEX2DEC[v1]);

			if ((v1 == 0) || ((tmp & 0xFF) == 0xFF) || (v2 == 0)
					|| (HEX2DEC[v2] == 0xFF)) {
				*beg = '\0';
				cnt = -1;
				break;
			}

			tmp = (tmp << 4);
			v1 = (tmp & 0xF0);
			v2 = (HEX2DEC[v2] & 0x0F);

			c = (v1 | v2);
		}
		cnt++;
		*out++ = c;
		c = *in++;
	}

	*out = '\0';

	return cnt;
}

void hl_print_response(hl_response_t *res) {
	int32_t i;

	util_print("[STATUS] --------------------------\r\n");
	util_print("HTTP Protocol       : %s\r\n", res->protocol);
	util_print("HTTP Status Code    : %d\r\n", res->status);
	util_print("HTTP Status Message : %s\r\n", res->status_msg);
	util_print("[HEADERS : %d] -------------------------\r\n",
			res->header_count);
	for (i = 0; i < res->header_count; i++) {
		util_print("%s: %s\r\n", res->headers[i].key, res->headers[i].value);
	}
	util_print("[BODY : %d] -----------------------\r\n", res->content_len);
	if (res->crc32 == 0) {
		util_print("%s\r\n", res->content_body);
	}
}

const char_t *hl_get_method_string(hl_method_e method) {
	return g_methods[(int32_t) method];
}

int32_t hl_port_by_protocol(const char_t *protocol) {
	int32_t rc = -1;

	if (protocol != NULL) {
		if (strcmp("ftp", protocol) == 0) {
			rc = 21;
		}
		if (strcmp("http", protocol) == 0) {
			rc = 80;
		}
		if (strcmp("https", protocol) == 0) {
			rc = 443;
		}
		if (strcmp("rtsp", protocol) == 0) {
			rc = 554;
		}
	}

	return rc;
}

int32_t hl_hostname_is_ipv4(char_t *hostname) {
	int32_t rc = 0L;
	char_t *p = hostname;

	if (hostname == NULL) {
		util_err1("Invalid arguments (hostname = %s)!!\r\n", hostname);
	} else {
		while (*p != '\0') {
			if ((*p != '.') && ((*p < '0') || (*p > '9'))) {
				rc = 1L;
				break;
			}
			p++;
		}
	}

	return rc;
}

#if MODEM_TELADIN

int32_t hl_hostname_to_ipv4(const char_t *hostname, char_t *out) {
	int32_t ret = 0;
	char_t dnsq[128];
	modem_req_t mreq;

	if (hostname == NULL || out == NULL) {
		util_err2("Invalid arguments (hostname = %s / out = %s)!!\r\n", hostname,
				out);
		return -1;
	}

	ret = hl_hostname_is_ipv4(hostname);
	if (ret) {
		strcpy(out, hostname);
		return 0;
	}

	if (svc_modem_state_get() < MODEM_S_READY) {
		util_err0("MODEM is not ready!!\r\n");
		return -1;
	}

	memset(dnsq, 0, sizeof(dnsq));
	strcpy(dnsq, "AT$$TCP_DNIP=");
	strcat(dnsq, hostname);
	strcat(dnsq, "\r\n");

	svc_modem_init_req(&mreq, 1, 3, dnsq, "$$TCP_DNIP:", NULL);
	ret = svc_modem_send_req(&mreq);
	if (ret == 0 && mreq.retLen > 12 && mreq.ret[12] == '0') {
		strncpy(out, &mreq.ret[15], (mreq.retLen - 15 - 2)); /* postion & (CR & LF) */
		return 0;
	}

	return -1;
}

#else /* MODEM_TELIT */

int32_t hl_hostname_to_ipv4(const char_t *hostname, /*@unique@*/char_t *out,
		int32_t olen) {
	int32_t rc = -1L;

	if ((NULL == hostname) || (NULL == out)) {
		util_err2("Invalid arguments (hostname = %s / out = %s)!!\r\n",
				hostname, out);
	} else {
		strncpy(out, hostname, olen);
		rc = 0L;
	}

	return rc;
}

#endif /* MODEM_TELADIN */
