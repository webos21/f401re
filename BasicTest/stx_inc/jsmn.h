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

#ifndef JSMN_H
#define JSMN_H

/* -----------------------------------------
 * The JSMN (JSON) Library
 * ----------------------------------------- */

#include "misra.h"

/*
 * Start C++ Declaration
 */
#ifdef __cplusplus
extern "C" {
#endif

#define JSMN_PARENT_LINKS

/**
 * JSON type identifier. Basic types are:
 * 	o Object
 * 	o Array
 * 	o String
 * 	o Other primitive: number, boolean (true/false) or null
 */
typedef enum e_jsmntype_e {
	JSMN_UNDEFINED = 0,
	JSMN_OBJECT = 1,
	JSMN_ARRAY = 2,
	JSMN_STRING = 3,
	JSMN_PRIMITIVE = 4
} jsmntype_e;

typedef enum e_jsmnerr_e {
	/* Not enough tokens were provided */
	JSMN_ERROR_NOMEM = -1,
	/* Invalid character inside JSON string */
	JSMN_ERROR_INVAL = -2,
	/* The string is not a full JSON packet, more bytes expected */
	JSMN_ERROR_PART = -3
} jsmnerr_e;

/**
 * JSON token description.
 * @param		type	type (object, array, string etc.)
 * @param		start	start position in JSON data string
 * @param		end		end position in JSON data string
 */
typedef struct s_jsmntok_t {
	jsmntype_e type;
	int32_t start;
	int32_t end;
	int32_t size;
#ifdef JSMN_PARENT_LINKS
	int32_t parent;
#endif
} jsmntok_t;

/**
 * JSON parser. Contains an array of token blocks available. Also stores
 * the string being parsed now and current position in that string
 */
typedef struct s_jsmn_parser {
	uint32_t pos; /* offset in the JSON string */
	uint32_t toknext; /* next token to allocate */
	int32_t toksuper; /* superior token node, e.g parent object or array */
} jsmn_parser;

/**
 * Create JSON parser over an array of tokens
 */
void jsmn_init(jsmn_parser *parser);

/**
 * Run JSON parser. It parses a JSON data string into and array of tokens, each describing
 * a single JSON object.
 */
int32_t jsmn_parse(jsmn_parser *parser, const char_t *js, uint32_t len,
		jsmntok_t *jtoks, uint32_t num_tokens);

/*
 * End C++ Declaration
 */
#ifdef __cplusplus
}
#endif

#endif /* JSMN_H */
