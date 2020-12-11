/* lombo_disp_tool.h
 *
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Authors:
 *	lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _LOMBO_DISP_TOOL_H_
#define _LOMBO_DISP_TOOL_H_

int disp_usb_write(void *cmd, void *data);
int disp_usb_read(void *cmd, void *data);

#endif	/* _LOMBO_DISP_TOOL_H_ */
