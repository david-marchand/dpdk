/*
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2023 Napatech A/S
 */

#include <stdlib.h>

#include "hw_mod_backend.h"
#include "flow_api_engine.h"

void km_free_ndev_resource_management(void **handle)
{
	if (*handle) {
		free(*handle);
		NT_LOG(DBG, FILTER, "Free NIC DEV CAM and TCAM record manager");
	}

	*handle = NULL;
}
