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

#include "svc_cmd_proc.h"

#include "svc_main.h"
#include "svc_modem.h"
#include "svc_http.h"
#include "svc_cfg.h"

#include "stx_sys.h"
#include "stx_cpu.h"
#include "stx_crc32.h"
#include "stx_clock.h"
#include "stx_iflash.h"
#include "stx_uart.h"
#include "stx_watchdog.h"
#include "stx_util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ----------------------------------------------
 * Internal Structures
 * ---------------------------------------------- */

/* ----------------------------------------------
 * Internal Functions
 * ---------------------------------------------- */

/* Forward Declaration : Begin */
static void svc_cmd_proc_line_b(void);
static void svc_cmd_proc_line_c(void);
static void svc_cmd_proc_line_e(void);
static void svc_cmd_proc_show_mcu(void);
static void svc_cmd_proc_show_cfg(void);
static void svc_cmd_proc_show_all(void);
static int32_t /*@alt void@*/svc_cmd_proc_show(char_t *cmd, int32_t cmdlen, const cmd_t *chnd);
static int32_t /*@alt void@*/svc_cmd_proc_modem(char_t *cmd, int32_t cmdlen, const cmd_t *chnd);
static int32_t /*@alt void@*/svc_cmd_proc_cfg(char_t *cmd, int32_t cmdlen, const cmd_t *chnd);
static int32_t /*@alt void@*/svc_cmd_proc_wget(char_t *cmd, int32_t cmdlen, const cmd_t *chnd);
static int32_t /*@alt void@*/svc_cmd_proc_inf(char_t *cmd, int32_t cmdlen, const cmd_t *chnd);
static int32_t /*@alt void@*/svc_cmd_proc_test(char_t *cmd, int32_t cmdlen, const cmd_t *chnd);
/* Forward Declaration : End */

static void svc_cmd_proc_line_b(void) {
	util_print("\r\n=========================================\r\n");
}

static void svc_cmd_proc_line_c(void) {
	util_print("-----------------------------------------\r\n");
}

static void svc_cmd_proc_line_e(void) {
	util_print("=========================================\r\n\r\n");
}

static void svc_cmd_proc_show_mcu(void) {
	uint32_t cpuId[3] = { 0U, 0U, 0U };
	uint16_t flashSize = 0;

	stx_cpu_id(cpuId);
	flashSize = stx_cpu_flash_size();

	util_print("[MCU]\r\n");
	util_print("  * ID            : 0x%X 0x%X 0x%X\r\n", cpuId[0], cpuId[1], cpuId[2]);
	util_hexdump2(cpuId, sizeof(cpuId), 0);
	util_print("  * Flash Size    : %d KB\r\n", flashSize);
}

static void svc_cmd_proc_show_cfg(void) {
	cfg_t *cfg = svc_cfg_get();

	util_print("[CFG]\r\n");
	util_print("  * Version       : %d\r\n", cfg->cfg_ver);
	util_print("  * Car Type      : %d\r\n", cfg->car_type);
	util_print("  * Server URL    : %s\r\n", cfg->dtg_url);
}

static void svc_cmd_proc_show_all(void) {
	svc_cmd_proc_line_b();
	svc_cmd_proc_show_mcu();
	svc_cmd_proc_line_c();
	svc_cmd_proc_show_cfg();
	svc_cmd_proc_line_e();
}

static int32_t svc_cmd_proc_show(char_t *cmd, int32_t cmdlen, const cmd_t *chnd) {
	int32_t rv = 0L;

	if (chnd->nparam < 2) {
		svc_cmd_proc_show_summary();
	} else {
		if (strcmp("all", chnd->param[1]) == 0) {
			svc_cmd_proc_show_all();
		} else if (strcmp("mcu", chnd->param[1]) == 0) {
			svc_cmd_proc_line_b();
			svc_cmd_proc_show_mcu();
			svc_cmd_proc_line_e();
		} else if (strcmp("cfg", chnd->param[1]) == 0) {
			svc_cmd_proc_line_b();
			svc_cmd_proc_show_cfg();
			svc_cmd_proc_line_e();
		} else if (strcmp("uart", chnd->param[1]) == 0) {
			svc_cmd_proc_line_b();
			stx_uart_status();
			svc_cmd_proc_line_e();
		} else {
			util_print("Invalid arguments : %s (len=%d)\r\n", cmd, cmdlen);
			rv = -1L;
		}
	}

	return rv;
}

