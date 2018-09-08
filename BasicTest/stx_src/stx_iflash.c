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

/* ----------------------------------------------
 * Headers
 * ---------------------------------------------- */

#include "stx_iflash.h"

#include "stx_watchdog.h"
#include "stx_util.h"

#include "stx_driver.h"

#ifdef S_SPLINT_S
#include "stm32f4xx_hal_flash.h"
#endif

/* ----------------------------------------------
 * Internal Structures
 * ---------------------------------------------- */

/* ----------------------------------------------
 * Interface Implementations
 * ---------------------------------------------- */

void stx_iflash_open(void) {
	(void) HAL_FLASH_Unlock();
	(void) HAL_FLASH_OB_Unlock();
}

void stx_iflash_close(void) {
	(void) HAL_FLASH_Lock();
	(void) HAL_FLASH_OB_Lock();
}

int32_t stx_iflash_erase(uint32_t sectorAddr, int32_t len) {
	FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t PAGEError = 0;

	int32_t rc = -1L;

	if ((sectorAddr % IFLASH_PAGE_SIZE) == 0) {
		EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
		EraseInitStruct.Banks = 0;
		EraseInitStruct.Sector = sectorAddr;
		EraseInitStruct.NbSectors = len;
		EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;

		if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) == HAL_OK) {
			rc = 0L;
		} else {
			/*
			 Error occurred while page erase.
			 User can add here some code to deal with this error.
			 PAGEError will contain the faulty page and then to know the code error on this page,
			 user can call function 'HAL_FLASH_GetError()'
			 */
			util_err0("Error on erasing!\r\n");
		}
	} else {
		util_err1("Invalid sector address! (0x%X)\r\n", sectorAddr);
	}

	return rc;
}

int32_t stx_iflash_write(uint32_t flashAddr, void* dat, int32_t len) {
	int32_t i = 0;
	int32_t dataLen;
	uint32_t *datBuf = dat;

	int32_t rc = -1L;

	if ((len % sizeof(int)) == 0) {
		dataLen = len / sizeof(int);
		for (i = 0;
				(i < dataLen) && (flashAddr <= (USER_FLASH_END_ADDRESS - 4));
				i++) {
			if ((HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, flashAddr,
					*(datBuf + i)) == HAL_OK)
					&& /* Check the written value */(*(unsigned int*) flashAddr
							== *(datBuf + i))) {
				flashAddr += 4;
				rc = 0L;
			} else {
				util_err0("Error on writing!\r\n");
				rc = -1L;
				break;
			}
		}
	} else {
		util_err0("data length is not multiples of sizeof(int)!!\r\n");
	}

	return rc;
}

int32_t stx_iflash_locked(uint32_t sectorAddr, int32_t len) {
	FLASH_OBProgramInitTypeDef OptionsBytesStruct;

	int32_t rc = -1L;

	if ((sectorAddr % IFLASH_PAGE_SIZE) == 0) {

		HAL_FLASHEx_OBGetConfig(&OptionsBytesStruct);
		/* Somthing to do */
	} else {
		util_err1("Invalid page address! (0x%X)\r\n", sectorAddr);
	}

	return rc;
}

int32_t stx_iflash_unlock(uint32_t sectorAddr, int32_t len) {
	/*
	 * Delete the Option Bytes (UNLOCK), and Reset Device
	 * But, this is never happened!!
	 */

	int32_t obChanged = 0;

	FLASH_OBProgramInitTypeDef OptionsBytesStruct;

	int32_t rc = -1L;

	if ((sectorAddr % IFLASH_PAGE_SIZE) == 0) {
		HAL_FLASHEx_OBGetConfig(&OptionsBytesStruct);

		rc = 0L;

		/* Somthing to do */
#if 0
		if ((OptionsBytesStruct.WRPSector & umm) != umm) {
			OptionsBytesStruct.OptionType = OPTIONBYTE_WRP;
			OptionsBytesStruct.WRPState = OB_WRPSTATE_DISABLE;
			OptionsBytesStruct.WRPSector = umm;
			if (HAL_FLASHEx_OBProgram(&OptionsBytesStruct) != HAL_OK) {
				util_err1("cannot unlock the page!! (0x08%X)\r\n", dp);
				rc = -1L;
				break;
			}
			obChanged = 1;
		}
#endif
		if (obChanged) {
			HAL_FLASH_OB_Launch();
		}
	} else {
		util_err1("Invalid page address! (0x%X)\r\n", sectorAddr);
	}

	return rc;
}

void stx_iflash_boot(uint32_t appAddress) {
	pFunction Jump_To_Application;
	uint32_t JumpAddress;

	/* Test if user code is programmed starting from address "APPLICATION_ADDRESS" */
	if (((*(__IO uint32_t*) appAddress) & 0x2FFD0000U) == 0x20000000U) {
		/* Lock the Flash Program Erase controller */
		stx_iflash_close();

		/* Jump to user application */
		JumpAddress = *(__IO uint32_t *) (appAddress + 4);
		Jump_To_Application = (pFunction) JumpAddress;

		/* Initialize user application's Stack Pointer */
		__set_MSP(*(__IO uint32_t *) appAddress);

		Jump_To_Application();
	} else {
		stx_iflash_close();
		util_err0("Jump to application error!\r\n");
	}

	while (1) {
	}
}
