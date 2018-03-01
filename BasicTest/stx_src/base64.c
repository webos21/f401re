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

#include "base64.h"

/* ----------------------------------------------
 * Internal Structures
 * ---------------------------------------------- */

/**
 * Encode/Decode lookup table for the "Default".
 */
#if 0
static const char_t g_basis_64[] =
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const uint8_t g_b64_pr2six[256] = {64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	62, 64, 64, 64, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64,
	64, 64, 64, 64, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
	16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64, 64, 26, 27,
	28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45,
	46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64};
#endif

/**
 * Encode lookup table for the "web safe" variant (RFC 3548
 * sec. 4) where - and _ replace + and /.
 */
static const char_t g_basis_64[] =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

static const uint8_t g_b64_pr2six[256] = { 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		62, 64, 64, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64,
		64, 64, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
		18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 63, 64, 26, 27, 28, 29,
		30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
		48, 49, 50, 51, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64 };

/* ----------------------------------------------
 * Internal Functions
 * ---------------------------------------------- */

/* ----------------------------------------------
 * Interface Implementations
 * ---------------------------------------------- */

int32_t base64_encode_len(int32_t len) {
	return ((len + 2) / 3 * 4) + 1;
}

int32_t base64_encode(char_t *coded_dst, const void *plain_src,
		int32_t len_plain_src) {
	int32_t i;
	char_t *p;
	const uint8_t *psrc = plain_src;

	p = coded_dst;
	for (i = 0; i < len_plain_src - 2; i += 3) {
		*p++ = g_basis_64[((psrc[i] >> 2U) & 0x3FU)];
		*p++ = g_basis_64[(((psrc[i] & 0x03U) << 4U)
				| ((uint8_t) (psrc[i + 1] & 0xF0) >> 4U))];
		*p++ = g_basis_64[((psrc[i + 1] & 0x0FU) << 2U)
				| ((uint8_t) (psrc[i + 2] & 0xC0U) >> 6U)];
		*p++ = g_basis_64[psrc[i + 2] & 0x3FU];
	}
	if (i < len_plain_src) {
		*p++ = g_basis_64[(psrc[i] >> 2) & 0x3FU];
		if (i == (len_plain_src - 1)) {
			*p++ = g_basis_64[((psrc[i] & 0x03U) << 4)];
			*p++ = '=';
		} else {
			*p++ = g_basis_64[((psrc[i] & 0x03U) << 4)
					| ((uint8_t) (psrc[i + 1] & 0xF0U) >> 4)];
			*p++ = g_basis_64[((psrc[i + 1] & 0x0FU) << 2)];
		}
		*p++ = '=';
	}

	*p = '\0';
	return (p - coded_dst);
}

int32_t base64_encode_websafe(char_t *coded_dst, const void *plain_src,
		int32_t len_plain_src) {
	int32_t enclen;
	int32_t pos;

	enclen = base64_encode(coded_dst, plain_src, len_plain_src);
	pos = enclen - 1;
	while (coded_dst[pos] == '=') {
		coded_dst[pos] = '\0';
		pos--;
	}

	return pos;
}

int32_t base64_decode_len(const void * coded_src) {
	int32_t nbytesdecoded;
	register const uint8_t *bufin;
	register int32_t nprbytes;

	bufin = coded_src;
	while (g_b64_pr2six[*(bufin++)] <= 63) {
		/* Nothing to do */
	}

	nprbytes = (int32_t) ((bufin - (const uint8_t *) coded_src) - 1L);
	nbytesdecoded = ((nprbytes + 3) / 4) * 3;

	return nbytesdecoded + 1;
}

int32_t base64_decode(void *plain_dst, const void *coded_src) {
	int32_t len = base64_decode_binary(plain_dst, coded_src);
	((uint8_t *) plain_dst)[len] = '\0';
	return len;
}

int32_t base64_decode_websafe(void *plain_dst, const void *coded_src) {
	return base64_decode(plain_dst, coded_src);
}

int32_t base64_decode_binary(void *plain_dst, const void *coded_src) {
	int32_t nbytesdecoded;
	register const uint8_t *bufin;
	register uint8_t *bufout;
	register int32_t nprbytes;

	bufin = coded_src;
	while (g_b64_pr2six[*(bufin++)] <= 63) {
		/* do nothing */
	}

	nprbytes = (int32_t) ((bufin - (const uint8_t *) coded_src) - 1L);
	nbytesdecoded = ((nprbytes + 3) / 4) * 3;

	bufout = plain_dst;
	bufin = coded_src;

	while (nprbytes > 4) {
		*(bufout++) = (uint8_t) ((uint8_t) (g_b64_pr2six[(*bufin)] << 2U)
				| (uint8_t) (g_b64_pr2six[((uint8_t) bufin[1])] >> 4U));
		*(bufout++) = (uint8_t) ((uint8_t) (g_b64_pr2six[(bufin[1])] << 4U)
				| (uint8_t) (g_b64_pr2six[(bufin[2])] >> 2U));
		*(bufout++) = (uint8_t) ((uint8_t) (g_b64_pr2six[(bufin[2])] << 6U)
				| (g_b64_pr2six[(bufin[3])]));
		bufin += 4;
		nprbytes -= 4;
	}

	/* Note: (nprbytes == 1) would be an error, so just ignore that case */
	if (nprbytes > 1) {
		*(bufout++) = (uint8_t) ((uint8_t) (g_b64_pr2six[(*bufin)] << 2U)
				| (uint8_t) (g_b64_pr2six[(bufin[1])] >> 4));
	}
	if (nprbytes > 2) {
		*(bufout++) = (uint8_t) ((uint8_t) (g_b64_pr2six[(bufin[1])] << 4U)
				| (uint8_t) (g_b64_pr2six[(bufin[2])] >> 2));
	}
	if (nprbytes > 3) {
		*(bufout++) = (uint8_t) ((uint8_t) (g_b64_pr2six[(bufin[2])] << 6U)
				| (uint8_t) (g_b64_pr2six[(bufin[3])]));
	}

	nbytesdecoded -= ((4U - nprbytes) & 3U);
	return nbytesdecoded;
}
