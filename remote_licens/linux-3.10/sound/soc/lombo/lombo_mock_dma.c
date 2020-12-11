/*
 * lombo_mock_dma.c - Common code for LomboTech SoCs
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
#include <linux/slab.h>
#include <linux/dma-mapping.h>
#include <linux/module.h>
#include <linux/semaphore.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/time.h>
#include <linux/delay.h>

#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/dmaengine_pcm.h>
#include <linux/dma-mapping.h>
#include <linux/of.h>
#include <mach/debug.h>
#include <mach/csp.h>
#include "lombo_mock_dma.h"
#include "csp/i2s/csp_i2s.h"

#undef  MOD_NAME
#define MOD_NAME "MOCK_DMA"

typedef struct {
	int			actived;
	int			transfer_avail;
	int			pos;
	int			period_cnt;
	int			period;
	int			buf_size;
	unsigned char		*buf_start;
	u16			*cur_buf;
	u32			cur_buf_size;
	u32			cur_buf_left;
	struct snd_pcm_substream *substream;
} i2s_cpu_ctx_t;

typedef struct {
	i2s_cpu_ctx_t i2s_ctx[2];
	struct mock_pcm_config config;
} mock_dma_t;

static mock_dma_t g_mock;

static irqreturn_t lombo_i2s_top_handler(int irq, void *dev_id)
{
	i2s_cpu_ctx_t *tx_ctx = &g_mock.i2s_ctx[SNDRV_PCM_STREAM_PLAYBACK];
	i2s_cpu_ctx_t *rx_ctx = &g_mock.i2s_ctx[SNDRV_PCM_STREAM_CAPTURE];

	tx_ctx->transfer_avail = csp_i2s_tx_empty(g_mock.config.i2s_base);
	rx_ctx->transfer_avail = csp_i2s_rx_full(g_mock.config.i2s_base);

	return IRQ_WAKE_THREAD;
}

static irqreturn_t lombo_i2s_bottom_handler(int irq, void *dev_id)
{
	int i;
	i2s_cpu_ctx_t *tx_ctx = &g_mock.i2s_ctx[SNDRV_PCM_STREAM_PLAYBACK];
	i2s_cpu_ctx_t *rx_ctx = &g_mock.i2s_ctx[SNDRV_PCM_STREAM_CAPTURE];

	if (tx_ctx->actived && tx_ctx->transfer_avail) {
		if (tx_ctx->cur_buf_left) {
			for (i = 0; i < 8; i++) {
				csp_i2s_writel(g_mock.config.i2s_base,
							tx_ctx->cur_buf[0]);
				csp_i2s_writer(g_mock.config.i2s_base,
							tx_ctx->cur_buf[1]);
				tx_ctx->cur_buf_left -= 2;
				tx_ctx->cur_buf += 2;
				if (tx_ctx->cur_buf_left == 0)
					break;
			}
			csp_i2s_clear_txfe(g_mock.config.i2s_base);
			tx_ctx->transfer_avail = 0;
		}
		if (tx_ctx->cur_buf_left == 0) {
			tx_ctx->pos += tx_ctx->period;
			if (tx_ctx->pos >= tx_ctx->buf_size)
				tx_ctx->pos = 0;
			snd_pcm_period_elapsed(tx_ctx->substream);
			tx_ctx->cur_buf =
				(u16 *)(tx_ctx->buf_start + tx_ctx->pos);
			tx_ctx->cur_buf_left = tx_ctx->period / 2;
		}
	}

	if (rx_ctx->actived && rx_ctx->transfer_avail) {
		u32 tmp_value;

		if (rx_ctx->cur_buf_left) {
			for (i = 0; i < 8; i++) {
				tmp_value =
					csp_i2s_readl(g_mock.config.i2s_base);
				rx_ctx->cur_buf[0] = (u16)tmp_value;
				tmp_value =
					csp_i2s_readr(g_mock.config.i2s_base);
				rx_ctx->cur_buf[1] = (u16)tmp_value;
				rx_ctx->cur_buf_left -= 2;
				rx_ctx->cur_buf += 2;
				if (rx_ctx->cur_buf_left == 0)
					break;
			}
			csp_i2s_clear_rxda(g_mock.config.i2s_base);
			rx_ctx->transfer_avail = 0;
		}
		if (rx_ctx->cur_buf_left == 0) {
			rx_ctx->pos += rx_ctx->period;
			if (rx_ctx->pos >= rx_ctx->buf_size)
				rx_ctx->pos = 0;
			snd_pcm_period_elapsed(rx_ctx->substream);
			rx_ctx->cur_buf = (u16 *)
				(rx_ctx->buf_start + rx_ctx->pos);
			rx_ctx->cur_buf_left = rx_ctx->period / 2;
		}
	}

	return IRQ_HANDLED;
}


static int mock_hw_params(struct snd_pcm_substream *substream,
	struct snd_pcm_hw_params *params)
{
	i2s_cpu_ctx_t *ctx;
	struct snd_pcm_runtime *runtime = substream->runtime;

	PRT_INFO("Entered %s\n", __func__);
	ctx = &g_mock.i2s_ctx[substream->stream];
	ctx->period = params_period_bytes(params);
	ctx->period_cnt = params_periods(params);
	ctx->buf_size = params_buffer_bytes(params);
	ctx->buf_start = substream->dma_buffer.area;
	ctx->substream = substream;
	PRT_INFO("period count %d, period size %d, buf size %d\n",
		ctx->period_cnt, ctx->period, ctx->buf_size);
	snd_pcm_set_runtime_buffer(substream, &substream->dma_buffer);
	runtime->dma_bytes = ctx->buf_size;

	return 0;
}

static int mock_hw_free(struct snd_pcm_substream *substream)
{
	PRT_INFO("Entered %s\n", __func__);
	snd_pcm_set_runtime_buffer(substream, NULL);
	return 0;
}

static int mock_trigger(struct snd_pcm_substream *substream, int cmd)
{
	i2s_cpu_ctx_t *ctx = &g_mock.i2s_ctx[substream->stream];

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
		ctx->cur_buf = (u16 *)ctx->buf_start;
		ctx->cur_buf_left = ctx->period / 2;
		ctx->pos = 0;
		ctx->actived = 1;
		break;
	case SNDRV_PCM_TRIGGER_STOP:
		ctx->actived = 0;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static snd_pcm_uframes_t mock_pointer(struct snd_pcm_substream *substream)
{
	i2s_cpu_ctx_t *ctx = &g_mock.i2s_ctx[substream->stream];

	return bytes_to_frames(substream->runtime, ctx->pos);
}

static int mock_open(struct snd_pcm_substream *substream)
{
	PRT_INFO("Entered %s\n", __func__);
	snd_soc_set_runtime_hwparams(substream, g_mock.config.pcm_hardware);
	return 0;
}

static int mock_close(struct snd_pcm_substream *substream)
{
	PRT_INFO("Entered %s\n", __func__);
	return 0;
}

static int mock_mmap(struct snd_pcm_substream *substream,
	struct vm_area_struct *vma)
{
	struct snd_pcm_runtime *runtime = substream->runtime;

	PRT_INFO("Entered %s\n", __func__);

	return dma_mmap_coherent(substream->pcm->card->dev, vma,
				     runtime->dma_area,
				     runtime->dma_addr,
				     runtime->dma_bytes);
}

static struct snd_pcm_ops mock_dma_ops = {
	.open		= mock_open,
	.close		= mock_close,
	.ioctl		= snd_pcm_lib_ioctl,
	.hw_params	= mock_hw_params,
	.hw_free	= mock_hw_free,
	.trigger	= mock_trigger,
	.pointer	= mock_pointer,
	.mmap		= mock_mmap,
};


static int preallocate_dma_buffer(struct snd_pcm *pcm, int stream)
{
	struct snd_pcm_substream *substream = pcm->streams[stream].substream;
	struct snd_dma_buffer *buf = &substream->dma_buffer;
	size_t size = g_mock.config.prealloc_buffer_size;

	PRT_INFO("Entered %s\n", __func__);

	buf->dev.type = SNDRV_DMA_TYPE_DEV;
	buf->dev.dev = pcm->card->dev;
	buf->private_data = NULL;
	buf->area = dma_alloc_coherent(pcm->card->dev, size,
					   &buf->addr, GFP_KERNEL);
	if (!buf->area) {
		PRT_ERR("alloc dma buffer err\n");
		return -ENOMEM;
	}
	buf->bytes = size;
	return 0;
}

static void mock_free_dma_buffers(struct snd_pcm *pcm)
{
	struct snd_pcm_substream *substream;
	struct snd_dma_buffer *buf;
	int stream;

	PRT_INFO("Entered %s\n", __func__);

	for (stream = 0; stream < 2; stream++) {
		substream = pcm->streams[stream].substream;
		if (!substream)
			continue;

		buf = &substream->dma_buffer;
		if (!buf->area)
			continue;

		dma_free_coherent(pcm->card->dev, buf->bytes,
				      buf->area, buf->addr);
		buf->area = NULL;
	}
}

static u64 dma_mask = DMA_BIT_MASK(32);

static int mock_pcm_new(struct snd_soc_pcm_runtime *rtd)
{
	struct snd_card *card = rtd->card->snd_card;
	struct snd_pcm *pcm = rtd->pcm;
	int ret = 0;

	PRT_INFO("Entered %s\n", __func__);

	if (!card->dev->dma_mask)
		card->dev->dma_mask = &dma_mask;
	if (!card->dev->coherent_dma_mask)
		card->dev->coherent_dma_mask = DMA_BIT_MASK(32);

	if (pcm->streams[SNDRV_PCM_STREAM_PLAYBACK].substream) {
		ret = preallocate_dma_buffer(pcm,
			SNDRV_PCM_STREAM_PLAYBACK);
		if (ret)
			goto out;
	}

	if (pcm->streams[SNDRV_PCM_STREAM_CAPTURE].substream) {
		ret = preallocate_dma_buffer(pcm,
			SNDRV_PCM_STREAM_CAPTURE);
		if (ret)
			goto out;
	}
out:
	return ret;
}

static struct snd_soc_platform_driver lombo_soc_platform = {
	.ops		= &mock_dma_ops,
	.pcm_new	= mock_pcm_new,
	.pcm_free	= mock_free_dma_buffers,
};

int mock_dma_platform_register(struct device *dev,
					struct mock_pcm_config *config)
{
	int ret;
	g_mock.config.i2s_base = config->i2s_base;
	g_mock.config.i2s_irq = config->i2s_irq;
	g_mock.config.pcm_hardware = config->pcm_hardware;
	g_mock.config.prealloc_buffer_size = config->prealloc_buffer_size;

	ret = devm_request_threaded_irq(dev, config->i2s_irq,
				lombo_i2s_top_handler,
				lombo_i2s_bottom_handler,
				IRQF_ONESHOT, "i2s", dev);
	if (ret) {
		PRT_ERR("request irq err\n");
		return ret;
	}

	return snd_soc_register_platform(dev, &lombo_soc_platform);
}
EXPORT_SYMBOL_GPL(mock_dma_platform_register);

void mock_dma_platform_unregister(struct device *dev)
{
	struct snd_soc_platform *platform = NULL;

	platform = snd_soc_lookup_platform(dev);
	if (!platform) {
		PRT_ERR("can't find platform\n");
		return;
	}
	snd_soc_remove_platform(platform);
}
EXPORT_SYMBOL_GPL(mock_dma_platform_unregister);