static int32_t svc_cmd_proc_modem(char_t *cmd, int32_t cmdlen, const cmd_t *chnd) {
	int32_t rv = -1L;

	if (chnd->nparam < 2) {
		util_print("Invalid arguments(%d < 2): %s\r\n", chnd->nparam, cmd);
	} else {
		rv = 0L;
		if (strcmp("on", chnd->param[1]) == 0) {
			if (chnd->nparam >= 3) {
				if (strcmp("fast", chnd->param[2]) == 0) {
					svc_modem_start(0);
				} else {
					util_print("Invalid argument : %s\r\n", chnd->param[2]);
					rv = -1L;
				}
			} else {
				svc_modem_start(1);
			}
		} else if (strcmp("off", chnd->param[1]) == 0) {
			svc_modem_stop();
		} else if (strcmp("status", chnd->param[1]) == 0) {
			svc_cmd_proc_line_b();
			svc_modem_status();
			svc_cmd_proc_line_e();
		} else if (strcmp("time", chnd->param[1]) == 0) {
			svc_modem_time_sync(3);
		} else if (strcmp("ppp", chnd->param[1]) == 0) {
			if (chnd->nparam < 3) {
				util_print("Invalid arguments(%d < 3): %s\r\n", chnd->nparam, cmd);
				rv = -1L;
			} else {
				if (strcmp("on", chnd->param[2]) == 0) {
					svc_modem_ppp_open(3);
				} else if (strcmp("off", chnd->param[2]) == 0) {
					svc_modem_ppp_close(3);
				} else {
					util_print("Invalid argument : %s\r\n", chnd->param[2]);
					rv = -1L;
				}
			}
		} else if (strcmp("normal", chnd->param[1]) == 0) {
			svc_modem_mode_set(RECV_MODE_LINE);
		} else if (strncasecmp("AT", chnd->param[1], 2) == 0) {
			char_t cmdbuf[64];
			main_t *handle = svc_main_get();

			svc_modem_clear_q();

			memset(cmdbuf, 0, sizeof(cmdbuf));
			(void) snprintf(cmdbuf, sizeof(cmdbuf), "%s\r\n", cmd + 6);

			svc_modem_init_req(&handle->mreq, 1, 3, cmdbuf, "OK", NULL);
			svc_modem_send_req(&handle->mreq);
		} else if (strncmp("dat", chnd->param[1], 3) == 0) {
			char_t cmdbuf[64];
			int32_t rc;

			memset(cmdbuf, 0, sizeof(cmdbuf));
			rc = snprintf(cmdbuf, sizeof(cmdbuf), "%s\r\n", cmd + 10);

			svc_modem_write(cmdbuf, rc);
		} else if (strcmp("dig", chnd->param[1]) == 0) {
			char_t buf[16];

			if (chnd->nparam < 3) {
				util_print("Invalid arguments(%d < 3): %s\r\n", chnd->nparam, cmd);
				rv = -1L;
			} else {
				memset(buf, 0, sizeof(buf));
				rv = hl_hostname_to_ipv4(chnd->param[2], buf, sizeof(buf));
				if (rv == 0) {
					util_print("%s = %s\r\n", chnd->param[2], buf);
				} else {
					util_print("Failed to resolve the '%s'...\r\n", chnd->param[2]);
				}
			}
		} else {
			util_print("Invalid arguments : %s (len=%d)\r\n", cmd, cmdlen);
			rv = -1L;
		}
	}

	return rv;
}

