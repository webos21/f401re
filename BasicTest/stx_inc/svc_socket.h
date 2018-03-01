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

#ifndef SVC_SOCKET_H
#define SVC_SOCKET_H

/* -----------------------------------------
 * The SOCKET functions for SERVICE
 * ----------------------------------------- */

#include "misra.h"

/*
 * Start C++ Declaration
 */
#ifdef __cplusplus
extern "C" {
#endif


#define SVC_SOCKETS_MAX    5

#define SVC_SOCKET_FD_MIN  0
#define SVC_SOCKET_FD_MAX  4

int32_t  svc_socket_open(void);

int32_t  svc_socket_connect(int32_t fd, const char_t *hostname, uint16_t port, int32_t timeout);
int32_t  svc_socket_recv(int32_t fd, void *buf, int32_t rlen); /* rlen => MODEM_ITM_LEN */
int32_t  svc_socket_send(int32_t fd, void *buf, int32_t wlen);

int32_t  /*@alt void@*/ svc_socket_close(int32_t fd);


/*
 * End C++ Declaration
 */
#ifdef __cplusplus
}
#endif

#endif /* SVC_SOCKET_H */
