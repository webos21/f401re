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

#include "jsmn.h"

#include <stddef.h>

/* ----------------------------------------------
 * Internal Structures
 * ---------------------------------------------- */

/* ----------------------------------------------
 * Internal Functions
 * ---------------------------------------------- */

/* Forward Declaration : Begin */
static jsmntok_t *jsmn_alloc_token(jsmn_parser *parser, jsmntok_t *jtoks,
		uint32_t num_tokens);
static void jsmn_fill_token(jsmntok_t *jtoks, jsmntype_e type, int32_t start,
		int32_t end);
static int32_t jsmn_parse_primitive(jsmn_parser *parser, const char_t *js,
		uint32_t len, jsmntok_t *jtoks, uint32_t num_tokens);
static int32_t jsmn_parse_string(jsmn_parser *parser, const char_t *js,
		uint32_t len, jsmntok_t *jtoks, uint32_t num_tokens);
/* Forward Declaration : End */

/**
 * Allocates a fresh unused token from the token pull.
 */
static jsmntok_t *jsmn_alloc_token(jsmn_parser *parser, jsmntok_t *jtoks,
		uint32_t num_tokens) {
	jsmntok_t *ret = NULL;

	if (parser->toknext < num_tokens) {
		ret = &jtoks[parser->toknext++];
		ret->start = ret->end = -1;
		ret->size = 0;
#ifdef JSMN_PARENT_LINKS
		ret->parent = -1;
#endif
	}

	return ret;
}

/**
 * Fills token type and boundaries.
 */
static void jsmn_fill_token(jsmntok_t *jtoks, jsmntype_e type, int32_t start,
		int32_t end) {
	jtoks->type = type;
	jtoks->start = start;
	jtoks->end = end;
	jtoks->size = 0;
}

/**
 * Fills next available token with JSON primitive.
 */
static int32_t jsmn_parse_primitive(jsmn_parser *parser, const char_t *js,
		uint32_t len, jsmntok_t *jtoks, uint32_t num_tokens) {
	int32_t rc = 1L;

	jsmntok_t *itok;
	int32_t spos;

	spos = parser->pos;

	for (; (parser->pos < len) && (js[parser->pos] != '\0'); parser->pos++) {
		if (
#ifndef JSMN_STRICT
		(js[parser->pos] == ':') ||
#endif
				(js[parser->pos] == '\t') || (js[parser->pos] == '\r')
				|| (js[parser->pos] == '\n') || (js[parser->pos] == ' ')
				|| (js[parser->pos] == ',') || (js[parser->pos] == ']')
				|| (js[parser->pos] == '}')) {
			rc = 0L;
		}
		if ((js[parser->pos] < 32) || (js[parser->pos] >= 127)) {
			parser->pos = spos;
			rc = JSMN_ERROR_INVAL;
		}
		if (rc <= 0L) {
			break;
		}
	}

	if (rc == 0L) {
		if (jtoks == NULL) {
			parser->pos--;
			rc = 0;
		} else {
			itok = jsmn_alloc_token(parser, jtoks, num_tokens);
			if (itok == NULL) {
				parser->pos = spos;
				rc = JSMN_ERROR_NOMEM;
			} else {
				jsmn_fill_token(itok, JSMN_PRIMITIVE, spos, parser->pos);
#ifdef JSMN_PARENT_LINKS
				itok->parent = parser->toksuper;
#endif
				parser->pos--;
			}
		}
	} else {
#ifdef JSMN_STRICT
		/* In strict mode primitive must be followed by a comma/object/array */
		parser->pos = spos;
		rc = JSMN_ERROR_PART;
#endif
	}

	return rc;
}

/**
 * Fills next token with JSON string.
 */