static int32_t svc_cmd_proc_cfg(char_t *cmd, int32_t cmdlen, const cmd_t *chnd) {
	int32_t rv = -1L;

	if (chnd->nparam < 3) {
		util_print("Invalid arguments(%d < 3): %s\r\n", chnd->nparam, cmd);
	} else {
		rv = 0L;

		if (strcmp("car_type", chnd->param[1]) == 0) {
			rv = 0L;
		} else if (strcmp("dtg_url", chnd->param[1]) == 0) {
			int32_t ret;
			main_t *handle = svc_main_get();

			ret = strcmp("force", chnd->param[3]);
			if ((chnd->nparam == 4) && (ret == 0)) {
				cfg_t *cfg = svc_cfg_get();
				memset(cfg->dtg_url, 0, sizeof(cfg->dtg_url));
				strcpy(cfg->dtg_url, chnd->param[2]);
				svc_cfg_sync();
			} else {
				ret = svc_http_info(handle->hl, &handle->hres, chnd->param[2]);
				if (ret > 0) {
					cfg_t *cfg = svc_cfg_get();
					memset(cfg->dtg_url, 0, sizeof(cfg->dtg_url));
					strcpy(cfg->dtg_url, chnd->param[2]);
					svc_cfg_sync();
				} else {
					util_print("Cannot connect to the URL : %s\r\n", chnd->param[2]);
					rv = -1L;
				}
			}
		} else {
			util_print("Invalid arguments(%d < 3): %s\r\n", chnd->nparam, cmd);
			rv = -1L;
		}
	}

	return rv;
}

static int32_t svc_cmd_proc_wget(char_t *cmd, int32_t cmdlen, const cmd_t *chnd) {
	int32_t rv = -1L;

	if (chnd->nparam < 3) {
		util_print("Invalid arguments(%d < 3): %s\r\n", chnd->nparam, cmd);
	} else {
		main_t *handle;

		handle = svc_main_get();
		if (strcmp("info", chnd->param[1]) == 0) {
			rv = svc_http_info(handle->hl, &handle->hres, chnd->param[2]);
			if (rv > 0) {
				hl_print_response(&handle->hres);
				rv = 0L;
			} else {
				rv = -1L;
			}
		} else {
			util_print("Invalid arguments : %s (len=%d)\r\n", cmd, cmdlen);
			rv = -1L;
		}
	}

	return rv;
}

static int32_t svc_cmd_proc_inf(char_t *cmd, int32_t cmdlen, const cmd_t *chnd) {
	int32_t rv = -1L;

	if (chnd->nparam < 4) {
		util_print("Invalid arguments(%d < 4): %s\r\n", chnd->nparam, cmd);
	} else {
		int32_t p;
		int32_t l;

		rv = 0L;

		p = strtol(chnd->param[2], NULL, 16);
		l = strtol(chnd->param[3], NULL, 10);

		if (strcmp("read", chnd->param[1]) == 0) {
			int32_t s;

			util_print("INF-Read : Address = 0x%08X / Length = %d\r\n", p, l);
			s = p;
			while (l > 0) {
				stx_watchdog_ok();
				if (l < 256) {
					util_hexdump2((void *) p, l, (p - s));
					p += l;
					l -= l;
				} else {
					util_hexdump2((void *) p, 256, (p - s));
					p += 256;
					l -= 256;
				}
			}
		} else if (strcmp("crc32", chnd->param[1]) == 0) {
			uint32_t crc32;
			stx_crc32_open();
			stx_crc32_calc((void *) p, l);
			crc32 = stx_crc32_close();

			util_print("CRC32 = 0x%08X\r\n", crc32);
		} else {
			util_print("Invalid arguments : %s (len=%d)\r\n", cmd, cmdlen);
			rv = -1L;
		}
	}

	return rv;
}

