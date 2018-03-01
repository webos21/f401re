/* Copyright Joyent, Inc. and other Node contributors. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
#ifndef http_parser_h
#define http_parser_h

/* -----------------------------------------
 * The HTTP Parser
 * ----------------------------------------- */

#include "misra.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Also update SONAME in the Makefile whenever you change these. */
#define HTTP_PARSER_VERSION_MAJOR 2
#define HTTP_PARSER_VERSION_MINOR 7
#define HTTP_PARSER_VERSION_PATCH 1

/* Compile with -DHTTP_PARSER_STRICT=0 to make less checks, but run
 * faster
 */
#ifndef HTTP_PARSER_STRICT
# define HTTP_PARSER_STRICT 1
#endif

/* Maximium header size allowed. If the macro is not defined
 * before including this header then the default is used. To
 * change the maximum header size, define the macro in the build
 * environment (e.g. -DHTTP_MAX_HEADER_SIZE=<value>). To remove
 * the effective limit on the size of the header, define the macro
 * to a very large number (e.g. -DHTTP_MAX_HEADER_SIZE=0x7fffffff)
 */
#ifndef HTTP_MAX_HEADER_SIZE
# define HTTP_MAX_HEADER_SIZE (80*1024)
#endif

typedef struct s_http_parser_t http_parser;
typedef struct s_http_parser_settings_t http_parser_settings;

/* Callbacks should return non-zero to indicate an error. The parser will
 * then halt execution.
 *
 * The one exception is on_headers_complete. In a HTTP_RESPONSE parser
 * returning '1' from on_headers_complete will tell the parser that it
 * should not expect a body. This is used when receiving a response to a
 * HEAD request which may contain 'Content-Length' or 'Transfer-Encoding:
 * chunked' headers that indicate the presence of a body.
 *
 * Returning `2` from on_headers_complete will tell parser that it should not
 * expect neither a body nor any futher responses on this connection. This is
 * useful for handling responses to a CONNECT request which may not contain
 * `Upgrade` or `Connection: upgrade` headers.
 *
 * http_data_cb does not return data chunks. It will be called arbitrarily
 * many times for each string. E.G. you might get 10 callbacks for "on_url"
 * each providing just a few characters more data.
 */
typedef int32_t (*http_data_cb)(http_parser *hp, const char_t *at,
		uint32_t length);
typedef int32_t (*http_cb)(http_parser *hp);

enum http_status {
	HTTP_STATUS_CONTINUE = 100,
	HTTP_STATUS_SWITCHING_PROTOCOLS = 101,
	HTTP_STATUS_PROCESSING = 102,
	HTTP_STATUS_OK = 200,
	HTTP_STATUS_CREATED = 201,
	HTTP_STATUS_ACCEPTED = 202,
	HTTP_STATUS_NON_AUTHORITATIVE_INFORMATION = 203,
	HTTP_STATUS_NO_CONTENT = 204,
	HTTP_STATUS_RESET_CONTENT = 205,
	HTTP_STATUS_PARTIAL_CONTENT = 206,
	HTTP_STATUS_MULTI_STATUS = 207,
	HTTP_STATUS_ALREADY_REPORTED = 208,
	HTTP_STATUS_IM_USED = 226,
	HTTP_STATUS_MULTIPLE_CHOICES = 300,
	HTTP_STATUS_MOVED_PERMANENTLY = 301,
	HTTP_STATUS_FOUND = 302,
	HTTP_STATUS_SEE_OTHER = 303,
	HTTP_STATUS_NOT_MODIFIED = 304,
	HTTP_STATUS_USE_PROXY = 305,
	HTTP_STATUS_TEMPORARY_REDIRECT = 307,
	HTTP_STATUS_PERMANENT_REDIRECT = 308,
	HTTP_STATUS_BAD_REQUEST = 400,
	HTTP_STATUS_UNAUTHORIZED = 401,
	HTTP_STATUS_PAYMENT_REQUIRED = 402,
	HTTP_STATUS_FORBIDDEN = 403,
	HTTP_STATUS_NOT_FOUND = 404,
	HTTP_STATUS_METHOD_NOT_ALLOWED = 405,
	HTTP_STATUS_NOT_ACCEPTABLE = 406,
	HTTP_STATUS_PROXY_AUTHENTICATION_REQUIRED = 407,
	HTTP_STATUS_REQUEST_TIMEOUT = 408,
	HTTP_STATUS_CONFLICT = 409,
	HTTP_STATUS_GONE = 410,
	HTTP_STATUS_LENGTH_REQUIRED = 411,
	HTTP_STATUS_PRECONDITION_FAILED = 412,
	HTTP_STATUS_PAYLOAD_TOO_LARGE = 413,
	HTTP_STATUS_URI_TOO_LONG = 414,
	HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE = 415,
	HTTP_STATUS_RANGE_NOT_SATISFIABLE = 416,
	HTTP_STATUS_EXPECTATION_FAILED = 417,
	HTTP_STATUS_MISDIRECTED_REQUEST = 421,
	HTTP_STATUS_UNPROCESSABLE_ENTITY = 422,
	HTTP_STATUS_LOCKED = 423,
	HTTP_STATUS_FAILED_DEPENDENCY = 424,
	HTTP_STATUS_UPGRADE_REQUIRED = 426,
	HTTP_STATUS_PRECONDITION_REQUIRED = 428,
	HTTP_STATUS_TOO_MANY_REQUESTS = 429,
	HTTP_STATUS_REQUEST_HEADER_FIELDS_TOO_LARGE = 431,
	HTTP_STATUS_UNAVAILABLE_FOR_LEGAL_REASONS = 451,
	HTTP_STATUS_INTERNAL_SERVER_ERROR = 500,
	HTTP_STATUS_NOT_IMPLEMENTED = 501,
	HTTP_STATUS_BAD_GATEWAY = 502,
	HTTP_STATUS_SERVICE_UNAVAILABLE = 503,
	HTTP_STATUS_GATEWAY_TIMEOUT = 504,
	HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED = 505,
	HTTP_STATUS_VARIANT_ALSO_NEGOTIATES = 506,
	HTTP_STATUS_INSUFFICIENT_STORAGE = 507,
	HTTP_STATUS_LOOP_DETECTED = 508,
	HTTP_STATUS_NOT_EXTENDED = 510,
	HTTP_STATUS_NETWORK_AUTHENTICATION_REQUIRED = 511
};