static int32_t jsmn_parse_string(jsmn_parser *parser, const char_t *js,
		uint32_t len, jsmntok_t *jtoks, uint32_t num_tokens) {
	int32_t rc = 1L;

	jsmntok_t *itok;

	int32_t spos = parser->pos;

	parser->pos++;

	/* Skip starting quote */
	for (; (parser->pos < len) && (js[parser->pos] != '\0'); parser->pos++) {
		char_t c = js[parser->pos];

		/* Flag : reset */
		rc = 1L;

		/* Quote: end of string */
		if (c == '\"') {
			if (jtoks == NULL) {
				rc = 0L;
			} else {
				itok = jsmn_alloc_token(parser, jtoks, num_tokens);
				if (itok == NULL) {
					parser->pos = spos;
					rc = JSMN_ERROR_NOMEM;
				} else {
					jsmn_fill_token(itok, JSMN_STRING, spos + 1, parser->pos);
#ifdef JSMN_PARENT_LINKS
					itok->parent = parser->toksuper;
#endif
					rc = 0L;
				}
			}
		} else if ((c == '\\') && (parser->pos + 1 < len)) { /* Backslash: Quoted symbol expected */
			int32_t i;

			parser->pos++;
			switch (js[parser->pos]) {
			/* Allowed escaped symbols */
			case '\"':
			case '/':
			case '\\':
			case 'b':
			case 'f':
			case 'r':
			case 'n':
			case 't':
				break;
				/* Allows escaped symbol \uXXXX */
			case 'u':
				parser->pos++;
				for (i = 0;
						(i < 4) && (parser->pos < len)
								&& (js[parser->pos] != '\0'); i++) {
					/* If it isn't a hex character we have an error */
					if (!((/* 0-9 */(js[parser->pos] >= 48)
							&& (js[parser->pos] <= 57))
							|| /* A-F */((js[parser->pos] >= 65)
									&& (js[parser->pos] <= 70))
							|| /* a-f */((js[parser->pos] >= 97)
									&& (js[parser->pos] <= 102)))) {
						parser->pos = spos;
						rc = JSMN_ERROR_INVAL;
						/*@innerbreak@*/
						break;
					} else {
						parser->pos++;
					}
				}
				parser->pos--;
				break;
				/* Unexpected symbol */
			default:
				parser->pos = spos;
				rc = JSMN_ERROR_INVAL;
				break;
			}
		} else {
			/* Nothing to do */
		}

		if (rc <= 0) {
			break;
		}
	}

	if (rc == 1L) {
		parser->pos = spos;
		rc = JSMN_ERROR_PART;
	}

	return rc;
}

/* ----------------------------------------------
 * Interface Implementations
 * ---------------------------------------------- */

/**
 * Creates a new parser based over a given  buffer with an array of tokens
 * available.
 */
void jsmn_init(jsmn_parser *parser) {
	parser->pos = 0;
	parser->toknext = 0;
	parser->toksuper = -1;
}

/**
 * Parse JSON string and fill tokens.
 */