static int32_t svc_cmd_proc_test(char_t *cmd, int32_t cmdlen, const cmd_t *chnd) {
	int32_t rv = -1L;

	if (chnd->nparam < 2) {
		util_print("Invalid arguments(%d < 3): %s\r\n", chnd->nparam, cmd);
	} else {
		rv = 0L;

		if (strcmp("gps_power_on", chnd->param[1]) == 0) {
			stx_gps_power_set(1);
		} else if (strcmp("gps_power_off", chnd->param[1]) == 0) {
			stx_gps_power_set(0);
		} else if (strcmp("modem_power_on", chnd->param[1]) == 0) {
			stx_modem_power_set(1);
		} else if (strcmp("modem_power_off", chnd->param[1]) == 0) {
			stx_modem_power_set(0);
		} else if (strcmp("obd_led_on", chnd->param[1]) == 0) {
			stx_obd_led_set(1);
		} else if (strcmp("obd_led_off", chnd->param[1]) == 0) {
			stx_obd_led_set(0);
		} else if (strcmp("modem_led_on", chnd->param[1]) == 0) {
			stx_modem_led_set(1);
		} else if (strcmp("modem_led_off", chnd->param[1]) == 0) {
			stx_modem_led_set(0);
		} else if (strcmp("read_sim", chnd->param[1]) == 0) {
			util_print("SIM State : %d\r\n", stx_modem_sim_read());
		} else if (strcmp("read_sw", chnd->param[1]) == 0) {
			util_print("Tact Switch State : %d\r\n", stx_tact_sw_read());
		} else if (strcmp("usec", chnd->param[1]) == 0) {
			if (chnd->nparam < 3) {
				util_print("Invalid arguments : %s (len=%d)\r\n", cmd, cmdlen);
				rv = -1L;
			} else {
				int32_t wt;
				int64_t stick;
				int64_t etick;
				int32_t dtick;

				wt = strtol(chnd->param[2], NULL, 10);

				stick = stx_clock_tick();
				util_usleep(wt);
				etick = stx_clock_tick();

				dtick = etick - stick;
				util_print("%d usec = %d\r\n", wt, dtick);
			}
		} else if (strcmp("msec", chnd->param[1]) == 0) {
			if (chnd->nparam < 3) {
				util_print("Invalid arguments : %s (len=%d)\r\n", cmd, cmdlen);
				rv = -1L;
			} else {
				int32_t wt;
				int64_t stick;
				int64_t etick;
				int32_t dtick;

				wt = strtol(chnd->param[2], NULL, 10);

				stick = stx_clock_tick();
				util_msleep(wt);
				etick = stx_clock_tick();

				dtick = etick - stick;
				util_print("%d msec = %d\r\n", wt, dtick);
			}
		} else {
			util_print("Invalid arguments : %s (len=%d)\r\n", cmd, cmdlen);
			rv = -1L;
		}
	}

	return rv;
}

/* ----------------------------------------------
 * Interface Implementations
 * ---------------------------------------------- */

