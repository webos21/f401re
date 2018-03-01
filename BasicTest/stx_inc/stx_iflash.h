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

#ifndef STX_IFLASH_H
#define STX_IFLASH_H

/* -----------------------------------------
 * The Internal-FLASH functions for STM32
 * ----------------------------------------- */

#include "misra.h"

/*
 * Start C++ Declaration
 */
#ifdef __cplusplus
extern "C" {
#endif

#if defined(STM32F10X_HD) || defined(STM32F10X_HD_VL) ||defined(STM32F10X_CL) || defined(STM32F10X_XL)
#    define IFLASH_PAGE_SIZE    (0x800)
#else
#    define IFLASH_PAGE_SIZE    (0x400)
#endif

/* XXX : Modify this value to device flash size */
#define USER_FLASH_SIZE         ((unsigned int)0x60000) /* 512kb */

#define STX_IFLASH_SECTOR_0     ((unsigned int)0x08000000) /* Base @ of Sector 0, 16 Kbyte */
#define STX_IFLASH_SECTOR_1     ((unsigned int)0x08004000) /* Base @ of Sector 1, 16 Kbyte */
#define STX_IFLASH_SECTOR_2     ((unsigned int)0x08008000) /* Base @ of Sector 2, 16 Kbyte */
#define STX_IFLASH_SECTOR_3     ((unsigned int)0x0800C000) /* Base @ of Sector 3, 16 Kbyte */
#define STX_IFLASH_SECTOR_4     ((unsigned int)0x08010000) /* Base @ of Sector 4, 64 Kbyte */
#define STX_IFLASH_SECTOR_5     ((unsigned int)0x08020000) /* Base @ of Sector 5, 128 Kbyte */
#define STX_IFLASH_SECTOR_6     ((unsigned int)0x08040000) /* Base @ of Sector 6, 128 Kbyte */
#define STX_IFLASH_SECTOR_7     ((unsigned int)0x08060000) /* Base @ of Sector 7, 128 Kbyte */
#define STX_IFLASH_SECTOR_8     ((unsigned int)0x08080000) /* Base @ of Sector 8, 128 Kbyte */
#define STX_IFLASH_SECTOR_9     ((unsigned int)0x080A0000) /* Base @ of Sector 9, 128 Kbyte */
#define STX_IFLASH_SECTOR_10    ((unsigned int)0x080C0000) /* Base @ of Sector 10, 128 Kbyte */
#define STX_IFLASH_SECTOR_11    ((unsigned int)0x080E0000) /* Base @ of Sector 11, 128 Kbyte */

/* End of the Flash address */
#define USER_FLASH_END_ADDRESS  ((unsigned int)(0x08000000 + USER_FLASH_SIZE - 1))

/* XXX : Modify this value to your flash map */
#define APPLICATION_ADDRESS     (STX_IFLASH_SECTOR_4)

typedef void (*pFunction)(void);

void    stx_iflash_open(void);
void    stx_iflash_close(void);
int32_t /*@alt void@*/ stx_iflash_erase(uint32_t sectorAddr, int32_t len);
int32_t /*@alt void@*/ stx_iflash_write(uint32_t flashAddr, void* dat, int32_t len); /* [len] must be multiple of 4 */
int32_t /*@alt void@*/ stx_iflash_locked(uint32_t sectorAddr, int32_t len);
int32_t /*@alt void@*/ stx_iflash_unlock(uint32_t sectorAddr, int32_t len);
void    stx_iflash_boot(uint32_t appAddress);

/*
 * End C++ Declaration
 */
#ifdef __cplusplus
}
#endif

#endif /* STX_IFLASH_H */
