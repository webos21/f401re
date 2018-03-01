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

#ifndef SVC_BASE_H
#define SVC_BASE_H

/* -----------------------------------------
 * The BASE definitions for SERVICE
 * ----------------------------------------- */

/*
 * Start C++ Declaration
 */
#ifdef __cplusplus
extern "C" {
#endif

#if (defined(SVC_HOST) && SVC_HOST)
#define SVC_VECTOR_OFFSET 0x0
#else /* !SVC_HOST */
#define SVC_VECTOR_OFFSET 0x10000
#endif /* SVC_HOST */

/*
 * End C++ Declaration
 */
#ifdef __cplusplus
}
#endif

#endif /* SVC_BASE_H */