enum http_method {
	HTTP_DELETE = 0,
	HTTP_GET = 1,
	HTTP_HEAD = 2,
	HTTP_POST = 3,
	HTTP_PUT = 4,
	/* pathological */
	HTTP_CONNECT = 5,
	HTTP_OPTIONS = 6,
	HTTP_TRACE = 7,
	/* WebDAV */
	HTTP_COPY = 8,
	HTTP_LOCK = 9,
	HTTP_MKCOL = 10,
	HTTP_MOVE = 11,
	HTTP_PROPFIND = 12,
	HTTP_PROPPATCH = 13,
	HTTP_SEARCH = 14,
	HTTP_UNLOCK = 15,
	HTTP_BIND = 16,
	HTTP_REBIND = 17,
	HTTP_UNBIND = 18,
	HTTP_ACL = 19,
	/* subversion */
	HTTP_REPORT = 20,
	HTTP_MKACTIVITY = 21,
	HTTP_CHECKOUT = 22,
	HTTP_MERGE = 23,
	/* upnp */
	HTTP_MSEARCH = 24,
	HTTP_NOTIFY = 25,
	HTTP_SUBSCRIBE = 26,
	HTTP_UNSUBSCRIBE = 27,
	/* RFC-5789 */
	HTTP_PATCH = 28,
	HTTP_PURGE = 29,
	/* CalDAV */
	HTTP_MKCALENDAR = 30,
	/* RFC-2068, section 19.6.1.2 */
	HTTP_LINK = 31,
	HTTP_UNLINK = 32
};

enum http_parser_type {
	HTTP_REQUEST,
	HTTP_RESPONSE,
	HTTP_BOTH
};

/* Flag values for http_parser.flags field */
enum e_flags {
	F_CHUNKED = 1 << 0,
	F_CONNECTION_KEEP_ALIVE = 1 << 1,
	F_CONNECTION_CLOSE = 1 << 2,
	F_CONNECTION_UPGRADE = 1 << 3,
	F_TRAILING = 1 << 4,
	F_UPGRADE = 1 << 5,
	F_SKIPBODY = 1 << 6,
	F_CONTENTLENGTH = 1 << 7
};

/* Define HPE_* values for each errno value above */
enum e_http_errno {
	HPE_OK,

	/* Callback-related errors */
	HPE_CB_message_begin,
	HPE_CB_url,
	HPE_CB_header_field,
	HPE_CB_header_value,
	HPE_CB_headers_complete,
	HPE_CB_body,
	HPE_CB_message_complete,
	HPE_CB_status,
	HPE_CB_chunk_header,
	HPE_CB_chunk_complete,

	/* Parsing-related errors */
	HPE_INVALID_EOF_STATE,
	HPE_HEADER_OVERFLOW,
	HPE_CLOSED_CONNECTION,
	HPE_INVALID_VERSION,
	HPE_INVALID_STATUS,
	HPE_INVALID_METHOD,
	HPE_INVALID_URL,
	HPE_INVALID_HOST,
	HPE_INVALID_PORT,
	HPE_INVALID_PATH,
	HPE_INVALID_QUERY_STRING,
	HPE_INVALID_FRAGMENT,
	HPE_LF_EXPECTED,
	HPE_INVALID_HEADER_TOKEN,
	HPE_INVALID_CONTENT_LENGTH,
	HPE_UNEXPECTED_CONTENT_LENGTH,
	HPE_INVALID_CHUNK_SIZE,
	HPE_INVALID_CONSTANT,
	HPE_INVALID_INTERNAL_STATE,
	HPE_STRICT,
	HPE_PAUSED,
	HPE_UNKNOWN
};

