/*
 * lombo_mock_dma.h - Generic definitions for LomboTech SoCs
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifndef __LOMBO_MOCK_DMA_H__
#define __LOMBO_MOCK_DMA_H__

#include <sound/pcm.h>
#include <sound/soc.h>
#include <linux/dmaengine.h>

struct mock_pcm_config {
	const struct snd_pcm_hardware *pcm_hardware;
	unsigned int prealloc_buffer_size;
	void *i2s_base;
	int i2s_irq;
};

int mock_dma_platform_register(struct device *dev,
			struct mock_pcm_config *config);
void mock_dma_platform_unregister(struct device *dev);

#endif /* __LOMBO_MOCK_DMA_H__ */