void svc_cmd_proc_help(void) {
	svc_cmd_proc_line_b();
	util_print("* Usages)\r\n");
	util_print("  - help               : show the commands (this usages)\r\n");
	util_print("  - show               : show the system summary\r\n");
	util_print("  - show all           : show the full status\r\n");
	util_print("  - show mcu           : show the MCU status\r\n");
	util_print("  - show cfg           : show the CONFIGURATION\r\n");
	util_print("  - show uart          : show the UART status\r\n");
	util_print("  - modem on [fast]    : turn on the modem\r\n");
	util_print("  - modem off          : turn off the modem\r\n");
	util_print("  - modem status       : get the modem status\r\n");
	util_print("  - modem time         : sync the time via modem\r\n");
	util_print("  - modem ppp on       : PPP open\r\n");
	util_print("  - modem ppp off      : PPP close\r\n");
	util_print("  - modem normal       : set the modem status to CMD mode\r\n");
	util_print("  - modem AT[CMD]      : send the AT command to modem\r\n");
	util_print("  - modem DAT:         : send the RAW DATA to modem\r\n");
	util_print("  - nslookup  [DNS]    : [DNS] to IP\r\n");
	util_print("  - wget info [url]    : get the web-page from [url]\r\n");
	util_print("  - exf erase [p] [l]  : erase the ExF address[p]\r\n");
	util_print("  - exf read  [p] [l]  : read  the ExF addr[p] len[l]\r\n");
	util_print("  - exf crc32 [p] [l]  : crc32 the ExF addr[p] len[l]\r\n");
	util_print("  - inf read  [p] [l]  : read  the IxF addr[p] len[l]\r\n");
	util_print("  - inf crc32 [p] [l]  : crc32 the IxF addr[p] len[l]\r\n");
	util_print("  - fuse ldr           : fuse the LDR bin from ExF to InF\r\n");
	util_print("  - fuse app           : move the APP bin from ExF to InF\r\n");
	util_print("  - reboot             : reboot the SYSTEM\r\n");
	svc_cmd_proc_line_e();
}

void svc_cmd_proc_show_summary(void) {
	char_t tbuf[64];
	stx_ts_t uts;

	main_t *handle;
	cfg_t *cfg;

	handle = svc_main_get();
	cfg = svc_cfg_get();

	stx_clock_ts(&uts);
	memset(tbuf, 0, sizeof(tbuf));
	(void) snprintf(tbuf, sizeof(tbuf), "%04d-%02d-%02d %02d:%02d:%02d", uts.year, uts.mon, uts.day, uts.hour, uts.min,
			uts.sec);

	svc_cmd_proc_line_b();
	util_print("[F401RE]\r\n");
#ifdef __ICCARM__
	util_print("  * Build by     : EWARM\r\n");
#else /* !__ICCARM__ */
	util_print("  * Build by     : GCC\r\n");
#endif /* __ICCARM__ */
	util_print("  * Up-Time      : %d (sec)\r\n", stx_clock_tick_sec());
	util_print("  * Current Time : %s\r\n", tbuf);
	util_print("  * Main  State  : %d\r\n", handle->state);
	util_print("  * CAN   Mode   : %d\r\n", cfg->car_type);
	util_print("  * Server URL   : %s\r\n", cfg->dtg_url);
	util_print("  * Phone No.    : %s\r\n", svc_modem_phone_no());
	util_print("  * Modem Status : %s\r\n", svc_modem_state_string((modem_state_e) svc_modem_state_get()));
	svc_cmd_proc_line_e();
}

void svc_cmd_proc(char_t *cmd, int32_t cmdlen) {
	if (cmdlen > 2) {
		cmd_t ct;
		svc_cmd_parse(&ct, cmd, cmdlen);

		switch (ct.cno) {
		case CMD_SHOW:
			svc_cmd_proc_show(cmd, cmdlen, &ct);
			break;
		case CMD_HELP:
			svc_cmd_proc_help();
			break;
		case CMD_CFG:
			svc_cmd_proc_cfg(cmd, cmdlen, &ct);
			break;
		case CMD_MODEM:
			svc_cmd_proc_modem(cmd, cmdlen, &ct);
			break;
		case CMD_WGET:
			svc_cmd_proc_wget(cmd, cmdlen, &ct);
			break;
		case CMD_INF:
			svc_cmd_proc_inf(cmd, cmdlen, &ct);
			break;
		case CMD_TEST:
			svc_cmd_proc_test(cmd, cmdlen, &ct);
			break;
		case CMD_REBOOT:
			stx_cpu_reboot();
			break;
		default:
			util_hexdump("Unknown command", cmd, cmdlen);
			break;
		}
		util_print("F401RE> ");
	} else {
		util_print("\r\nF401RE> ");
	}
}
