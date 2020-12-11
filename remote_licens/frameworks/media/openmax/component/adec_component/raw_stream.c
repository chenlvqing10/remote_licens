/*
 * pcm_steam.c - demo code for aenc component.
 *
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#define DBG_LEVEL         DBG_INFO
#define LOG_TAG           "adec_raw_stream"

#include <oscl.h>
#include <base_component.h>
#include "raw_stream.h"
#include "decode_com.h"
#include <demuxer_plugin.h>

int resetRawBuf(raw_buf_manager *raw_manager)
{
	raw_manager->len = 0;
	raw_manager->offset = 0;

	return 0;
}

/*
********************************************************************************
*Description: updata raw data for audio decode lib.
*
*Arguments  : raw_manager       struct for store raw data;
*             data     data to be decoded from demuxer;
*             size     data length
*             packet   in buffer for audio decode lib;
*             ret      decode return data;
*
*Return     : data size has been consumed;
********************************************************************************
*/
int updataRawData(raw_buf_manager *raw_manager,
	uint8_t *data, size_t size,
	packet_t *packet, int ret)
{
	int res = 0;
	if (!raw_manager || !packet)
		return -1;
	if ((data == NULL || size == 0) && raw_manager->len <= 0)
		return 0;

	if (ret >= 0) {
		if (raw_manager->len) {
			raw_manager->len -= ret;
			raw_manager->offset += ret;
			if (raw_manager->len < 0) {
				OSCL_LOGE("errorlen:%d", raw_manager->len);
				raw_manager->len = 0;
				raw_manager->offset = 0;
			} else if (raw_manager->len == 0) {
				OSCL_LOGD("offset:%d", raw_manager->offset);
				raw_manager->offset = 0;
			} else if ((raw_manager->offset >
					RAW_BUFFER_SIZE - PCM_FRAME_SIZE)
				&& (raw_manager->offset > raw_manager->len)) {
				memmove(raw_manager->raw_buf, raw_manager->raw_buf
					+ raw_manager->offset, raw_manager->len),
					raw_manager->offset = 0;
			}
#if 0
		} else if (ret < size) {
			if (size - ret < RAW_BUFFER_SIZE - PCM_FRAME_SIZE) {
				memcpy(raw_manager->raw_buf, data + ret, size - ret);
				raw_manager->len = size - ret;
				raw_manager->offset = 0;
				res = size;
			} else {
				OSCL_LOGE("too large size:%ld,ret:%d", (long)size, ret);
				res = ret;
			}
		} else {
			res = size;
		}
#else
		} else {
			res = ret < size ? ret : size;
		}
#endif

	} else if (ret == DEC_UNDERFLOW) {
		/* get next data */
		OSCL_LOGD("DEC_UNDERFLOW, packet size %d", packet->size);
		if (size > 0) {
			memcpy(raw_manager->raw_buf + raw_manager->offset, data, size);
			raw_manager->len += size;
			res = size;
		}
		if (raw_manager->len) {
			raw_manager->len -= packet->size;
			raw_manager->offset += packet->size;
		}
	} else {
		if (raw_manager->len) {
			raw_manager->len -= packet->size;
			raw_manager->offset += packet->size;
			if (raw_manager->len <= 0) {
				raw_manager->len = 0;
				raw_manager->offset = 0;
			}
		} else {
			if (packet->size > size && size > 0)
				packet->size = size;
			else if (packet->size <= 0)
				packet->size = 1;
			int cpsz = size - packet->size;
			memcpy(raw_manager->raw_buf, data + packet->size, cpsz);
			raw_manager->len = cpsz;
			raw_manager->offset = 0;
			res = packet->size + cpsz;
		}
		OSCL_LOGE("decode err! ret:%d, packet.size:%d, res=%d",
			ret, packet->size, res);
	}

	return res;
}

/*
********************************************************************************
*Description: updata raw data for audio decode lib.
*
*Arguments  : raw_manager       struct for store raw data;
*             data     data to be decoded from demuxer;
*             size     data length
*             packet   in buffer for audio decode lib;
*
*Return     : data size has been consumed;
********************************************************************************
*/
int getRawData(raw_buf_manager *raw_manager,
	uint8_t *data, size_t size,
	packet_t *packet)
{
	int res = 0;
	if (!raw_manager || !packet)
		return -1;
	if ((data == NULL || size == 0) && raw_manager->len <= 0) {
		memset(packet, 0, sizeof(packet_t));
		return 0;
	}

	if (raw_manager->len > 0) {
		if (size > 0) {
			if ((raw_manager->offset + raw_manager->len + size >
					RAW_BUFFER_SIZE - PCM_FRAME_SIZE)
				&& (raw_manager->offset > raw_manager->len)) {
				memcpy(raw_manager->raw_buf, raw_manager->raw_buf +
					raw_manager->offset, raw_manager->len);
				OSCL_LOGD("len:%d,offset size %d:size:%d",
					raw_manager->len, raw_manager->offset, size);
				raw_manager->offset = 0;
			}
			if (raw_manager->len  + raw_manager->offset + size <
				RAW_BUFFER_SIZE) {
				memcpy(raw_manager->raw_buf + raw_manager->offset
					+ raw_manager->len, data, size);
				raw_manager->len += size;
				res = size;
			}
		}

		packet->data = raw_manager->raw_buf + raw_manager->offset;
		packet->size = raw_manager->len;
	} else {
		packet->data = data;
		packet->size = size;
	}

	return res;
}

