/*
 * Copyright (c) 2000-2015 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Flora License, Version 1.1 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://floralicense.org/license/
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file     drm-tizen-mid.h
 * @author   Sunggun Jung <sunggun.jung@samsung.com>
 *           Kyungwook Tak <k.tak@samsung.com>
 *
 */

#ifndef __DRM_TIZEN_MID_H__
#define __DRM_TIZEN_MID_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <linux/unistd.h>
#include <unistd.h>

/*Flag to Enable Widget DRM Rights installation using the Linux OMA DRM design */
//#define __EDRM_ENABLE_WIDGET_DRM__
#define drmgettid() (long int)syscall(__NR_gettid)
#define DRM_FILENAME(X) (strrchr((char*)(X), '/') ? (char*)(strrchr((char*)(X), '/') + 1) : ((char*)(X)) )


#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "DRM_TIZEN"

#include <dlog.h>

/* Tizen Apps DRM related logs */
#define DRM_TAPPS_LOG(FMT, ARG...)               SLOGI(FMT, ##ARG)
#define DRM_TAPPS_FRQ_LOG(FMT, ARG...)           SLOGD(FMT, ##ARG)
#define DRM_TAPPS_EXCEPTION(FMT, ARG...)         SLOGE(FMT, ##ARG)
#define DRM_TAPPS_SECURE_LOG(FMT, ARG...)        SECURE_SLOGI(FMT, ##ARG)
#define DRM_TAPPS_SECURE_EXCEPTION(FMT, ARG...)  SECURE_SLOGE(FMT, ##ARG)

#ifdef __cplusplus
}
#endif

#endif /* __DRM_TIZEN_MID_H__*/
