/*
 * memctrl-test.c - memctrl test driver
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

#include <linux/delay.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/dmaengine.h>
#include <linux/dma-mapping.h>
#include <linux/platform_data/dma-imx.h>

#include "common.h"

/* redefine mode name, for PTR_XXX */
#undef MOD_NAME
#define MOD_NAME	"MEMCTRL_TEST"	/* for print message */

#define TRANSFER_COUNTS_1k		1024

static bool dma_m2m_filter(struct dma_chan *chan, void *param)
{
	return true;
}

static void dma_m2m_callback(void *data)
{
	return;
}

void memctrl_test(void)
{
	dma_cap_mask_t m2m_dma_mask;
	struct dma_slave_config dma_m2m_config;
	struct dma_async_tx_descriptor *tx;
	struct dma_chan *m2m_dma_chan = NULL;
	dma_cookie_t cookie;
	char *src = NULL, *dst = NULL;
	dma_addr_t dma_src, dma_dst;
	int ret;

	/* 1. request channel */
	dma_cap_zero(m2m_dma_mask);
	dma_cap_set(DMA_SLAVE, m2m_dma_mask);
	m2m_dma_chan = dma_request_channel(m2m_dma_mask, dma_m2m_filter, NULL);
	if (!m2m_dma_chan) {
		PRT_ERR("request dma channel failed\n");
		return;
	}

	/* 2. alloc dma memory */
	src = dma_alloc_coherent(NULL, TRANSFER_COUNTS_1k, &dma_src, GFP_DMA);
	if (!src) {
		PRT_ERR("alloc dma failed\n");
		goto exit;
	}
	dst = dma_alloc_coherent(NULL, TRANSFER_COUNTS_1k, &dma_dst, GFP_DMA);
	if (!dst) {
		PRT_ERR("alloc dma failed\n");
		goto exit;
	}

	/* 3. config and prepare cyclic dma */
	dma_m2m_config.direction = DMA_MEM_TO_MEM;
	dma_m2m_config.dst_addr = dma_dst;
	dma_m2m_config.src_addr = dma_src;
	dma_m2m_config.src_addr_width = DMA_SLAVE_BUSWIDTH_8_BYTES;
	dma_m2m_config.dst_addr_width = DMA_SLAVE_BUSWIDTH_8_BYTES;
	dma_m2m_config.dst_maxburst = 8;
	dma_m2m_config.src_maxburst = 8;
	ret = dmaengine_slave_config(m2m_dma_chan, &dma_m2m_config);
	if (ret) {
		PRT_ERR("dmaengine_slave_config failed");
		goto exit;
	}

	tx = m2m_dma_chan->device->device_prep_dma_memcpy(m2m_dma_chan,
							dma_src,
							dma_dst,
							TRANSFER_COUNTS_1k,
							DMA_CTRL_ACK);
	if (!tx) {
		PRT_ERR("fail to prepare dma memcpy\n");
		goto exit;
	}

	/* 4. submit tx descriptor */
	tx->callback = dma_m2m_callback;
	tx->callback_param = NULL;
	cookie = tx->tx_submit(tx);
	if (dma_submit_error(cookie)) {
		PRT_ERR("submit cookie fail\n");
		goto exit;
	}

	/* 5. issue dma transfer */
	dma_async_issue_pending(m2m_dma_chan);

	msleep(2000);

	/* 6. terminate dma transfer */
	ret = dmaengine_terminate_all(m2m_dma_chan);
	if (ret) {
		PRT_ERR("dmaengine_terminate_all fail\n");
		goto exit;
	}

	PRT_INFO("memctrl test ok\n");

exit:
	if (src)
		dma_free_coherent(NULL, TRANSFER_COUNTS_1k, src, dma_src);
	if (dst)
		dma_free_coherent(NULL, TRANSFER_COUNTS_1k, dst, dma_dst);
	if (m2m_dma_chan)
		dma_release_channel(m2m_dma_chan);
}

