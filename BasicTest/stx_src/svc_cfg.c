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

#include "svc_cfg.h"

#include "stx_sflash.h"

#include <string.h>

/* ----------------------------------------------
 * Internal Structures
 * ---------------------------------------------- */

static cfg_t g_cfg;

/* ----------------------------------------------
 * Internal Functions
 * ---------------------------------------------- */

/* Forward Declaration : Begin */
static void svc_cfg_reset(void);
/* Forward Declaration : End */

static void svc_cfg_reset(void) {
	memset(&g_cfg, 0xFF, sizeof(cfg_t));

	g_cfg.cfg_ver = sizeof(cfg_t);
	memset(g_cfg.dtg_url, 0, sizeof(g_cfg.dtg_url));

	svc_cfg_sync();
}

/* ----------------------------------------------
 * Interface Implementations
 * ---------------------------------------------- */

void svc_cfg_init(void) {
	memset(&g_cfg, 0xFF, sizeof(cfg_t));
	svc_cfg_reload();
}

void svc_cfg_sync(void) {
#if 0
	stx_sflash_sector_erase(FM_CFG_SECTOR);
	stx_sflash_write_buf(FM_CFG_SECTOR, &g_cfg, sizeof(cfg_t));
#endif
}

void svc_cfg_reload(void) {
#if 0
	stx_sflash_read(FM_CFG_SECTOR, &g_cfg, sizeof(cfg_t));
#endif
	if (g_cfg.cfg_ver != (int32_t) sizeof(cfg_t)) {
		svc_cfg_reset();
	}
}

cfg_t *svc_cfg_get(void) {
	return &g_cfg;
}

int32_t svc_cfg_car_get(void) {
	return g_cfg.car_type;
}

char_t *svc_cfg_svr_url(void) {
	return g_cfg.dtg_url;
}

