#if defined(__MINGW32__)

/********************************************************
 *
 * STUB FUNCTION FOR MINGW COMPILER
 *
 ********************************************************/

#include "stx_driver.h"

static __IO uint32_t uwTick;

HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority) {
	/*Configure the SysTick to have interrupt in 1ms time basis*/
	HAL_SYSTICK_Config(SystemCoreClock / 1000U);

	/*Configure the SysTick IRQ priority */
	HAL_NVIC_SetPriority(SysTick_IRQn, TickPriority, 0U);

	/* Return function status */
	return HAL_OK;
}

void HAL_IncTick(void) {
	uwTick++;
}

uint32_t HAL_GetTick(void) {
	return uwTick;
}

HAL_StatusTypeDef HAL_RCC_OscConfig(RCC_OscInitTypeDef  *RCC_OscInitStruct) {
	UNUSED(RCC_OscInitStruct);
	return HAL_OK;
}

void HAL_TIMEx_CommutationCallback(TIM_HandleTypeDef *htim) {
	/* Prevent unused argument(s) compilation warning */
	UNUSED(htim);
	/* NOTE : This function Should not be modified, when the callback is needed,
	 the HAL_TIMEx_CommutationCallback could be implemented in the user file
	 */
}

void HAL_TIMEx_BreakCallback(TIM_HandleTypeDef *htim) {
	/* Prevent unused argument(s) compilation warning */
	UNUSED(htim);
	/* NOTE : This function Should not be modified, when the callback is needed,
	 the HAL_TIMEx_BreakCallback could be implemented in the user file
	 */
}

#else /* !__MINGW32__ */
typedef int compiler_happy_pedantic;
#endif
