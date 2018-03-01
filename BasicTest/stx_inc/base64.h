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

#ifndef BASE64_H
#define BASE64_H

/* -----------------------------------------
 * The Base64 encoding/decoding API
 * ----------------------------------------- */

#include "misra.h"

/**
 * @brief Base64 encoding/decoding API
 *
 * @file base64.h
 * @date 2010-08-31
 * @author Cheolmin Jo (webos21@gmail.com)
 */

/*
 * Start C++ Declaration
 */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief
 * Simple BASE64 encode/decode functions.\n
 * \n
 * As we might encode binary strings, hence we require the length of
 * the incoming plain source. And return the length of what we decoded.\n
 * \n
 * The decoding function takes any non valid char (i.e. whitespace, \\0
 * or anything non A-Z,0-9 etc) as terminal.\n
 * \n
 * The handling of terminating \\0 characters differs from function to
 * function.
 */

/**
 * Given the length of an un-encrypted string, get the length of the
 * encrypted string.
 *
 * @param len the length of an unencrypted string.
 * @return the length of the string after it is encrypted, including the trailing \\0
 */
int32_t base64_encode_len(int32_t len);


/**
 * Encode a text string using base64encoding.
 *
 * @param coded_dst The destination string for the encoded string. A \\0 is appended.
 * @param plain_src The original string in plain text
 * @param len_plain_src The length of the plain text string
 * @return the length of the encoded string, including the trailing \\0
 */
int32_t base64_encode(char_t *coded_dst, const void *plain_src,
		int32_t len_plain_src);

/**
 * Encode a text string using base64encoding.
 *
 * @param coded_dst The destination string for the encoded string. A \\0 is appended.
 * @param plain_src The original string in plain text
 * @param len_plain_src The length of the plain text string
 * @return the length of the encoded string, including the trailing \\0
 */
int32_t base64_encode_websafe(char_t *coded_dst, const void *plain_src,
		int32_t len_plain_src);

/**
 * Determine the maximum buffer length required to decode the plain text
 * string given the encoded string.
 *
 * @param coded_src The encoded string
 * @return the maximum required buffer length for the plain text string
 */
int32_t base64_decode_len(const void *coded_src);


/**
 * Decode a string to plain text.
 *
 * @param plain_dst The destination string for the plain text. A \\0 is appended.
 * @param coded_src The encoded string
 * @return the length of the plain text string (excluding the trailing \\0)
 */
int32_t base64_decode(void *plain_dst, const void *coded_src);



/**
 * Decode a string to plain text.
 *
 * @param plain_dst The destination string for the plain text. A \\0 is appended.
 * @param coded_src The encoded string
 * @return the length of the plain text string (excluding the trailing \\0)
 */
int32_t base64_decode_websafe(void *plain_dst, const void *coded_src);


/**
 * Decode an string to plain text. This is the same as xi_base64_decode()
 * except the result is not \\0-terminated
 *
 * @param plain_dst The destination string for the plain text. The string is not \\0-terminated.
 * @param coded_src The encoded string
 * @return the length of the plain text string
 */
int32_t base64_decode_binary(void *plain_dst, const void *coded_src);


/*
 * End C++ Declaration
 */
#ifdef __cplusplus
}
#endif

#endif /* BASE64_H */
