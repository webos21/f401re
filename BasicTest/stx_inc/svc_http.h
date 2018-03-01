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

#ifndef SVC_HTTP_H
#define SVC_HTTP_H

/* -----------------------------------------
 * The HTTP functions for SERVICE
 * ----------------------------------------- */

#include "httplib.h"

/*
 * Start C++ Declaration
 */
#ifdef __cplusplus
extern "C" {
#endif

int32_t svc_http_post(hl_t *handle, hl_response_t *res, char_t *url, char_t *data, uint32_t dlen);

int32_t svc_http_info(hl_t *handle, hl_response_t *res, char_t *url);

int32_t svc_http_down(hl_t *handle, hl_response_t *res, char_t *url, uint32_t storage);

/*
 * End C++ Declaration
 */
#ifdef __cplusplus
}
#endif

#endif /* SVC_HTTP_H */
