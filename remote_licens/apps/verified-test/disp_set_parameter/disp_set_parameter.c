/*
 * disp_set_parameter.c - main interface of disp set parameter testcase.
 *
 * Copyright (C) 2016 - 2019, LomboTech Co.Ltd.
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

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>

#include "lcd_tool_drm.h"

int main(int argc, char **argv)
{
	int type, val;

	if (argc >= 3) {
		type = atoi(argv[1]);
		val = atoi(argv[2]);
		switch (type) {
		case 0:
			if (val == 0)
				lcd_tool_color_enhance_disable();
			else
				lcd_tool_color_enhance_enable();
			break;
		case 1:
			/* val range: 0 -100 */
			lcd_tool_set_color_enhance_brightness(val);
			break;
		case 2:
			/* val range: 0 -100 */
			lcd_tool_set_color_enhance_saturation(val);
			break;
		case 3:
			/* val range: 0 -100 */
			lcd_tool_set_color_enhance_contrast(val);
			break;
		case 4:
			/* val range: 0 -31, 0 mean close this function */
			lcd_tool_set_video_lti(val);
			break;
		case 5:
			/* val range: 0 -15, 0 mean close this function */
			lcd_tool_set_video_peak(val);
			break;
		default:
			printf("illegal type\n");
			break;
		}
	} else
		printf("too few parameters");

	return 0;
}