/* Get an http_errno value from an http_parser */
#define HTTP_PARSER_ERRNO(p)            ((enum e_http_errno) (p)->http_errno)

struct s_http_parser_t {
	/** PRIVATE **/
	uint32_t type :2; /* enum http_parser_type */
	uint32_t flags :8; /* F_* values from 'flags' enum; semi-public */
	uint32_t state :7; /* enum state from http_parser.c */
	uint32_t header_state :7; /* enum header_state from http_parser.c */
	uint32_t index :7; /* index into current matcher */
	uint32_t lenient_http_headers :1;

	uint32_t nread; /* # bytes read in various scenarios */
	uint64_t content_length; /* # bytes in body (0 if no Content-Length header) */

	/** READ-ONLY **/
	uint16_t http_major;
	uint16_t http_minor;
	uint32_t status_code :16; /* responses only */
	uint32_t method :8; /* requests only */
	uint32_t http_errno :7;

	/* 1 = Upgrade header was present and the parser has exited because of that.
	 * 0 = No upgrade header present.
	 * Should be checked when http_parser_execute() returns in addition to
	 * error checking.
	 */
	uint32_t upgrade :1;

	/** PUBLIC **/
	void *data; /* A pointer to get hook to the "connection" or "socket" object */
};

struct s_http_parser_settings_t {
	http_cb on_message_begin;
	http_data_cb on_url;
	http_data_cb on_status;
	http_data_cb on_header_field;
	http_data_cb on_header_value;
	http_cb on_headers_complete;
	http_data_cb on_body;
	http_cb on_message_complete;
	/* When on_chunk_header is called, the current chunk length is stored
	 * in parser->content_length.
	 */
	http_cb on_chunk_header;
	http_cb on_chunk_complete;
};

enum http_parser_url_fields {
	UF_SCHEMA = 0,
	UF_HOST = 1,
	UF_PORT = 2,
	UF_PATH = 3,
	UF_QUERY = 4,
	UF_FRAGMENT = 5,
	UF_USERINFO = 6,
	UF_MAX = 7
};

/* Result structure for http_parser_parse_url().
 *
 * Callers should index into field_data[] with UF_* values iff field_set
 * has the relevant (1 << UF_*) bit set. As a courtesy to clients (and
 * because we probably have padding left over), we convert any port to
 * a uint16_t.
 */
struct s_http_parser_url {
	uint16_t field_set; /* Bitmask of (1 << UF_*) values */
	uint16_t port; /* Converted UF_PORT string */

	struct {
		uint16_t off; /* Offset into buffer in which field starts */
		uint16_t len; /* Length of run in buffer */
	} field_data[UF_MAX];
};

/* Returns the library version. Bits 16-23 contain the major version number,
 * bits 8-15 the minor version number and bits 0-7 the patch level.
 * Usage example:
 *    unsigned long version = http_parser_version();
 *    unsigned major = (version >> 16) & 255;
 *    unsigned minor = (version >> 8) & 255;
 *    unsigned patch = version & 255;
 *    <code>printf( "http_parser v%u.%u.%u\n", major, minor, patch );</code>
 */
uint32_t http_parser_version(void);

void http_parser_init(http_parser *parser, enum http_parser_type type);

/* Initialize http_parser_settings members to 0
 */
void http_parser_settings_init(http_parser_settings *settings);

/* Executes the parser. Returns number of parsed bytes. Sets
 * `parser->http_errno` on error. */
uint32_t http_parser_execute(http_parser *parser,
		const http_parser_settings *settings, const char_t *data, uint32_t len);

/* If http_should_keep_alive() in the on_headers_complete or
 * on_message_complete callback returns 0, then this should be
 * the last message on the connection.
 * If you are the server, respond with the "Connection: close" header.
 * If you are the client, close the connection.
 */
int32_t http_should_keep_alive(const http_parser *parser);

/*@exposed@*/
/* Returns a string version of the HTTP method. */
const char_t *http_method_str(enum http_method m);

/*@exposed@*/
/* Return a string name of the given error */
const char_t *http_errno_name(enum e_http_errno err);

/*@exposed@*/
/* Return a string description of the given error */
const char_t *http_errno_description(enum e_http_errno err);

/* Initialize all http_parser_url members to 0 */
void http_parser_url_init(struct s_http_parser_url *u);

/* Parse a URL; return nonzero on failure */
int32_t http_parser_parse_url(const char_t *buf, uint32_t buflen,
		int32_t is_connect, struct s_http_parser_url *u);

/* Pause or un-pause the parser; a nonzero value pauses */
void http_parser_pause(http_parser *parser, int32_t paused);

/* Checks if this is the final chunk of the body. */
int32_t http_body_is_final(const http_parser *parser);

#ifdef __cplusplus
}
#endif
#endif

