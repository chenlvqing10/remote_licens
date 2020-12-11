/*
 * relay.h - Define function for operate relay
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

#ifndef _RELAY_H
#define _RELAY_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* status of relay */
typedef enum _RELAY_STATE {
	RELAY_OFF = 0,
	RELAY_ON,
} RELAY_STATE;

/*
* relay_ctrl - set relay status
* @state: relay status
*
* return 0 if success, otherwise return -1
*/
int relay_ctrl(RELAY_STATE state);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _RELAY_H */

