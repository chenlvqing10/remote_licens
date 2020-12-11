/*
 * wiegand_write.c - define some function of operate wiegand
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

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>

#define LOG_TAG "WG"
#include "log/log.h"

#include "wiegand_write.h"

#define WIEGAND_TX_DEV_PATH		"/dev/wiegand_tx"
#define DEFAULT_DADA_BYTES		26
#define MAX_TX_DATA_SIZE		64

struct wiegand_tx_para {
	int data_size; /* weigand bit nums, 24 when wiegand26, 32 when wiegand34 */
	uint64_t cardid; /* weigand card id, max size is 64 bits*/
};

int wg_write(int data_size, uint64_t cardid)
{
	int fd, ret;
	struct wiegand_tx_para tx_para;

	fd = open(WIEGAND_TX_DEV_PATH, O_RDWR);
	if (fd <= 0) {
		ALOGE("open %s device error!\n", WIEGAND_TX_DEV_PATH);
		return -1;
	}

	tx_para.data_size = data_size;
	tx_para.cardid = cardid;

	if ((tx_para.data_size > MAX_TX_DATA_SIZE)
		|| (tx_para.data_size % 2 != 0)) {
		ALOGE("data_size[%u] is illegal, use default val[%d]\n",
				tx_para.data_size, DEFAULT_DADA_BYTES);
		tx_para.data_size = DEFAULT_DADA_BYTES;
	}

	/* ALOGD("wiegand%u cardid[%llu]\n", tx_para.data_size, tx_para.cardid); */

	tx_para.data_size = tx_para.data_size - 2;
	ret = write(fd, &tx_para, sizeof(tx_para));
	if (ret != sizeof(tx_para))
		ALOGE("write err ret[%d]\n", ret);
	ALOGD("%s[%d] write() ret %d (sizeof(tr_para) %d) data %lld",
			__func__, __LINE__, ret, sizeof(tx_para), tx_para.cardid);

	close(fd);

	return 0;
}
