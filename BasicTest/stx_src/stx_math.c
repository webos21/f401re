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

#include "stx_math.h"

#include <string.h>

/* ----------------------------------------------
 * Internal Structures
 * ---------------------------------------------- */

/* ----------------------------------------------
 * Interface Implementations
 * ---------------------------------------------- */

float32_t stxm_atof(const char_t *s) {
	float32_t a = 0.0F;
	int32_t e = 0;
	int32_t c;

	c = *s++;
	while ((c != '\0') && (c >= '0') && (c <= '9')) {
		a = a * 10.0F + (c - '0');
		c = *s++;
	}
	if (c == '.') {
		c = *s++;
		while ((c != '\0') && (c >= '0') && (c <= '9')) {
			a = a * 10.0F + (c - '0');
			e = e - 1;
			c = *s++;
		}
	}
	if ((c == 'e') || (c == 'E')) {
		int32_t sign = 1;
		int32_t i = 0;
		c = *s++;
		if (c == '+') {
			c = *s++;
		} else if (c == '-') {
			c = *s++;
			sign = -1;
		} else {
			/* nothing to do */
		}
		while ((c >= '0') && (c <= '9')) {
			i = i * 10 + (c - '0');
			c = *s++;
		}
		e += i * sign;
	}
	while (e > 0) {
		a *= 10.0F;
		e--;
	}
	while (e < 0) {
		a *= 0.1F;
		e++;
	}

	return a;
}

char_t *stxm_ftoa(float32_t f, /*@returned@*/char_t *s, int32_t ap) {
	int32_t i;
	int32_t clen = 0;

	int32_t neg;

	int32_t ipart;

	char_t pbuf[32];
	char_t *ret = (char_t *) s;

	/* shift the floating-point */
	for (i = 0; i < ap; i++) {
		f *= 10.0F;
	}

	/* extract the number and negative flag */
	ipart = (int) f;
	if (ipart < 0) {
		ipart = -ipart;
		neg = 1;
	} else {
		neg = 0;
	}

	/* print the floating-part */
	for (i = 0; i < ap; i++) {
		pbuf[clen++] = (ipart % 10) + '0';
		ipart = ipart / 10;
	}

	/* print the '.' */
	pbuf[clen++] = '.';

	/* print the integer-part */
	while (ipart) {
		pbuf[clen++] = (ipart % 10) + '0';
		ipart = ipart / 10;
	}

	/* if integer-part is 0, then attach the '0'!! */
	if (pbuf[clen - 1] == '.') {
		pbuf[clen++] = '0';
	}

	/* print the negative sign, if it needed */
	if (neg) {
		pbuf[clen++] = '-';
	}
	pbuf[clen] = '\0';

	/* reverse the result string! */
	for (i = 0; i < clen; i++) {
		ret[i] = pbuf[(clen - i - 1)];
	}
	ret[i] = '\0';

	return ret;
}

float32_t stxm_modf(float32_t f, register float32_t *iptr) {
	int32_t ipart;

	ipart = (int32_t) f;
	(*iptr) = (f - ipart);

	return ((float32_t) ipart);
}

