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

#ifndef STX_SFLASH_H
#define STX_SFLASH_H

/* -----------------------------------------
 * The Serial-FLASH functions for STM32
 * ----------------------------------------- */

#include "misra.h"

/*
 * Start C++ Declaration
 */
#ifdef __cplusplus
extern "C" {
#endif

/* -----------------------------------------
 * Block & Sector & Page)
 * -----------------------------------------
 *  - Block  Size = 0x010000  (64 KBytes)
 *  - Sector Size = 0x001000  (4 KBytes)
 *  - Page   Size = 0x000100  (256 Bytes)
 * -----------------------------------------
 * [EN25Q64]
 *  - 64 MBits = 8MBytes = 8,388,608 bytes
 *  (128 Blocks = 2048 Sectors = 32768 Pages)
 * -----------------------------------------
 *    Block#00 : 0x000000 ~ 0x00FFFF
 *       Sector#00 = 0x000000 ~ 0x000FFF
 *       Sector#01 = 0x001000 ~ 0x001FFF
 *       Sector#02 = 0x002000 ~ 0x002FFF
 *            ...
 *       Sector#15 = 0x00F000 ~ 0x00FFFF
 *
 *    Block#01 : 0x010000 ~ 0x01FFFF
 *       Sector#00 = 0x010000 ~ 0x010FFF
 *       Sector#01 = 0x011000 ~ 0x011FFF
 *       Sector#02 = 0x012000 ~ 0x012FFF
 *            ...
 *       Sector#15 = 0x01F000 ~ 0x01FFFF
 *    ...
 * ----------------------------------------- */
#define SFLASH_PAGESIZE 0x0100
/* ----------------------------------------- */

void stx_sflash_init(void);

/*----- basic operation functions -----*/
uint32_t stx_sflash_read_id(void);
void     stx_sflash_chip_erase(void);
void     stx_sflash_block_erase(uint32_t BlockAddr);
void     stx_sflash_sector_erase(uint32_t SectorAddr);
void     stx_sflash_read(uint32_t rAddr, void *rBuf, uint32_t rLen);
void     stx_sflash_write_page(uint32_t wAddr, void *wBuf, uint32_t wLen);
void     stx_sflash_write_buf(uint32_t wAddr, void *wBuf, uint32_t wLen);

/*
 * End C++ Declaration
 */
#ifdef __cplusplus
}
#endif

#endif /* STX_SFLASH_H */
