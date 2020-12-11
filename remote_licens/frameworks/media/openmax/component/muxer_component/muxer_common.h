/*
 * muxer_common.h - Standard functionality for the audio encodec component.
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

#ifndef __MUXER_COMMON_H__
#define __MUXER_COMMON_H__

int muxer_init(format_muxer *muxer, int mode, AVIOFILE fp);
int muxer_write_header(format_muxer *muxer);
int muxer_write_packet(format_muxer *muxer, packet_t *pin);
int muxer_write_trailer(format_muxer *muxer);
int muxer_deinit(format_muxer *muxer);
int muxer_check_bitstream(format_muxer *muxer, packet_t *pin);
int muxer_ctrl(format_muxer *muxer, int cmd, void *param);

#endif /* __MUXER_COMMON_H__ */