int32_t jsmn_parse(jsmn_parser *parser, const char_t *js, uint32_t len,
		jsmntok_t *jtoks, uint32_t num_tokens) {
	int32_t rc = 1L;

	int32_t r;
	int32_t i;
	jsmntok_t *itok;
	int32_t count = parser->toknext;

	for (; (parser->pos < len) && (js[parser->pos] != '\0'); parser->pos++) {
		char_t c;
		jsmntype_e type;

		rc = 1L;

		c = js[parser->pos];
		switch (c) {
		case '{':
		case '[':
			count++;
			if (jtoks == NULL) {
				rc = 0L;
			} else {
				itok = jsmn_alloc_token(parser, jtoks, num_tokens);
				if (itok == NULL) {
					rc = JSMN_ERROR_NOMEM;
				} else {
					if (parser->toksuper != -1) {
						jtoks[parser->toksuper].size++;
#ifdef JSMN_PARENT_LINKS
						itok->parent = parser->toksuper;
#endif
					}
					itok->type = (c == '{' ? JSMN_OBJECT : JSMN_ARRAY);
					itok->start = parser->pos;
					parser->toksuper = parser->toknext - 1;
				}
			}
			break;
		case '}':
		case ']':
			if (jtoks == NULL) {
				rc = 0L;
			} else {
				type = (c == '}' ? JSMN_OBJECT : JSMN_ARRAY);
#ifdef JSMN_PARENT_LINKS
				if (parser->toknext < 1) {
					rc = JSMN_ERROR_INVAL;
				} else {
					itok = &jtoks[parser->toknext - 1];
					for (;;) {
						if ((itok->start != -1) && (itok->end == -1)) {
							if (itok->type != type) {
								rc = JSMN_ERROR_INVAL;
							} else {
								itok->end = parser->pos + 1;
								parser->toksuper = itok->parent;
								rc = 0L;
							}
						}
						if (itok->parent == -1) {
							if ((itok->type != type)
									|| (parser->toksuper == -1)) {
								rc = JSMN_ERROR_INVAL;
							} else {
								rc = 0L;
							}
						}
						if (rc <= 0L) {
							/*@innerbreak@*/
							break;
						}
						itok = &jtoks[itok->parent];
					}
#else
					for (i = parser->toknext - 1; i >= 0; i--) {
						itok = &jtoks[i];
						if (itok->start != -1 && itok->end == -1) {
							if (itok->type != type) {
								return JSMN_ERROR_INVAL;
							}
							parser->toksuper = -1;
							itok->end = parser->pos + 1;
							/*@innerbreak@*/
							break;
						}
					}
					/* Error if unmatched closing bracket */
					if (i == -1) {
						return JSMN_ERROR_INVAL;
					}
					for (; i >= 0; i--) {
						itok = &jtoks[i];
						if (itok->start != -1 && itok->end == -1) {
							parser->toksuper = i;
							/*@innerbreak@*/
							break;
						}
					}
#endif
				}
			}
			break;
		case '\"':
			r = jsmn_parse_string(parser, js, len, jtoks, num_tokens);
			if (r < 0) {
				count = r;
				rc = 0L;
			} else {
				count++;
				if ((parser->toksuper != -1) && (jtoks != NULL)) {
					jtoks[parser->toksuper].size++;
				}
			}
			break;
		case '\t':
		case '\r':
		case '\n':
		case ' ':
			break;
		case ':':
			parser->toksuper = parser->toknext - 1;
			break;
		case ',':
			if ((jtoks != NULL) && (parser->toksuper != -1)
					&& (jtoks[parser->toksuper].type != JSMN_ARRAY)
					&& (jtoks[parser->toksuper].type != JSMN_OBJECT)) {
#ifdef JSMN_PARENT_LINKS
				parser->toksuper = jtoks[parser->toksuper].parent;
#else
				for (i = parser->toknext - 1; i >= 0; i--) {
					if (jtoks[i].type == JSMN_ARRAY
							|| jtoks[i].type == JSMN_OBJECT) {
						if (jtoks[i].start != -1 && jtoks[i].end == -1) {
							parser->toksuper = i;
							/*@innerbreak@*/
							break;
						}
					}
				}
#endif
			}
			break;
#ifdef JSMN_STRICT
			/* In strict mode primitives are: numbers and booleans */
			case '-': case '0': case '1' : case '2': case '3' : case '4':
			case '5': case '6': case '7' : case '8': case '9':
			case 't': case 'f': case 'n' :
			/* And they must not be keys of the object */
			if (jtoks != NULL && parser->toksuper != -1) {
				jsmntok_t *t = &jtoks[parser->toksuper];
				if (t->type == JSMN_OBJECT ||
						(t->type == JSMN_STRING && t->size != 0)) {
					return JSMN_ERROR_INVAL;
				}
			}
#else
			/* In non-strict mode every unquoted value is a primitive */
		default:
#endif
			r = jsmn_parse_primitive(parser, js, len, jtoks, num_tokens);
			if (r < 0) {
				count = r;
				rc = 0;
			} else {
				count++;
				if ((parser->toksuper != -1) && (jtoks != NULL)) {
					jtoks[parser->toksuper].size++;
				}
			}
			break;

#ifdef JSMN_STRICT
			/* Unexpected char in strict mode */
			default:
			return JSMN_ERROR_INVAL;
#endif
		}

		if (rc <= 0L) {
			break;
		}
	}

	if (jtoks != NULL) {
		for (i = parser->toknext - 1; i >= 0; i--) {
			/* Unmatched opened object or array */
			if ((jtoks[i].start != -1) && (jtoks[i].end == -1)) {
				rc = JSMN_ERROR_PART;
			}
		}
	}

	return ((rc == 0L) ? count : rc);
}
