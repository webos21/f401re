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

#ifndef STX_MATH_H
#define STX_MATH_H

/* -----------------------------------------
 * The Math functions for STM32
 * ----------------------------------------- */

#include "misra.h"

/*
 * STM32F2xx has no FPU and the Soft-FP is only handle the [float].
 * So, these functions must be implemented!!
 */

/*
 * Start C++ Declaration
 */
#ifdef __cplusplus
extern "C" {
#endif

float32_t stxm_atof(const char_t *s);
char_t /*@alt void@*/ *stxm_ftoa(float32_t f, /*@returned@*/char_t *s, int32_t ap);  /* No Rounding, Just Ceiling */
float32_t stxm_modf(float32_t f, float32_t *iptr);

/*
 * End C++ Declaration
 */
#ifdef __cplusplus
}
#endif

#endif /* STX_MATH_H */
