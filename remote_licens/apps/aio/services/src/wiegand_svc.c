/*
 * wiegand_svc.c - Interface to wiegand
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

#include "wiegand_write.h"
#include "wiegand_svc.h"

#define LOG_TAG "wiegand_svc"
#include <log/log.h>

#define DEF_WIEGAND_DATA_SIZE	26

/* wiegand data size contian even bit and odd bit */
static int s_data_size = DEF_WIEGAND_DATA_SIZE;

int wiegand_write_svc(uint64_t cardid)
{
	int ret;

	ret = wg_write(s_data_size, cardid);
	if(ret != 0) {
		ALOGE("%s[%d] wiegand write error",__func__, __LINE__);
		return -1;
	}

	return 0;
}

/**
 * wiegand_write_set_data_size - set wiegand protocol
 * @data_size: wiegand data size contian even bit and odd bit
 *
 * Returns 0 if success, otherwise not 0.
 */
int wiegand_write_set_data_size(int data_size)
{
	s_data_size = data_size;

	return 0;
}

