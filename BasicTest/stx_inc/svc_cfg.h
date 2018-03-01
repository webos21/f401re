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

#ifndef SVC_CFG_H
#define SVC_CFG_H

/* -----------------------------------------
 * The CFG functions for SERVICE
 * ----------------------------------------- */

#include "misra.h"

/*
 * Start C++ Declaration
 */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * See the "svc_exf.h"
 */
#ifndef FM_CFG_SECTOR
#define FM_CFG_SECTOR  0x001000    /* Sector 1 */
#endif /* FM_CFG_SECTOR */

typedef struct s_cfg_t {
	int32_t cfg_ver;

	int32_t car_type;
	char_t dtg_url[64];
} cfg_t;

void svc_cfg_init(void);
void svc_cfg_sync(void);
void svc_cfg_reload(void);

/*@exposed@*/
cfg_t *svc_cfg_get(void);

int32_t svc_cfg_car_get(void);

/*@exposed@*/
char_t *svc_cfg_svr_url(void);

/*
 * End C++ Declaration
 */
#ifdef __cplusplus
}
#endif

#endif /* SVC_CFG_H */
