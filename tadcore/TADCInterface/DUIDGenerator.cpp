//
// Copyright (c) 2012 Samsung Electronics Co., Ltd.
//
// Licensed under the Apache License, Version 2.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

/**
 * @file		DUIDGenerator.cpp
 * @brief		This is the implementation file for get_id.
 */

#include <stdlib.h>
#include <string.h>
#include <device_info.h>

#include "DUIDGenerator.h"
#include "drm-tizen-mid.h"

int get_duid(char **duid)
{
	const char *device_id = NULL;

	if (duid == NULL) {
		DRM_TAPPS_EXCEPTION("Invalid argument.");
		return -1;
	}

	device_id = get_device_id();
	if (device_id == NULL) {
		DRM_TAPPS_EXCEPTION("Failed to generate DUID.");
		return -1;
	}

	*duid = strdup(device_id);
	DRM_TAPPS_LOG("DUID is [%s].", *duid);

	return 0;
}
