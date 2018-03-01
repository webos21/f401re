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

#include "stx_sys.h"

#include "stx_driver.h"

#ifdef S_SPLINT_S
#include "stm32f1xx_hal_gpio.h"
#endif

/* ----------------------------------------------
 * Internal Structures
 * ---------------------------------------------- */

#ifndef STX_SYS
#define GPIO_PORT_A                 (GPIOA)
#define GPIO_PORT_C                 (GPIOC)

#define OBD_LED_PIN                 (GPIO_PIN_5)

#define TACT_SW_PIN                 (GPIO_PIN_13)
#endif /* STX_SYS */

static __IO int32_t g_obd_on = 0L;
static __IO int32_t g_modem_on = 0L;

/* ----------------------------------------------
 * Interface Implementations
 * ---------------------------------------------- */

void stx_sys_init(void) {
	stx_modem_led_set(0);
	stx_obd_led_set(0);
}

void stx_obd_led_set(int32_t onOff) {
	g_obd_on = onOff;
	if (onOff != 0) {
		HAL_GPIO_WritePin(GPIO_PORT_A, OBD_LED_PIN, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(GPIO_PORT_A, OBD_LED_PIN, GPIO_PIN_RESET);
	}
}

void stx_data_led_toggle(void) {
	stx_obd_led_set(((g_obd_on != 0) ? 0 : 1));
}

void stx_modem_power_set(int32_t onOff) {
	stx_obd_led_set(onOff);
}

void stx_modem_led_set(int32_t onOff) {
	stx_obd_led_set(onOff);
}

void stx_modem_led_toggle(void) {
	stx_modem_led_set(((g_modem_on != 0) ? 0 : 1));
}

int32_t stx_modem_sim_read(void) {
	return stx_tact_sw_read();
}

void stx_gps_power_set(int32_t onOff) {
	stx_obd_led_set(onOff);
}

int32_t stx_tact_sw_read(void) {
	GPIO_PinState ps = HAL_GPIO_ReadPin(GPIO_PORT_C, TACT_SW_PIN);
	return ((ps == GPIO_PIN_SET) ? 0L : 1L); /* opposite value */
}
