/*
 * isp.h - Define function for operate isp
 *
 * Copyright (C) 2019, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _ISP_H
#define _ISP_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#include "face_svc.h"

/*
* isp_get_brightness - get brightness from isp
* @type: camera type
*
* return 0 if success, otherwise return -1
*/
int isp_get_brightness(enum face_camera_id type);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _ISP_H */

