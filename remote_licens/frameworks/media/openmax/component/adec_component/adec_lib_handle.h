/*
 * adec_lib_handle.h - demo code for adec.
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
#ifndef __ADEC_LIB_HANDLE_H__
#define __ADEC_LIB_HANDLE_H__

#include "av_media_type.h"

void *open_adec_lib(av_codec_type_e id);
void close_adec_lib(void *dl_hdl);
int audio_codec_supported(av_codec_type_e id);

#endif /* __ADEC_LIB_HANDLE_H__ */
