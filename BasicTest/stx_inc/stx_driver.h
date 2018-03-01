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

#ifndef STX_DRIVER_H
#define STX_DRIVER_H

/* -----------------------------------------
 * The STM32 Driver/Library
 * ----------------------------------------- */

/*
 * Start C++ Declaration
 */
#ifdef __cplusplus
extern "C" {
#endif

#ifdef STM32F401xE

#ifdef __ICCARM__
/*
 * for including "HAL Driver"
 * Pm154 : (MISRA C 2004 rule 19.10) function-like macro, each instance of a parameter shall be enclosed
 */
#pragma diag_suppress=Pm154
#endif
#ifdef S_SPLINT_S
#include "stm32f4xx_hal_def.h"
#endif
#include "stm32f4xx_hal.h"

#ifdef HAL_CAN_MODULE_ENABLED

typedef CanTxMsgTypeDef can_txm_t;
typedef CanRxMsgTypeDef can_rxm_t;

#define CAN_RTR_D    CAN_RTR_DATA
#define CAN_IDT_S    CAN_ID_STD
#define CAN_IDT_E    CAN_ID_EXT
#endif /* HAL_CAN_MODULE_ENABLED */

#endif /* STM32F105xC */


#ifdef STM32F10X_CL

#include "stm32f10x_conf.h"

typedef CanTxMsg can_txm_t;
typedef CanRxMsg can_rxm_t;

#define CAN_RTR_D    CAN_RTR_Data
#define CAN_IDT_S    CAN_Id_Standard
#define CAN_IDT_E    CAN_Id_Extended

#endif /* STM32F10X_CL */

/*
 * End C++ Declaration
 */
#ifdef __cplusplus
}
#endif

#endif /* STX_DRIVER_H */
