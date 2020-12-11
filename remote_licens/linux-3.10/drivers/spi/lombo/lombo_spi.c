/*
 * lombo_spi.c - Driver of LomboTech SPI Controller
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

#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/of_irq.h>
#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/dma-mapping.h>
#include <linux/dmaengine.h>
#include <linux/platform_device.h>
#include <linux/spi/spi.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/uaccess.h>
#include "lombo_spi.h"

/* #define LOMBO_SPI_USE_CPU_ONLY */

#define msecs_to_loops(t) (loops_per_jiffy / 1000 * HZ * t)

#define RXBUSY				(1 << 2)
#define TXBUSY				(1 << 3)
#define SPI_MAX_PAGES			(32)

#define LOMBO_SPI_BAUD_LOW		(24  * 1000 * 1000)
#define LOMBO_SPI_BAUD_HIGH		(50  * 1000 * 1000)
#define LOMBO_SPI_BAUD_HIGH_LV1		(75  * 1000 * 1000)
#define LOMBO_SPI_BAUD_HIGH_LV2		(100 * 1000 * 1000)
#define LOMBO_SPI_BAUD_HIGH_LV3		(120 * 1000 * 1000)
#define LOMBO_SPI_BAUD_HIGH_LV4		(150 * 1000 * 1000)

/**
 * enum lombo_spi_work_mode - SPI work mode.
 * @WORK_MODE_NORMAL_SPI: normal spi, only data transfer.
 * @WORK_MODE_EXTENDED_SPI: extended spi, cmd + addr + (waitcycle) + data.
 */
enum lombo_spi_work_mode {
	WORK_MODE_NORMAL_SPI,
	WORK_MODE_EXTENDED_SPI,
};

/**
 * enum lombo_spi_xfer_mode - Runtime control data for SPI device.
 * @XFER_MODE_TX_RX: full duplex, transmit and receive.
 * @XFER_MODE_TX_ONLY: half duplex, transmit only.
 * @XFER_MODE_RX_ONLY: half duplex, receive only.
 * @XFER_MODE_EEPROM_READ: half duplex, eeprom read.
 */
enum lombo_spi_xfer_mode {
	XFER_MODE_TX_RX,
	XFER_MODE_TX_ONLY,
	XFER_MODE_RX_ONLY,
	XFER_MODE_EEPROM_READ,
	XFER_MODE_NULL,
};

/**
 * struct lombo_spi_ctrldata - Runtime control data for SPI device.
 * @frame_format: standard/dual/quad/octal mode.
 * @work_mode: spi work mode, normal or extended spi.
 */
struct lombo_spi_ctrldata {
	u32 frame_format;
	u32 work_mode;
};

/**
 * struct lombo_spi_dma_data - Runtime DMA holder for SPI driver.
 * @direction: dma transfer direction.
 * @ch: dma channel.
 * @nents: the number of entries in the sglist.
 */
struct lombo_spi_dma_data {
	enum dma_transfer_direction direction;
	struct dma_chan *ch;
	int nents;
	struct scatterlist sg[SPI_MAX_PAGES];
};

/**
 * struct lombo_spi - Runtime info holder for SPI driver.
 * @master: Pointer to the SPI Protocol master.
 * @pdev: Pointer to the SPI platform device.
 * @base_addr: Pointer to virtual address of SPI master.
 * @base_addr_phy: Physical address of SPI master.
 * @name: Module name.
 * @irq: Irq No.
 * @lines: Support max lines.
 * @state: Flags to indicate status.
 * @cur_mode: The active configuration of the controller.
 * @cur_bpw: The active bits per word settings.
 * @cur_baud_rate: The active xfer clock speed.
 * @cur_wc: The active wait cycle of the controller.
 * @cur_tx_ndf: The tx ndf for xfer.
 * @cur_rx_ndf: The rx ndf for xfer.
 * @init_baud_rate: Spi init baud rate.
 * @max_baud_rate: Spi max baud rate.
 * @ahb_gate: Pointer to ahb gate clock.
 * @ahb_reset: Pointer to ahb reset clock.
 * @spi_clk: Pointer to spi module clock.
 * @parent: Pointer to spi parent clock.
 * @pctrl: Pointer to spi pinctrl handle.
 * @cs_active: Interface to cs control.
 * @txrx_bufs: Interface to transfer in cpu mode.
 * @cs_bitmap: cs0:0x1, cs1:0x2, cs0 & cs1:0x3.
 * @tx_dma: TX DMA.
 * @rx_dma: RX DMA.
 * @lock: Spin lock.
 * @xfer_completion: Wakeup another spi transfer.
 * @cur_ff: Current spi frame format.
 * @cur_wm: Current spi work mode.
 * @cur_xm: Current spi xfer mode.
 */
struct lombo_spi {
	struct spi_master *master;
	struct platform_device *pdev;

	void __iomem *base_addr;
	unsigned int base_addr_phy;

	char name[16];
	unsigned int irq;
	unsigned int lines;

	unsigned int state;
	unsigned int cur_mode;
	unsigned int cur_bpw;
	unsigned int cur_baud_rate;
	unsigned int cur_wc;

	unsigned int cur_tx_ndf;
	unsigned int cur_rx_ndf;

	unsigned int init_baud_rate;
	unsigned int max_baud_rate;

	struct clk *ahb_gate;
	struct clk *ahb_reset;
	struct clk *spi_clk;
	struct clk *parent;

	struct pinctrl *pctrl;

#ifdef LOMBO_SPI_CS_SW
	void (*cs_active)(struct spi_device *spi, u32 active);
#endif
	int (*txrx_bufs)(struct spi_device *spi, struct spi_transfer *t);

	unsigned int cs_bitmap;

	struct lombo_spi_dma_data tx_dma;
	struct lombo_spi_dma_data rx_dma;

	spinlock_t lock;
	struct completion xfer_completion;

	enum lombo_spi_frame_format cur_ff;
	enum lombo_spi_work_mode cur_wm;
	enum lombo_spi_xfer_mode cur_xm;
};

static int lombo_spi_set_gpio_state(struct lombo_spi *espi, u32 baud);

#ifdef LOMBO_SPI_CS_SW
/**
 * lombo_spi_cs_active - select or deselect one cs.
 * @spi: pointer to spi device.
 * @active: 0:select cs; 1:deselect cs;
 */
static void lombo_spi_cs_active(struct spi_device *spi, u32 active)
{
	struct lombo_spi *espi = spi_master_get_devdata(spi->master);
	csp_spi_cs_active_cfg(espi->base_addr, spi->chip_select, active);
}
#endif

/**
 * lombo_spi_cs_check - check cs valid or not.
 * @spi: pointer to spi device.
 * @cs_num: the number of cs.
 *
 * Return 0 if valid, !0 error.
 */
static int lombo_spi_cs_check(struct spi_device *spi, int cs_num)
{
	struct lombo_spi *espi = spi_master_get_devdata(spi->master);
	int err = 0;

	if (cs_num >= spi->master->num_chipselect) {
		PRT_ERR("spi%d: %s's cs(%d) is out of range(%d)\n",
				spi->master->bus_num,
				spi->modalias, spi->chip_select,
				spi->master->num_chipselect);
		err = __LINE__;
		goto out;
	}

	if (!((1 << cs_num) & espi->cs_bitmap)) {
		PRT_ERR("spi%d: unsupported cs:%d\n",
				spi->master->bus_num, cs_num);
		err = __LINE__;
		goto out;
	}

out:
	return err;
}

/**
 * lombo_spi_get_ctrldata - get lombo_spi_ctrldata
 * @spi: pointer to spi device.
 *
 * Return the pointer to lombo_spi_ctrldata if success, otherwise NULL.
 */
static struct lombo_spi_ctrldata *lombo_spi_get_ctrldata(struct spi_device *spi)
{
	struct lombo_spi_ctrldata *dcd = NULL;
	struct device_node *data_np = NULL;

	data_np = of_get_child_by_name(spi->dev.of_node, "controller-data");
	if (!data_np)
		goto out;

	dcd = kzalloc(sizeof(*dcd), GFP_KERNEL);
	if (!dcd)
		goto node_put;

	if (of_property_read_u32(data_np, "frame_format", &dcd->frame_format))
		goto dcd_free;

	if (!((dcd->frame_format == FRAME_FORMAT_STANDARD_SPI)
		|| (dcd->frame_format == FRAME_FORMAT_DUAL_SPI)
		|| (dcd->frame_format == FRAME_FORMAT_QUAD_SPI)
		|| (dcd->frame_format == FRAME_FORMAT_OCTAL_SPI))) {
		PRT_ERR("spi%d(%s): unsupported spi frame format:%d\n",
				spi->master->bus_num, spi->modalias,
				dcd->frame_format);
		goto dcd_free;
	}

	if (of_property_read_u32(data_np, "work_mode", &dcd->work_mode))
		goto dcd_free;

	if (!((dcd->work_mode == WORK_MODE_NORMAL_SPI)
		|| (dcd->work_mode == WORK_MODE_EXTENDED_SPI))) {
		PRT_ERR("spi%d(%s): unsupported spi work mode:%d\n",
				spi->master->bus_num, spi->modalias,
				dcd->work_mode);
		goto dcd_free;
	}

	goto out;

dcd_free:
	kfree(dcd);
	dcd = NULL;
node_put:
	of_node_put(data_np);
out:
	return dcd;
}

/**
 * lombo_spi_transfer_setup - setup spi transfer, including mode, baudrate etc.
 * @spi: pointer to spi device.
 * @t: pointer to spi transfer.
 *
 * Return 0 if valid, !0 error.
 */
static int lombo_spi_transfer_setup(struct spi_device *spi,
				struct spi_transfer *t)
{
	struct lombo_spi *espi = spi_master_get_devdata(spi->master);
	u32 speed;
	u8 bpw, mode;
	int err = 0;
	int rate = 0;

	/* check transfer length for normal spi */
	if (espi->cur_wm == WORK_MODE_NORMAL_SPI) {
		if (t->len == 0) {
			PRT_ERR("spi%d: transfer length is %d\n",
					spi->master->bus_num, t->len);
			return -EINVAL;
		}
	}

	/* check bits per word valid */
	if (!((t->bits_per_word == 8)
			|| (t->bits_per_word == 16)
			|| (t->bits_per_word == 32))) {
		PRT_ERR("spi%d: unsupported transfer bits per word:%d\n",
				spi->master->bus_num,
				t->bits_per_word);
		return -EINVAL;
	}

	/* xfer length must be bits_per_word aligned */
	if (t->len % (t->bits_per_word / 8)) {
		PRT_ERR("spi%d: xfer len(%u) not a multiple word size(%u)\n",
				spi->master->bus_num,
				t->len, t->bits_per_word / 8);
		return -EINVAL;
	}

	/* check speed valid */
	if (t->speed_hz > espi->max_baud_rate) {
		PRT_ERR("spi%d: transfer speed(%d) is out of range(%d)\n",
				spi->master->bus_num, t->speed_hz,
				espi->max_baud_rate);
		return -EINVAL;
	}

	mode = spi->mode;
	bpw = t->bits_per_word ? : spi->bits_per_word;
	speed = t->speed_hz ? : spi->max_speed_hz;

	/* if master's(controller) state differs from that needed by slave */
	if ((espi->cur_baud_rate != speed)
			|| (espi->cur_mode != mode)
			|| (espi->cur_bpw != bpw)) {
#ifdef CONFIG_ARCH_LOMBO_N7
		/* set spi disable */
		csp_spi_en_cfg(espi->base_addr, 0);
#endif

		/* set bit per word */
		if (espi->cur_bpw != bpw) {
			espi->cur_bpw = bpw;
			csp_spi_dfs_cfg(espi->base_addr, espi->cur_bpw);
		}

		/* update polarity and phase */
		if (espi->cur_mode != mode) {
			espi->cur_mode = mode;
			csp_spi_mode_cfg(espi->base_addr, espi->cur_mode);
		}

		/* update baud rate */
		if (espi->cur_baud_rate != speed) {
			espi->cur_baud_rate = speed;
			rate = espi->cur_baud_rate * LOMBO_SPI_CLK_FACTOR;

			/* disable spi clk */
			clk_disable_unprepare(espi->spi_clk);

			/* put spi parent clk */
			clk_put(espi->parent);

			if (espi->cur_baud_rate <=
				(LOMBO_SPI_BAUD_LOW / LOMBO_SPI_CLK_FACTOR))
				espi->parent = of_clk_get(espi->pdev->dev.of_node, 3);
			else
				espi->parent = of_clk_get(espi->pdev->dev.of_node, 4);
			if (IS_ERR(espi->parent)) {
				PRT_ERR("spi%d: get parent clk failed\n",
						espi->master->bus_num);
				return -EINVAL;
			}

			/* set spi parent clk */
			if (clk_set_parent(espi->spi_clk, espi->parent)) {
				PRT_ERR("spi%d: set parent clk failed\n",
						espi->master->bus_num);
				return -EINVAL;
			}

			/* set spi clk rate */
			if (clk_set_rate(espi->spi_clk, rate)) {
				PRT_ERR("spi%d: set clk rate to %d failed\n",
						espi->master->bus_num, rate);
				return -EINVAL;
			}

			/* enable spi clk */
			if (clk_prepare_enable(espi->spi_clk)) {
				PRT_ERR("spi%d: enable spi clock failed\n",
						espi->master->bus_num);
				return -EINVAL;
			}

			err = lombo_spi_set_gpio_state(espi,
					espi->cur_baud_rate);
			if (err) {
				PRT_ERR("spi%d: set gpio state failed\n",
						espi->master->bus_num);
				return -EINVAL;
			}

			csp_spi_set_baud_rate(espi->base_addr,
					clk_get_rate(espi->spi_clk),
					espi->cur_baud_rate);

			csp_spi_rx_sample_delay_cfg(espi->base_addr,
					espi->cur_baud_rate);

			csp_spi_rx_clk_delay_cfg(espi->base_addr,
					espi->cur_baud_rate);

			PRT_DBG("clk in:%lu baud rate:%u\n",
					clk_get_rate(espi->spi_clk),
					espi->cur_baud_rate);
		}
	}

	return 0;
}

/**
 * lombo_spi_dma_callback - DMA finish callback.
 * @data: pointer to lombo_spi_dma_data.
 */
static void lombo_spi_dma_callback(void *data)
{
	struct lombo_spi_dma_data *dma_data = data;
	struct lombo_spi *espi;
	unsigned long flags;

	if (dma_data->direction == DMA_DEV_TO_MEM)
		espi = container_of(data, struct lombo_spi, rx_dma);
	else
		espi = container_of(data, struct lombo_spi, tx_dma);

	spin_lock_irqsave(&espi->lock, flags);

	if (dma_data->direction == DMA_DEV_TO_MEM) {
		PRT_DBG("spi%d: rx dma done callback\n", espi->master->bus_num);
		espi->state &= ~RXBUSY;
		complete(&espi->xfer_completion);
	} else {
		PRT_DBG("spi%d: tx dma done callback\n", espi->master->bus_num);
		espi->state &= ~TXBUSY;

		if (espi->cur_xm == XFER_MODE_TX_ONLY)
			complete(&espi->xfer_completion);
	}

	spin_unlock_irqrestore(&espi->lock, flags);
}

/**
 * lombo_spi_dma_stop - stop DMA.
 * @data: pointer to lombo_spi_dma_data.
 */
static void lombo_spi_dma_stop(struct lombo_spi_dma_data *dma_data)
{
	dmaengine_terminate_all(dma_data->ch);
}

/**
 * lombo_spi_dmg_sg_cnt - calculate sg count.
 * @addr: pointer to buffer.
 * @len: map bytes.
 *
 * Return sg conut.
 */
static int lombo_spi_dmg_sg_cnt(void *addr, int len)
{
	int npages = 0;
	char *bufp = (char *)addr;
	int mapbytes = 0;
	int bytesleft = len;

	while (bytesleft > 0) {
		if (bytesleft < (PAGE_SIZE - offset_in_page(bufp)))
			mapbytes = bytesleft;
		else
			mapbytes = PAGE_SIZE - offset_in_page(bufp);

		npages++;
		bufp += mapbytes;
		bytesleft -= mapbytes;
	}

	return npages;
}

/**
 * lombo_spi_dma_init_sg - init DMA sg config.
 * @dma_data: pointer to lombo_spi_dma_data.
 * @addr: pointer to buffer.
 * @len: transfer length.
 * @write: VERIFY_READ or VERIFY_WRITE.
 *
 * Return 0 if success, !0 error.
 */
static int lombo_spi_dma_init_sg(struct lombo_spi_dma_data *dma_data,
				void *addr, int len, int write)
{
	int i;
	int npages = 0;
	void *bufp = addr;
	int mapbytes = 0;
	int bytesleft = len;
	struct lombo_spi *espi;

	if (dma_data->direction == DMA_DEV_TO_MEM)
		espi = container_of(dma_data, struct lombo_spi, rx_dma);
	else
		espi = container_of(dma_data, struct lombo_spi, tx_dma);

	if (!access_ok(write ? VERIFY_READ : VERIFY_WRITE,
				(void __user *)addr, len))
		return -EFAULT;

	npages = lombo_spi_dmg_sg_cnt(addr, len);
	WARN_ON(npages == 0);

	PRT_DBG("spi%d: npages=%d, len=%d\n",
		espi->master->bus_num, npages, len);

	if (npages > SPI_MAX_PAGES)
		npages = SPI_MAX_PAGES;

	sg_init_table(dma_data->sg, npages);

	for (i = 0; i < npages; i++) {
		/*
		 * If there are less bytes left than what fits
		 * in the current page (plus page alignment offset)
		 * we just feed in this, else we stuff in as much
		 * as we can.
		 */
		if (bytesleft < (PAGE_SIZE - offset_in_page(bufp)))
			mapbytes = bytesleft;
		else
			mapbytes = PAGE_SIZE - offset_in_page(bufp);

		PRT_DBG("spi%d: %d: len %d, offset %ld, addr %p(%d)\n",
				espi->master->bus_num,
				i, mapbytes,
				offset_in_page(bufp),
				bufp, virt_addr_valid(bufp));

		if (virt_addr_valid(bufp))
			sg_set_page(&dma_data->sg[i], virt_to_page(bufp),
				mapbytes, offset_in_page(bufp));
		else
			sg_set_page(&dma_data->sg[i], vmalloc_to_page(bufp),
				mapbytes, offset_in_page(bufp));

		bufp += mapbytes;
		bytesleft -= mapbytes;
	}

	BUG_ON(bytesleft);

	dma_data->nents = npages;

	return 0;
}

/**
 * lombo_spi_dma_config_rx - config DMA rx.
 * @dma_data: pointer to lombo_spi_dma_data.
 * @t: pointer to spi transfer.
 *
 * Return 0 if success, !0 error.
 */
static int lombo_spi_dma_config_rx(struct lombo_spi_dma_data *dma_data,
				struct spi_transfer *t)
{
	int err = 0;
	int nents = 0;
	struct dma_slave_config dma_conf = {0};
	struct dma_async_tx_descriptor *dma_desc = NULL;
	struct lombo_spi *espi;

	if (dma_data->direction == DMA_DEV_TO_MEM)
		espi = container_of(dma_data, struct lombo_spi, rx_dma);
	else
		espi = container_of(dma_data, struct lombo_spi, tx_dma);

	PRT_DBG("spi%d: t->rx_buf=%p, t->len=%d\n",
				espi->master->bus_num, t->rx_buf, t->len);

	err = lombo_spi_dma_init_sg(dma_data, t->rx_buf, t->len, VERIFY_WRITE);
	if (err) {
		PRT_ERR("spi%d: init dma sg failed\n", espi->master->bus_num);
		return err;
	}

	dma_conf.direction = dma_data->direction;
	dma_conf.src_addr = csp_spi_get_dma_src_addr(espi->base_addr_phy);
	dma_conf.src_addr_width = espi->cur_bpw / 8;
	dma_conf.src_maxburst = 8;
	dma_conf.dst_addr_width = dma_conf.src_addr_width;
	dma_conf.dst_maxburst = dma_conf.src_maxburst;
	dmaengine_slave_config(dma_data->ch, &dma_conf);

	nents = dma_map_sg(&espi->pdev->dev, dma_data->sg,
			dma_data->nents, dma_data->direction);
	if (!nents) {
		PRT_ERR("spi%d: dma_map_sg(%d) failed! return %d\n",
				espi->master->bus_num, dma_data->nents, nents);
		return -ENOMEM;
	}

	PRT_DBG("spi%d: npages=%d, nents=%d\n",
		espi->master->bus_num, dma_data->nents, nents);

	dma_desc = dmaengine_prep_slave_sg(dma_data->ch, dma_data->sg, nents,
				dma_data->direction,
				DMA_PREP_INTERRUPT | DMA_CTRL_ACK);
	if (!dma_desc) {
		PRT_ERR("spi%d: dmaengine_prep_slave_sg() failed!\n",
				espi->master->bus_num);
		dma_unmap_sg(&espi->pdev->dev, dma_data->sg,
				dma_data->nents, dma_data->direction);
		return -EINVAL;
	}

	dma_desc->callback = lombo_spi_dma_callback;
	dma_desc->callback_param = dma_data;
	dmaengine_submit(dma_desc);

	return 0;
}

/**
 * lombo_spi_dma_config_tx - config DMA tx.
 * @dma_data: pointer to lombo_spi_dma_data.
 * @t: pointer to spi transfer.
 *
 * Return 0 if success, !0 error.
 */
static int lombo_spi_dma_config_tx(struct lombo_spi_dma_data *dma_data,
				struct spi_transfer *t)
{
	int err = 0;
	int nents = 0;
	struct dma_slave_config dma_conf = {0};
	struct dma_async_tx_descriptor *dma_desc = NULL;
	struct lombo_spi *espi;

	if (dma_data->direction == DMA_DEV_TO_MEM)
		espi = container_of(dma_data, struct lombo_spi, rx_dma);
	else
		espi = container_of(dma_data, struct lombo_spi, tx_dma);

	PRT_DBG("spi%d: t->tx_buf=%p, t->len=%d\n",
				espi->master->bus_num, t->tx_buf, t->len);

	err = lombo_spi_dma_init_sg(dma_data, (void *)t->tx_buf, t->len,
				VERIFY_READ);
	if (err) {
		PRT_ERR("spi%d: init dma sg failed\n", espi->master->bus_num);
		return err;
	}

	dma_conf.direction = dma_data->direction;
	dma_conf.dst_addr = csp_spi_get_dma_dst_addr(espi->base_addr_phy);
	dma_conf.dst_addr_width = espi->cur_bpw / 8;
	dma_conf.dst_maxburst = 8;
	dma_conf.src_addr_width = dma_conf.dst_addr_width;
	dma_conf.src_maxburst = dma_conf.dst_maxburst;
	dmaengine_slave_config(dma_data->ch, &dma_conf);

	nents = dma_map_sg(&espi->pdev->dev, dma_data->sg,
				dma_data->nents, dma_data->direction);
	if (!nents) {
		PRT_ERR("spi%d: dma_map_sg(%d) failed! return %d\n",
				espi->master->bus_num, dma_data->nents, nents);
		return -ENOMEM;
	}

	PRT_DBG("spi%d: npages=%d, nents=%d\n",
		espi->master->bus_num, dma_data->nents, nents);

	dma_desc = dmaengine_prep_slave_sg(dma_data->ch, dma_data->sg, nents,
				dma_data->direction,
				DMA_PREP_INTERRUPT | DMA_CTRL_ACK);
	if (!dma_desc) {
		PRT_ERR("spi%d: dmaengine_prep_slave_sg() failed!\n",
				espi->master->bus_num);
		dma_unmap_sg(&espi->pdev->dev, dma_data->sg,
				dma_data->nents, dma_data->direction);
		return -1;
	}

	dma_desc->callback = lombo_spi_dma_callback;
	dma_desc->callback_param = dma_data;
	dmaengine_submit(dma_desc);

	return 0;
}

/**
 * lombo_spi_dma_free_sg - free DMA sg.
 * @espi: pointer to lombo_spi.
 * @dma_data: pointer to lombo_spi_dma_data.
 */
static void lombo_spi_dma_free_sg(struct lombo_spi *espi,
				struct lombo_spi_dma_data *dma_data)
{
	dma_unmap_sg(&espi->pdev->dev, dma_data->sg,
				dma_data->nents, dma_data->direction);
}

/**
 * lombo_spi_dma_request - request DMA channel.
 * @master: pointer to spi master.
 *
 * Return 0 if success, !0 error.
 */
static int lombo_spi_dma_request(struct spi_master *master)
{
#ifdef CONFIG_LOMBO_DMAC
	struct lombo_spi *espi = spi_master_get_devdata(master);
	char dma_tx[8] = {0};
	char dma_rx[8] = {0};
	int err = 0;

	snprintf(dma_tx, sizeof(dma_tx), "spi%d-tx", master->bus_num);
	snprintf(dma_rx, sizeof(dma_rx), "spi%d-rx", master->bus_num);

	/* request dma channel */
	espi->tx_dma.ch = dma_request_slave_channel(&espi->pdev->dev, dma_tx);
	if (!espi->tx_dma.ch) {
		PRT_ERR("spi%d: request dma %s failed\n",
				master->bus_num, dma_tx);
		return -EBUSY;
	}

	/* request dma channel */
	espi->rx_dma.ch = dma_request_slave_channel(&espi->pdev->dev, dma_rx);
	if (!espi->rx_dma.ch) {
		PRT_ERR("spi%d: request dma %s failed\n",
				master->bus_num, dma_rx);
		err = -EBUSY;
		goto out;
	}
#endif

	return 0;

#ifdef CONFIG_LOMBO_DMAC
out:
	dma_release_channel(espi->tx_dma.ch);
	return err;
#endif
}

/**
 * lombo_spi_dma_release - release DMA channel.
 * @master: pointer to spi master.
 */
static void lombo_spi_dma_release(struct spi_master *master)
{
#ifdef CONFIG_LOMBO_DMAC
	struct lombo_spi *espi = spi_master_get_devdata(master);

	/* free DMA channels */
	dma_release_channel(espi->rx_dma.ch);
	dma_release_channel(espi->tx_dma.ch);
#endif
}

/**
 * lombo_spi_xfer_txrx_8 - xfer in cpu 8 bits.
 * @spi: pointer to spi device.
 * @t: pointer to spi transfer.
 *
 * Return 0 if success, !0 error.
 */
static int lombo_spi_xfer_txrx_8(struct spi_device *spi,
				struct spi_transfer *t)
{
	struct lombo_spi *espi = spi_master_get_devdata(spi->master);
	int i = 0, j = 0;
	u8 *src_p8, *dst_p8;
	void __iomem *base_addr = espi->base_addr;
	int m = 0, n = 0, err = 0;
	u32 ns_per_byte = 1000000000 / espi->cur_baud_rate * 8 / espi->cur_ff;
	u32 timeout_us = 5; /* some tolerance */

	PRT_DBG("spi%d: use txrx_%d in cpu polling mode\n",
				spi->master->bus_num, espi->cur_bpw);

	if (espi->cur_xm == XFER_MODE_TX_RX) {
		src_p8 = (u8 *)(t->tx_buf);
		dst_p8 = (u8 *)(t->rx_buf);
		timeout_us *= (espi->cur_tx_ndf * ns_per_byte / 1000 + 1);

		while (i < espi->cur_tx_ndf || j < espi->cur_rx_ndf) {
			if (i < espi->cur_tx_ndf) {
				if (csp_spi_is_tx_fifo_not_full(base_addr)) {
					csp_spi_write_data(base_addr, *src_p8);
					src_p8++;
					i++;
				} else {
					m++;
					udelay(1);
				}

				if (m > timeout_us) {
					PRT_ERR("spi%d: tx timeout, len=%d\n",
						espi->master->bus_num,
						espi->cur_tx_ndf);
					err = __LINE__;
					goto out;
				}
			}

			if (j < espi->cur_rx_ndf) {
				if (csp_spi_is_rx_fifo_not_empty(base_addr)) {
					*dst_p8 = csp_spi_read_data(base_addr);
					dst_p8++;
					j++;
				} else {
					n++;
					udelay(1);
				}

				if (n > timeout_us) {
					PRT_ERR("spi%d: rx timeout, len=%d\n",
						espi->master->bus_num,
						espi->cur_rx_ndf);
					err = __LINE__;
					goto out;
				}
			}
		}
	} else if (espi->cur_xm == XFER_MODE_TX_ONLY) {
		src_p8 = (u8 *)(t->tx_buf);
		timeout_us *= (espi->cur_tx_ndf * ns_per_byte / 1000 + 1);

		while (i < espi->cur_tx_ndf) {
			if (csp_spi_is_tx_fifo_not_full(base_addr)) {
				csp_spi_write_data(base_addr, *src_p8);
				src_p8++;
				i++;
			} else {
				m++;
				udelay(1);
			}

			if (m > timeout_us) {
				PRT_ERR("spi%d: tx timeout, len=%d\n",
						espi->master->bus_num,
						espi->cur_tx_ndf);
				err = __LINE__;
				goto out;
			}
		}
	} else {
		dst_p8 = (u8 *)(t->rx_buf);
		timeout_us *= (espi->cur_rx_ndf * ns_per_byte / 1000 + 1);

		while (i < espi->cur_rx_ndf) {
			if (csp_spi_is_rx_fifo_not_empty(base_addr)) {
				*dst_p8 = csp_spi_read_data(base_addr);
				dst_p8++;
				i++;
			} else {
				m++;
				udelay(1);
			}

			if (m > timeout_us) {
				PRT_ERR("spi%d: rx timeout, len=%d\n",
						espi->master->bus_num,
						espi->cur_rx_ndf);
				err = __LINE__;
				goto out;
			}
		}
	}

out:
	return err;
}

/**
 * lombo_spi_xfer_txrx_16 - xfer in cpu 16 bits.
 * @spi: pointer to spi device.
 * @t: pointer to spi transfer.
 *
 * Return 0 if success, !0 error.
 */
static int lombo_spi_xfer_txrx_16(struct spi_device *spi,
				struct spi_transfer *t)
{
	struct lombo_spi *espi = spi_master_get_devdata(spi->master);
	int i = 0, j = 0;
	u16 *src_p16, *dst_p16;
	void __iomem *base_addr = espi->base_addr;
	int m = 0, n = 0, err = 0;
	u32 ns_per_byte = 1000000000 / espi->cur_baud_rate * 8 / espi->cur_ff;
	u32 timeout_us = 5; /* some tolerance */

	PRT_DBG("spi%d: use txrx_%d in cpu polling mode\n",
				spi->master->bus_num, espi->cur_bpw);

	if (espi->cur_xm == XFER_MODE_TX_RX) {
		src_p16 = (u16 *)(t->tx_buf);
		dst_p16 = (u16 *)(t->rx_buf);
		timeout_us *= (espi->cur_tx_ndf * 2 * ns_per_byte / 1000 + 1);

		while (i < espi->cur_tx_ndf || j < espi->cur_rx_ndf) {
			if (i < espi->cur_tx_ndf) {
				if (csp_spi_is_tx_fifo_not_full(base_addr)) {
					csp_spi_write_data(base_addr, *src_p16);
					src_p16++;
					i++;
				} else {
					m++;
					udelay(1);
				}

				if (m > timeout_us) {
					PRT_ERR("spi%d: tx timeout, len=%d\n",
						espi->master->bus_num,
						espi->cur_tx_ndf * 2);
					err = __LINE__;
					goto out;
				}
			}

			if (j < espi->cur_rx_ndf) {
				if (csp_spi_is_rx_fifo_not_empty(base_addr)) {
					*dst_p16 = csp_spi_read_data(base_addr);
					dst_p16++;
					j++;
				} else {
					n++;
					udelay(1);
				}

				if (n > timeout_us) {
					PRT_ERR("spi%d: rx timeout, len=%d\n",
						espi->master->bus_num,
						espi->cur_rx_ndf * 2);
					err = __LINE__;
					goto out;
				}
			}
		}
	} else if (espi->cur_xm == XFER_MODE_TX_ONLY) {
		src_p16 = (u16 *)(t->tx_buf);
		timeout_us *= (espi->cur_tx_ndf * 2 * ns_per_byte / 1000 + 1);

		while (i < espi->cur_tx_ndf) {
			if (csp_spi_is_tx_fifo_not_full(base_addr)) {
				csp_spi_write_data(base_addr, *src_p16);
				src_p16++;
				i++;
			} else {
				m++;
				udelay(1);
			}

			if (m > timeout_us) {
				PRT_ERR("spi%d: tx timeout, len=%d\n",
						espi->master->bus_num,
						espi->cur_tx_ndf * 2);
				err = __LINE__;
				goto out;
			}
		}
	} else {
		dst_p16 = (u16 *)(t->rx_buf);
		timeout_us *= (espi->cur_rx_ndf * 2 * ns_per_byte / 1000 + 1);

		while (i < espi->cur_rx_ndf) {
			if (csp_spi_is_rx_fifo_not_empty(base_addr)) {
				*dst_p16 = csp_spi_read_data(base_addr);
				dst_p16++;
				i++;
			} else {
				m++;
				udelay(1);
			}

			if (m > timeout_us) {
				PRT_ERR("spi%d: rx timeout, len=%d\n",
						espi->master->bus_num,
						espi->cur_rx_ndf * 2);
				err = __LINE__;
				goto out;
			}
		}
	}

out:
	return err;
}

/**
 * lombo_spi_xfer_txrx_32 - xfer in cpu 32 bits.
 * @spi: pointer to spi device.
 * @t: pointer to spi transfer.
 *
 * Return 0 if success, !0 error.
 */
static int lombo_spi_xfer_txrx_32(struct spi_device *spi,
				struct spi_transfer *t)
{
	struct lombo_spi *espi = spi_master_get_devdata(spi->master);
	int i = 0, j = 0;
	u32 *src_p32, *dst_p32;
	void __iomem *base_addr = espi->base_addr;
	int m = 0, n = 0, err = 0;
	u32 ns_per_byte = 1000000000 / espi->cur_baud_rate * 8 / espi->cur_ff;
	u32 timeout_us = 5; /* some tolerance */

	PRT_DBG("spi%d: use txrx_%d in cpu polling mode\n",
				spi->master->bus_num, espi->cur_bpw);

	if (espi->cur_xm == XFER_MODE_TX_RX) {
		src_p32 = (u32 *)(t->tx_buf);
		dst_p32 = (u32 *)(t->rx_buf);
		timeout_us *= (espi->cur_tx_ndf * 4 * ns_per_byte / 1000 + 1);

		while (i < espi->cur_tx_ndf || j < espi->cur_rx_ndf) {
			if (i < espi->cur_tx_ndf) {
				if (csp_spi_is_tx_fifo_not_full(base_addr)) {
					csp_spi_write_data(base_addr, *src_p32);
					src_p32++;
					i++;
				} else {
					m++;
					udelay(1);
				}

				if (m > timeout_us) {
					PRT_ERR("spi%d: tx timeout, len=%d\n",
						espi->master->bus_num,
						espi->cur_tx_ndf * 4);
					err = __LINE__;
					goto out;
				}
			}

			if (j < espi->cur_rx_ndf) {
				if (csp_spi_is_rx_fifo_not_empty(base_addr)) {
					*dst_p32 = csp_spi_read_data(base_addr);
					dst_p32++;
					j++;
				} else {
					n++;
					udelay(1);
				}

				if (n > timeout_us) {
					PRT_ERR("spi%d: rx timeout, len=%d\n",
						espi->master->bus_num,
						espi->cur_rx_ndf * 4);
					err = __LINE__;
					goto out;
				}
			}
		}
	} else if (espi->cur_xm == XFER_MODE_TX_ONLY) {
		src_p32 = (u32 *)(t->tx_buf);
		timeout_us *= (espi->cur_tx_ndf * 4 * ns_per_byte / 1000 + 1);

		while (i < espi->cur_tx_ndf) {
			if (csp_spi_is_tx_fifo_not_full(base_addr)) {
				csp_spi_write_data(base_addr, *src_p32);
				src_p32++;
				i++;
			} else {
				m++;
				udelay(1);
			}

			if (m > timeout_us) {
				PRT_ERR("spi%d: tx timeout, len=%d\n",
						espi->master->bus_num,
						espi->cur_tx_ndf * 4);
				err = __LINE__;
				goto out;
			}
		}
	} else {
		dst_p32 = (u32 *)(t->rx_buf);
		timeout_us *= (espi->cur_rx_ndf * 4 * ns_per_byte / 1000 + 1);

		while (i < espi->cur_rx_ndf) {
			if (csp_spi_is_rx_fifo_not_empty(base_addr)) {
				*dst_p32 = csp_spi_read_data(base_addr);
				dst_p32++;
				i++;
			} else {
				m++;
				udelay(1);
			}

			if (m > timeout_us) {
				PRT_ERR("spi%d: rx timeout, len=%d\n",
						espi->master->bus_num,
						espi->cur_rx_ndf * 4);
				err = __LINE__;
				goto out;
			}
		}
	}

out:
	return err;
}

/**
 * lombo_spi_datapath_setup - setup data path in DMA or CPU.
 * @spi: pointer to spi device.
 * @t: pointer to spi transfer.
 * @dma_mode: use DMA or not.
 *
 * Return 0 if success, !0 error.
 */
static int lombo_spi_datapath_setup(struct spi_device *spi,
				struct spi_transfer *t, int dma_mode)
{
	struct lombo_spi *espi = spi_master_get_devdata(spi->master);
	unsigned long flags;
	int err = 0;

	if (dma_mode) {
		if (t->tx_buf) {
			spin_lock_irqsave(&espi->lock, flags);
			espi->state |= TXBUSY;
			spin_unlock_irqrestore(&espi->lock, flags);
			err = lombo_spi_dma_config_tx(&espi->tx_dma, t);
			if (err) {
				PRT_ERR("spi%d: config dma tx failed\n",
						espi->master->bus_num);
				goto out;
			}
		}

		if (t->rx_buf) {
			spin_lock_irqsave(&espi->lock, flags);
			espi->state |= RXBUSY;
			spin_unlock_irqrestore(&espi->lock, flags);
			err = lombo_spi_dma_config_rx(&espi->rx_dma, t);
			if (err) {
				PRT_ERR("spi%d: config dma rx failed\n",
						espi->master->bus_num);
				goto out;
			}
		}
	} else {
		spin_lock_irqsave(&espi->lock, flags);

		switch (espi->cur_xm) {
		case XFER_MODE_TX_RX:
			espi->state |= TXBUSY;
			espi->state |= RXBUSY;
			break;
		case XFER_MODE_TX_ONLY:
			espi->state |= TXBUSY;
			break;
		default:
			espi->state |= RXBUSY;
			break;
		}

		spin_unlock_irqrestore(&espi->lock, flags);

		switch (espi->cur_bpw) {
		case 32:
			espi->txrx_bufs = lombo_spi_xfer_txrx_32;
			break;
		case 16:
			espi->txrx_bufs = lombo_spi_xfer_txrx_16;
			break;
		default:
			espi->txrx_bufs = lombo_spi_xfer_txrx_8;
			break;
		}
	}

out:
	return err;
}

/**
 * lombo_normal_spi_setup - transfer in normal spi.
 * @spi: pointer to spi device.
 * @t: pointer to spi transfer.
 *
 * Return 0 if success, !0 error.
 */
static int lombo_normal_spi_setup(struct spi_device *spi,
				struct spi_transfer *t)
{
	PRT_DBG("spi%d: setup normal spi\n", spi->master->bus_num);

	return 0;
}

/**
 * lombo_extended_spi_setup - transfer in extended spi.
 * @spi: pointer to spi device.
 * @t: pointer to spi transfer.
 *
 * Return 0 if success, !0 error.
 */
static int lombo_extended_spi_setup(struct spi_device *spi,
				struct spi_transfer *t)
{
	struct lombo_spi *espi = spi_master_get_devdata(spi->master);
	struct lombo_spi_operate_para *esop = t->esop;

#if defined(CONFIG_ARCH_LOMBO_N7)
	if (espi->cur_xm == XFER_MODE_NULL) {
#elif defined(CONFIG_ARCH_LOMBO_N9)
	if (espi->cur_xm == XFER_MODE_NULL) {
#endif
		PRT_ERR("spi%d: current transfer mode unsupported: %d\n",
				spi->master->bus_num, espi->cur_xm);
		return -EINVAL;
	}

	if ((esop->ff == FRAME_FORMAT_DUAL_SPI)
			&& (espi->lines < 2)) {
		PRT_ERR("spi%d: max lines:%d invalid for current transfer:%d\n",
				spi->master->bus_num, espi->lines, esop->ff);
		return -EINVAL;
	} else if ((esop->ff == FRAME_FORMAT_QUAD_SPI)
			&& (espi->lines < 4)) {
		PRT_ERR("spi%d: max lines:%d invalid for current transfer:%d\n",
				spi->master->bus_num, espi->lines, esop->ff);
		return -EINVAL;
	} else if ((esop->ff == FRAME_FORMAT_OCTAL_SPI)
			&& (espi->lines < 8)) {
		PRT_ERR("spi%d: max lines:%d invalid for current transfer:%d\n",
				spi->master->bus_num, espi->lines, esop->ff);
		return -EINVAL;
	}

	espi->cur_tx_ndf = espi->cur_rx_ndf = 0;

	if (espi->cur_xm == XFER_MODE_TX_ONLY)
		espi->cur_tx_ndf = t->len / ((espi->cur_bpw / 8));
#if defined(CONFIG_ARCH_LOMBO_N7)
	else if ((espi->cur_xm == XFER_MODE_EEPROM_READ)
		|| (espi->cur_xm == XFER_MODE_RX_ONLY))
		espi->cur_rx_ndf = t->len / ((espi->cur_bpw / 8));
#elif defined(CONFIG_ARCH_LOMBO_N9)
	else if (espi->cur_xm == XFER_MODE_RX_ONLY)
		espi->cur_rx_ndf = t->len / ((espi->cur_bpw / 8));
#endif
	else if (espi->cur_xm == XFER_MODE_TX_RX)
		espi->cur_tx_ndf = espi->cur_rx_ndf = t->len /
						((espi->cur_bpw / 8));

	PRT_DBG("spi%d: tn:%d rn:%d ff:%d xm:%d cmd:0x%x al:%d ad:0x%x wc:%d\n",
		spi->master->bus_num, espi->cur_tx_ndf, espi->cur_rx_ndf,
		esop->ff, espi->cur_xm, esop->inst, esop->addr_bit_len,
		esop->addr[0], esop->wait_cycle);

	/* config data frame num */
#if defined(CONFIG_ARCH_LOMBO_N7)
	csp_spi_ndf_cfg(espi->base_addr, espi->cur_tx_ndf, espi->cur_rx_ndf);
#elif defined(CONFIG_ARCH_LOMBO_N9)
	csp_spi_tx_ndf_cfg(espi->base_addr, espi->cur_tx_ndf);
	csp_spi_rx_ndf_cfg(espi->base_addr, espi->cur_rx_ndf);
#endif

	/* config frame format */
	if (esop->ff != espi->cur_ff) {
		csp_spi_frame_format_cfg(espi->base_addr, esop->ff);
		espi->cur_ff = esop->ff;
	}

	/* config transfer mode */
	csp_spi_transfer_mode_cfg(espi->base_addr, espi->cur_xm);

	/* config command */
	csp_spi_inst_cfg(espi->base_addr, esop->inst);

	/* config address length */
	csp_spi_addr_len_cfg(espi->base_addr, esop->addr_bit_len);

	/* config address0 */
	if (esop->addr_bit_len)
		csp_spi_addr0_cfg(espi->base_addr, esop->addr[0]);

	/* config wait cycle */
	if (esop->wait_cycle != espi->cur_wc) {
		espi->cur_wc = esop->wait_cycle;
		csp_spi_wait_cycle_cfg(espi->base_addr, esop->wait_cycle);
	}

	return 0;
}

/**
 * lombo_spi_work_mode_setup - setup spi work mode.
 * @spi: pointer to spi device.
 * @t: pointer to spi transfer.
 *
 * Return 0 if success, !0 error.
 */
static int lombo_spi_work_mode_setup(struct spi_device *spi,
				struct spi_transfer *t)
{
	struct lombo_spi *espi = spi_master_get_devdata(spi->master);
	struct lombo_spi_ctrldata *dcd = spi->controller_data;
	int err = 0;

	if (dcd && (espi->cur_ff != dcd->frame_format)) {
		csp_spi_frame_format_cfg(espi->base_addr, dcd->frame_format);
		espi->cur_ff = dcd->frame_format;
	}

	espi->cur_wm = dcd ? dcd->work_mode : WORK_MODE_EXTENDED_SPI;

	/* in normal spi, tx rx buffer can not be NULL at the same time */
	if (t->tx_buf == NULL && t->rx_buf == NULL) {
		if (espi->cur_wm == WORK_MODE_NORMAL_SPI) {
			PRT_ERR("spi%d: transfer tx and rx buf is null\n",
					espi->master->bus_num);
			err = -EINVAL;
		} else {
			espi->cur_xm = XFER_MODE_TX_ONLY;
		}

		goto out;
	}

	if (t->tx_buf && t->rx_buf) {
		espi->cur_xm = XFER_MODE_TX_RX;
	} else if (t->tx_buf) {
		espi->cur_xm = XFER_MODE_TX_ONLY;
	} else if (t->rx_buf) {
#if defined(CONFIG_ARCH_LOMBO_N7)
		if (espi->cur_wm == WORK_MODE_EXTENDED_SPI) {
			if (t->esop->ff == FRAME_FORMAT_STANDARD_SPI)
				espi->cur_xm = XFER_MODE_EEPROM_READ;
			else
				espi->cur_xm = XFER_MODE_RX_ONLY;
		} else {
			espi->cur_xm = XFER_MODE_RX_ONLY;
		}
#elif defined(CONFIG_ARCH_LOMBO_N9)
		espi->cur_xm = XFER_MODE_RX_ONLY;
#endif
	}

out:
	return err;
}

/**
 * lombo_spi_xfer_start - setup data path in DMA or CPU.
 * @spi: pointer to spi device.
 * @t: pointer to spi transfer.
 * @dma_mode: use DMA or not.
 *
 * Return 0 if success, !0 error.
 */
static int lombo_spi_xfer_start(struct spi_device *spi,
				struct spi_transfer *t, int dma_mode)
{
	struct lombo_spi *espi = spi_master_get_devdata(spi->master);
	int err = 0;

	PRT_DBG("spi%d: one transfer start in %s mode\n",
		spi->master->bus_num, dma_mode ? "DMA" : "CPU");

	/* clear all pending */
	csp_spi_all_pending_clear(espi->base_addr);

	if (dma_mode) {
		/* enable spi dma tx */
		if (t->tx_buf && (espi->state & TXBUSY))
			csp_spi_dma_tx_en_cfg(espi->base_addr, 1);

		/* enable spi dma rx */
		if (t->rx_buf && (espi->state & RXBUSY))
			csp_spi_dma_rx_en_cfg(espi->base_addr, 1);

		/* enable spi bus for transfer start */
#if defined(CONFIG_ARCH_LOMBO_N7)
		csp_spi_en_cfg(espi->base_addr, 1);
#elif defined(CONFIG_ARCH_LOMBO_N9)
		csp_spi_start_sel(espi->base_addr, 1);
#endif

		if (t->tx_buf && (espi->state & TXBUSY))
			dma_async_issue_pending(espi->tx_dma.ch);

		if (t->rx_buf && (espi->state & RXBUSY))
			dma_async_issue_pending(espi->rx_dma.ch);
	} else {
		/* enable spi bus for transfer start */
#if defined(CONFIG_ARCH_LOMBO_N7)
		csp_spi_en_cfg(espi->base_addr, 1);
#elif defined(CONFIG_ARCH_LOMBO_N9)
		csp_spi_start_sel(espi->base_addr, 1);
#endif

		/* transfer in cpu polling mode */
		err = espi->txrx_bufs(spi, t);
	}

	return err;
}

/**
 * lombo_spi_wait_xfer_completion - wait for transfer completion.
 * @spi: pointer to spi device.
 * @t: pointer to spi transfer.
 * @dma_mode: use DMA or not.
 *
 * Return 0 if success, !0 error.
 */
static int lombo_spi_wait_xfer_completion(struct spi_device *spi,
				struct spi_transfer *t, int dma_mode)
{
	struct lombo_spi *espi = spi_master_get_devdata(spi->master);
	unsigned long val;
	int ms;
	u32 fsm_end = 0;

	/* millisecs to xfer 'len' bytes @ 'cur_baud_rate' */
	ms = (t->len * 8 * 1000 / espi->cur_baud_rate) / espi->cur_ff;
	ms += 10; /* some tolerance */

	if (dma_mode) {
		val = msecs_to_jiffies(ms) + 10;
		val = wait_for_completion_timeout(&espi->xfer_completion, val);
		if (!val) {
			PRT_ERR("spi%d: xfer complete time out in DMA mode\n",
					spi->master->bus_num);
			return -EIO;
		}
	} else {
		val = msecs_to_loops(ms);
		fsm_end = csp_spi_fsm_finish(espi->base_addr);

		/* wait fsm finished */
		while ((!fsm_end) && --val) {
			cpu_relax();
			fsm_end = csp_spi_fsm_finish(espi->base_addr);
		}

		if (!val) {
			PRT_ERR("spi%d: xfer complete time out in CPU mode\n",
					spi->master->bus_num);
			return -EIO;
		}
	}

	if (dma_mode) {
		/*
		 * DmaTx returns after simply writing data in the FIFO,
		 * w/o waiting for real transmission on the bus to finish.
		 * DmaRx returns only after Dma read data from FIFO which
		 * needs bus transmission to finish, so we don't worry if
		 * Xfer involved Rx(with or without Tx).
		 */
		if (espi->cur_xm == XFER_MODE_TX_ONLY) {
			val = msecs_to_loops(10);
			fsm_end = csp_spi_fsm_finish(espi->base_addr);
			while ((!fsm_end) && --val) {
				cpu_relax();
				fsm_end = csp_spi_fsm_finish(espi->base_addr);
			}

			if (!val) {
				PRT_ERR("spi%d: tx fifo time out in DMA mode\n",
						spi->master->bus_num);
				return -EIO;
			}
		}
	}

	return 0;
}

/**
 * lombo_spi_xfer_finish - transfer finish.
 * @spi: pointer to spi device.
 * @t: pointer to spi transfer.
 * @dma_mode: use DMA or not.
 *
 * Return 0 if success, !0 error.
 */
static void lombo_spi_xfer_finish(struct spi_device *spi,
				struct spi_transfer *t, int dma_mode)
{
	struct lombo_spi *espi = spi_master_get_devdata(spi->master);
	unsigned long flags = 0;

	if (dma_mode) {
		spin_lock_irqsave(&espi->lock, flags);

		if (t->tx_buf) {
			/* disable spi dma tx */
			csp_spi_dma_tx_en_cfg(espi->base_addr, 0);
			/* free tx dma sg */
			lombo_spi_dma_free_sg(espi, &espi->tx_dma);
		}

		if (t->rx_buf) {
			/* disable spi dma rx */
			csp_spi_dma_rx_en_cfg(espi->base_addr, 0);
			/* free rx dma sg */
			lombo_spi_dma_free_sg(espi, &espi->rx_dma);
		}

		spin_unlock_irqrestore(&espi->lock, flags);
	}

#ifdef CONFIG_ARCH_LOMBO_N7
	/* disable spi bus */
	csp_spi_en_cfg(espi->base_addr, 0);
#endif
}

/**
 * lombo_spi_transfer_one_message - transfer one message.
 * @master: pointer to spi master.
 * @message: pointer to spi message.
 *
 * Return 0 if success, !0 error.
 */
static int lombo_spi_transfer_one_message(struct spi_master *master,
				struct spi_message *message)
{
	struct lombo_spi *espi = spi_master_get_devdata(master);
	struct spi_device *spi = message->spi;
	struct spi_transfer *t;
	int status = 0, cs_change = 1;
	int use_dma = 0;
	unsigned long flags;

	PRT_DBG("spi%d: one message start\n", master->bus_num);

	list_for_each_entry(t, &message->transfers, transfer_list) {
		/* reinitialize xfer_completion */
		INIT_COMPLETION(espi->xfer_completion);

		/* setup spi work mode */
		status = lombo_spi_work_mode_setup(spi, t);
		if (status)
			goto out;

		/* setup spi transfer, including cs, clk, etc... */
		status = lombo_spi_transfer_setup(spi, t);
		if (status)
			goto out;

		PRT_DBG("spi%d: mode:%d speed:%d bpw:%d work mode:%s\n",
				master->bus_num,
				espi->cur_mode,
				espi->cur_baud_rate,
				espi->cur_bpw,
				espi->cur_wm == WORK_MODE_NORMAL_SPI
					? "normal spi"
					: "extended spi");
#ifdef CONFIG_ARCH_DMA_ADDR_T_64BIT
		PRT_DBG("spi%d: len:%d tbf:%p rbf:%p tdma:0x%llx rdma:0x%llx\n",
				master->bus_num,
				t->len, t->tx_buf, t->rx_buf,
				t->tx_dma, t->rx_dma);
#else
		PRT_DBG("spi%d: len:%d tbf:%p rbf:%p tdma:0x%x rdma:0x%x\n",
				master->bus_num,
				t->len, t->tx_buf, t->rx_buf,
				t->tx_dma, t->rx_dma);
#endif

		/* setup extended spi or normal spi */
		if (espi->cur_wm == WORK_MODE_EXTENDED_SPI)
			status = lombo_extended_spi_setup(spi, t);
		else
			status = lombo_normal_spi_setup(spi, t);
		if (status)
			goto out;

		spin_lock_irqsave(&espi->lock, flags);

		/* pending only which is to be done */
		espi->state &= ~RXBUSY;
		espi->state &= ~TXBUSY;

		spin_unlock_irqrestore(&espi->lock, flags);

#ifndef LOMBO_SPI_USE_CPU_ONLY
		use_dma = 0;
		/* polling method for xfers not bigger than fifo capacity */
		if (espi->rx_dma.ch
			&& espi->tx_dma.ch
			&& (t->len > LOMBO_SPI_FIFO_MAX_CAP)) {
			use_dma = 1;
		}
#endif

		/* setup spi data path */
		status = lombo_spi_datapath_setup(spi, t, use_dma);
		if (status)
			goto out;

		/* enable chip select */
		if (cs_change) {
			/* set cs enable */
			csp_spi_cs_en_cfg(espi->base_addr, spi->chip_select, 1);
#ifdef LOMBO_SPI_CS_SW
			/* set cs active */
			espi->cs_active(spi, 1);
#endif
		}

		/* update the new cs value */
		cs_change = t->cs_change;

		/* start xfer */
		status = lombo_spi_xfer_start(spi, t, use_dma);
		if (status)
			goto cs_release;

		/* wait for transfer completion */
		status = lombo_spi_wait_xfer_completion(spi, t, use_dma);
		if (status) {
			PRT_ERR("spi%d: rx-%d tx-%d sta:rx-%c tx-%c len-%d\n",
				master->bus_num,
				t->rx_buf ? 1 : 0, t->tx_buf ? 1 : 0,
				(espi->state & RXBUSY) ? 'b' : 'i',
				(espi->state & TXBUSY) ? 'b' : 'i',
				t->len);

			if (use_dma) {
				if ((t->tx_buf)
					&& (espi->state & TXBUSY))
					lombo_spi_dma_stop(&espi->tx_dma);

				if ((t->rx_buf)
					&& (espi->state & RXBUSY))
					lombo_spi_dma_stop(&espi->rx_dma);
			}

			goto cs_release;
		}

		message->actual_length += t->len;

		/* may be need to delay */
		if (t->delay_usecs)
			udelay(t->delay_usecs);

		/* if zero, keep active, otherwise inactive */
		if (cs_change) {
#ifdef LOMBO_SPI_CS_SW
			/* set cs inactive */
			espi->cs_active(spi, 0);
#endif
			/* set cs disable */
			csp_spi_cs_en_cfg(espi->base_addr, spi->chip_select, 0);
		}

		/* xfer finish */
		lombo_spi_xfer_finish(spi, t, use_dma);

		PRT_DBG("spi%d: one transfer finish\n", master->bus_num);
	}

cs_release:
	/* fail or need to change cs */
	if (!cs_change || status) {
#ifdef LOMBO_SPI_CS_SW
		/* set cs inactive */
		espi->cs_active(spi, 0);
#endif
		/* set cs disable */
		csp_spi_cs_en_cfg(espi->base_addr, spi->chip_select, 0);
	}

	/* xfer finish */
	if (status)
		lombo_spi_xfer_finish(spi, t, use_dma);

out:
	message->status = status;
	spi_finalize_current_message(master);

	if (status == 0)
		PRT_DBG("spi%d: one message finish\n", master->bus_num);

	return status;
}

/**
 * lombo_spi_setup - check spi device transfer paras.
 * @spi: pointer to spi device.
 *
 * Return 0 if valid, !0 error.
 */
static int lombo_spi_setup(struct spi_device *spi)
{
	struct lombo_spi *espi = spi_master_get_devdata(spi->master);
	struct lombo_spi_ctrldata *dcd = NULL;

	if ((!spi->controller_data) && spi->dev.of_node)
		spi->controller_data = lombo_spi_get_ctrldata(spi);

	if (spi->controller_data) {
		dcd = (struct lombo_spi_ctrldata *)spi->controller_data;
		if ((dcd->frame_format == FRAME_FORMAT_DUAL_SPI)
			&& (espi->lines < 2)) {
			PRT_ERR("spi%d: max lines %d invalid for %s:%d\n",
					spi->master->bus_num, espi->lines,
					spi->modalias, dcd->frame_format);
			return -EINVAL;
		} else if ((dcd->frame_format == FRAME_FORMAT_QUAD_SPI)
			&& (espi->lines < 4)) {
			PRT_ERR("spi%d: max lines %d invalid for %s:%d\n",
					spi->master->bus_num, espi->lines,
					spi->modalias, dcd->frame_format);
			return -EINVAL;
		} else if ((dcd->frame_format == FRAME_FORMAT_OCTAL_SPI)
			&& (espi->lines < 8)) {
			PRT_ERR("spi%d: max lines %d invalid for %s:%d\n",
					spi->master->bus_num, espi->lines,
					spi->modalias, dcd->frame_format);
			return -EINVAL;
		}
	}

	/* only support 8/16/32 bits per word */
	if (!((spi->bits_per_word == 8)
		|| (spi->bits_per_word == 16)
		|| (spi->bits_per_word == 32))) {
		PRT_ERR("spi%d: unsupported bits_per_word %d for %s\n",
				spi->master->bus_num,
				spi->bits_per_word, spi->modalias);
		return -EINVAL;
	}

	/* check max speed */
	if (spi->max_speed_hz > espi->max_baud_rate) {
		PRT_ERR("spi%d: max frequency %d < %s(%d)\n",
				spi->master->bus_num, espi->max_baud_rate,
				spi->modalias, spi->max_speed_hz);
		return -EINVAL;
	}

	/* check cs valid */
	if (lombo_spi_cs_check(spi, spi->chip_select)) {
		PRT_ERR("spi%d: unsupported cs%d for %s\n",
				spi->master->bus_num,
				spi->chip_select, spi->modalias);
		return -EINVAL;
	}

#ifdef LOMBO_SPI_CS_SW
	/* set cs use sw */
	csp_spi_cs_by_sw(espi->base_addr, spi->chip_select);
#else
	/* set cs use hw */
	csp_spi_cs_by_hw(espi->base_addr, spi->chip_select);
#endif

	return 0;
}

/**
 * lombo_spi_cleanup - cleanup spi device resources which requested.
 * @spi: pointer to spi device.
 */
static void lombo_spi_cleanup(struct spi_device *spi)
{
	struct lombo_spi_ctrldata *dcd = spi->controller_data;

	if (dcd) {
		if (spi->dev.of_node)
			kfree(dcd);
		spi->controller_data = NULL;
	}
}

/**
 * lombo_spi_irq_handler - spi IRQ handler.
 * @irq: irq number.
 * @dev_id: cookie to identify the device.
 *
 * Return IRQ_HANDLED if success.
 */
static irqreturn_t lombo_spi_irq_handler(int irq, void *dev_id)
{
	struct lombo_spi *espi = (struct lombo_spi *)dev_id;
	unsigned int status = 0;
	unsigned long flags = 0;

	spin_lock_irqsave(&espi->lock, flags);

	/* query irq pending */
	status = csp_spi_qry_irq_pending(espi->base_addr);

	PRT_DBG("spi%d: irq status = 0x%x\n", espi->master->bus_num, status);

	/* clear irq pending */
	csp_spi_clr_irq_pending(espi->base_addr, status);

	/* check all error int_pd */
	if (csp_spi_int_en_err_rxfo(espi->base_addr, status))
		PRT_ERR("spi%d: rx fifo over flow\n", espi->master->bus_num);

	if (csp_spi_int_en_err_rxfu(espi->base_addr, status))
		PRT_ERR("spi%d: rx fifo under flow\n", espi->master->bus_num);

	if (csp_spi_int_en_err_txfo(espi->base_addr, status))
		PRT_ERR("spi%d: tx fifo over flow\n", espi->master->bus_num);

	spin_unlock_irqrestore(&espi->lock, flags);

	return IRQ_HANDLED;
}

/**
 * lombo_spi_select_gpio_state - set spi gpio state.
 * @espi: pointer to lombo spi.
 * @name: state name.
 *
 * Return 0 if success, !0 error.
 */
static int lombo_spi_select_gpio_state(struct lombo_spi *espi, char *name)
{
	int err = 0;
	struct pinctrl_state *pctrl_state = NULL;

	pctrl_state = pinctrl_lookup_state(espi->pctrl, name);
	if (IS_ERR(pctrl_state)) {
		PRT_ERR("spi%d: pinctrl_lookup_state(%s) failed! return %p\n",
				espi->master->bus_num, name, pctrl_state);
		return -EINVAL;
	}

	err = pinctrl_select_state(espi->pctrl, pctrl_state);
	if (err < 0)
		PRT_ERR("spi%d: pinctrl_select_state(%s) failed! return %d\n",
				espi->master->bus_num, name, err);

	return err;
}

static int lombo_spi_set_gpio_state(struct lombo_spi *espi, u32 baud)
{
	int err = 0;

	if (baud <= LOMBO_SPI_BAUD_HIGH)
		err = lombo_spi_select_gpio_state(espi, PINCTRL_STATE_DEFAULT);
	else if (baud <= LOMBO_SPI_BAUD_HIGH_LV1)
		err = lombo_spi_select_gpio_state(espi, "high-level1");
	else if (baud <= LOMBO_SPI_BAUD_HIGH_LV2)
		err = lombo_spi_select_gpio_state(espi, "high-level2");
	else if (baud <= LOMBO_SPI_BAUD_HIGH_LV3)
		err = lombo_spi_select_gpio_state(espi, "high-level3");
	else if (baud <= LOMBO_SPI_BAUD_HIGH_LV4)
		err = lombo_spi_select_gpio_state(espi, "high-level4");
	else
		BUG();

	return err;
}

/**
 * lombo_spi_request_gpio - request spi gpio.
 * @master: pointer to spi master.
 *
 * Return 0 if success, !0 error.
 */
static int lombo_spi_request_gpio(struct spi_master *master)
{
	struct lombo_spi *espi = spi_master_get_devdata(master);

	espi->pctrl = devm_pinctrl_get(&espi->pdev->dev);
	if (IS_ERR_OR_NULL(espi->pctrl)) {
		PRT_ERR("spi%d: devm_pinctrl_get() failed! return %ld\n",
				master->bus_num, PTR_ERR(espi->pctrl));
		return -EINVAL;
	}

	if (lombo_spi_set_gpio_state(espi, espi->init_baud_rate)) {
		PRT_ERR("spi%d: lombo_spi_set_gpio_state() failed!\n",
				master->bus_num);
		return -EINVAL;
	}

	return 0;
}

/**
 * lombo_spi_release_gpio - release spi gpio.
 * @master: pointer to spi master.
 */
static void lombo_spi_release_gpio(struct spi_master *master)
{
	struct lombo_spi *espi = spi_master_get_devdata(master);
	devm_pinctrl_put(espi->pctrl);
	espi->pctrl = NULL;
}

/**
 * lombo_spi_clk_init - init spi clock.
 * @master: pointer to spi master.
 *
 * Return 0 if success, !0 error.
 */
static int lombo_spi_clk_init(struct spi_master *master)
{
	struct lombo_spi *espi = spi_master_get_devdata(master);
	int rate = espi->init_baud_rate * LOMBO_SPI_CLK_FACTOR;
	int err = 0;

	/* get spi ahb gate */
	espi->ahb_gate = of_clk_get(espi->pdev->dev.of_node, 0);
	if (IS_ERR_OR_NULL(espi->ahb_gate)) {
		PRT_ERR("spi%d: get ahb gate clk failed\n",
				espi->master->bus_num);
		return -EINVAL;
	}

	/* enable spi ahb gate */
	err = clk_prepare_enable(espi->ahb_gate);
	if (err) {
		PRT_ERR("spi%d: enable ahb gate clk failed\n",
				espi->master->bus_num);
		return err;
	}

	/* get spi ahb reset */
	espi->ahb_reset = of_clk_get(espi->pdev->dev.of_node, 1);
	if (IS_ERR_OR_NULL(espi->ahb_reset)) {
		PRT_ERR("spi%d: get ahb reset clk failed\n",
				espi->master->bus_num);
		return -EINVAL;
	}

	/* enable spi ahb reset */
	err = clk_prepare_enable(espi->ahb_reset);
	if (err) {
		PRT_ERR("spi%d: enable ahb reset clk failed\n",
				espi->master->bus_num);
		return err;
	}

	/* get spi module clk */
	espi->spi_clk = of_clk_get(espi->pdev->dev.of_node, 2);
	if (IS_ERR(espi->spi_clk)) {
		PRT_ERR("spi%d: get clk failed\n", espi->master->bus_num);
		return -EINVAL;
	}

	/* get spi parent clk */
	if (espi->init_baud_rate <= (LOMBO_SPI_BAUD_LOW / LOMBO_SPI_CLK_FACTOR))
		espi->parent = of_clk_get(espi->pdev->dev.of_node, 3);
	else
		espi->parent = of_clk_get(espi->pdev->dev.of_node, 4);
	if (IS_ERR(espi->parent)) {
		PRT_ERR("spi%d: get parent clk failed\n",
				espi->master->bus_num);
		return -EINVAL;
	}

	/* set spi parent clk */
	err = clk_set_parent(espi->spi_clk, espi->parent);
	if (err) {
		PRT_ERR("spi%d: set parent clk failed\n",
				espi->master->bus_num);
		return err;
	}

	/* set spi clk rate */
	err = clk_set_rate(espi->spi_clk, rate);
	if (err) {
		PRT_ERR("spi%d: set clk rate to %d failed\n",
				espi->master->bus_num, rate);
		return err;
	}

	/* enable spi clk */
	err = clk_prepare_enable(espi->spi_clk);
	if (err) {
		PRT_ERR("spi%d: enable spi clock failed\n",
				espi->master->bus_num);
		return err;
	}

	return 0;
}

/**
 * lombo_spi_clk_exit - exit spi clock.
 * @master: pointer to spi master.
 */
static void lombo_spi_clk_exit(struct spi_master *master)
{
	struct lombo_spi *espi = spi_master_get_devdata(master);

	/* disable spi clk */
	clk_disable_unprepare(espi->spi_clk);
	/* put spi module clk */
	clk_put(espi->spi_clk);

	/* put spi parent clk */
	clk_put(espi->parent);

	/* disable spi ahb reset clk */
	clk_disable_unprepare(espi->ahb_reset);
	/* put spi ahb reset clk */
	clk_put(espi->ahb_reset);

	/* disable spi ahb gate clk */
	clk_disable_unprepare(espi->ahb_gate);
	/* put spi ahb gate clk */
	clk_put(espi->ahb_gate);
}

/**
 * lombo_spi_hw_init - init spi controller default state.
 * @master: pointer to spi master.
 */
static void lombo_spi_hw_init(struct spi_master *master)
{
	struct lombo_spi *espi = spi_master_get_devdata(master);

#if defined(CONFIG_ARCH_LOMBO_N7)
	/* disable spi */
	csp_spi_en_cfg(espi->base_addr, 0);

	/* disable all xip feature */
	csp_spi_xip_disable(espi->base_addr);

#elif defined(CONFIG_ARCH_LOMBO_N9)
	/* reset spi */
	csp_spi_reset_sel(espi->base_addr, 1);
#endif

	/* set spi mode */
	csp_spi_mode_cfg(espi->base_addr, espi->cur_mode);

	/* set the default frequency */
	csp_spi_set_baud_rate(espi->base_addr, clk_get_rate(espi->spi_clk),
				espi->init_baud_rate);

	/* set the default bits per word */
	csp_spi_dfs_cfg(espi->base_addr, espi->cur_bpw);

	/* set rx sample delay */
	csp_spi_rx_sample_delay_cfg(espi->base_addr, espi->init_baud_rate);

	/* set rx clk delay */
	csp_spi_rx_clk_delay_cfg(espi->base_addr, espi->init_baud_rate);

	/* set spi frame format to motorola spi */
	csp_spi_frame_format_sel(espi->base_addr, MOTOROLA_SPI);

	/* set spi frame format to standard SPI */
	csp_spi_frame_format_cfg(espi->base_addr, espi->cur_ff);

	/* set spi trans type to 1-1-x */
	csp_spi_trans_type_cfg(espi->base_addr, 0);

	/* set init wait cycle */
	csp_spi_wait_cycle_cfg(espi->base_addr, espi->cur_wc);

	/* set instruction length to 8bit */
	csp_spi_inst_len_cfg(espi->base_addr, 8);

	/* set addr0 to 0 */
	csp_spi_addr0_cfg(espi->base_addr, 0);

#ifdef CONFIG_ARCH_LOMBO_N7
	/* set addr1 to 0 */
	csp_spi_addr1_cfg(espi->base_addr, 0);
#endif

	/* set little endian if support */
	csp_spi_set_little_endian(espi->base_addr);

	/* set dma tx threshold */
	csp_spi_dma_tx_th_cfg(espi->base_addr, 8);

	/* set dma rx threshold */
	csp_spi_dma_rx_th_cfg(espi->base_addr, 8);

	/* enable spi all error interrupt - we use polling if not DMA mode */
	csp_spi_int_en_err_cfg(espi->base_addr, 1);
}

/**
 * lombo_spi_hw_exit - exit spi controller default state.
 * @master: pointer to spi master.
 */
static void lombo_spi_hw_exit(struct spi_master *master)
{
	struct lombo_spi *espi = spi_master_get_devdata(master);

#ifdef CONFIG_ARCH_LOMBO_N7
	/* disable spi controller */
	csp_spi_en_cfg(espi->base_addr, 0);
#endif

	/* disable spi all error interrupt */
	csp_spi_int_en_err_cfg(espi->base_addr, 0);

	/* disable clock */
	lombo_spi_clk_exit(master);

	/* release gpio */
	lombo_spi_release_gpio(master);
}

/**
 * lombo_spi_probe - probe function of spi master.
 * @pdev: pointer to spi master platform device.
 *
 * Return 0 if success, 0! error.
 */
static int lombo_spi_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	struct resource *mem_res;
	struct lombo_spi *espi;
	struct spi_master *master;
	char spi_para[32] = {0};
	int err = 0, irq = 0;
	u32 cs_num = 0;

	/* get spi id */
	pdev->id = of_alias_get_id(np, "spi");
	if (pdev->id < 0) {
		PRT_ERR("spi%d: failed to get alias id\n", pdev->id);
		return -EINVAL;
	}

	/* create spi master */
	master = spi_alloc_master(&pdev->dev, sizeof(struct lombo_spi));
	if (!master) {
		PRT_ERR("spi%d: failed to allocate master\n", pdev->id);
		return -ENOMEM;
	}

	/* get espi pointer */
	espi = spi_master_get_devdata(master);
	memset(espi, 0, sizeof(struct lombo_spi));

	/* get mem resource */
	mem_res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!mem_res) {
		PRT_ERR("spi%d: failed to get mem resource\n", pdev->id);
		return -EINVAL;
	}

	/* request mem region */
	if (!request_mem_region(mem_res->start,
				resource_size(mem_res), pdev->name)) {
		PRT_ERR("spi%d: failed to request mem region\n", pdev->id);
		err = -EINVAL;
		goto err_free_master;
	}

	/* get irq */
	irq = platform_get_irq(pdev, 0);
	if (irq < 0) {
		PRT_ERR("spi%d: failed to get irq\n", pdev->id);
		err = -EINVAL;
		goto err_free_mem_region;
	}

	/* get spix_number_cs in dts */
	snprintf(spi_para, sizeof(spi_para), "spi%d_cs_number", pdev->id);
	err = of_property_read_u32(np, spi_para, &cs_num);
	if (err) {
		PRT_ERR("spi%d: failed to get the number of chip select\n",
				pdev->id);
		goto err_free_mem_region;
	}

	/* get spix_cs_bitmap in dts */
	snprintf(spi_para, sizeof(spi_para), "spi%d_cs_bitmap", pdev->id);
	err = of_property_read_u32(np, spi_para, &espi->cs_bitmap);
	if (err) {
		PRT_ERR("spi%d: failed to get cs bitmap\n", pdev->id);
		goto err_free_mem_region;
	}

	/* get spix_max_lines in dts */
	snprintf(spi_para, sizeof(spi_para), "spi%d_max_lines", pdev->id);
	err = of_property_read_u32(np, spi_para, &espi->lines);
	if (err) {
		PRT_ERR("spi%d: failed to get max lines\n", pdev->id);
		goto err_free_mem_region;
	}

	/* get init-baud-rate in dts */
	err = of_property_read_u32(np, "init-baud-rate", &espi->init_baud_rate);
	if (err) {
		PRT_ERR("spi%d: failed to get init baud rate\n", pdev->id);
		goto err_free_mem_region;
	}

	/* get max-baud-rate in dts */
	err = of_property_read_u32(np, "max-baud-rate",
				&espi->max_baud_rate);
	if (err) {
		PRT_ERR("spi%d: failed to get max baud rate\n", pdev->id);
		goto err_free_mem_region;
	}

	master->dev.of_node = pdev->dev.of_node;
	master->bus_num = pdev->id;
	master->num_chipselect = cs_num;
	master->mode_bits = SPI_CPOL | SPI_CPHA;
	master->setup = lombo_spi_setup;
	master->cleanup = lombo_spi_cleanup;
	master->transfer_one_message = lombo_spi_transfer_one_message;
	master->bits_per_word_mask = BIT(32 - 1) | BIT(16 - 1) | BIT(8 - 1);
	platform_set_drvdata(pdev, master);

	espi->pdev = pdev;
	espi->irq  = irq;
	espi->master = master;
#ifdef LOMBO_SPI_CS_SW
	espi->cs_active = lombo_spi_cs_active;
#endif
	espi->tx_dma.direction = DMA_MEM_TO_DEV;
	espi->rx_dma.direction = DMA_DEV_TO_MEM;
	espi->cur_ff = FRAME_FORMAT_STANDARD_SPI;
	espi->cur_wm = WORK_MODE_EXTENDED_SPI;
	espi->cur_xm = XFER_MODE_NULL;
	espi->cur_mode = SPI_MODE_0;
	espi->cur_bpw = 8;
	espi->cur_wc  = 0;

	snprintf(espi->name, sizeof(espi->name),
			LOMBO_SPI_DEV_NAME"%d", pdev->id);
	pdev->dev.init_name = espi->name;

	spin_lock_init(&espi->lock);
	init_completion(&espi->xfer_completion);

	espi->base_addr = ioremap(mem_res->start, resource_size(mem_res));
	if (espi->base_addr == NULL) {
		PRT_ERR("spi%d: failed to ioremap\n", master->bus_num);
		err = -EINVAL;
		goto err_free_mem_region;
	}
	espi->base_addr_phy = mem_res->start;

	/* request irq */
	err = request_irq(espi->irq, lombo_spi_irq_handler,
				0, espi->name, espi);
	if (err) {
		PRT_ERR("spi%d: failed to request irq:%d\n",
				master->bus_num, espi->irq);
		goto err_free_ioremap;
	}

	/* spi gpio request */
	err = lombo_spi_request_gpio(master);
	if (err) {
		PRT_ERR("spi%d: failed to request gpio\n", master->bus_num);
		goto err_free_irq;
	}

	/* spi clock init */
	err = lombo_spi_clk_init(master);
	if (err) {
		PRT_ERR("spi%d: failed to init clk\n", master->bus_num);
		goto err_release_gpio;
	}

	/* register spi master */
	err = spi_register_master(master);
	if (err) {
		PRT_ERR("spi%d: failed to register master\n", master->bus_num);
		goto err_release_clk;
	}

	/* request dma channel */
	err = lombo_spi_dma_request(master);
	if (err) {
		PRT_ERR("spi%d: failed to request dma\n", master->bus_num);
		goto err_unregister_master;
	}

	/* set spi default config */
	lombo_spi_hw_init(master);

	espi->cur_baud_rate = csp_spi_get_baud_rate(espi->base_addr,
				clk_get_rate(espi->spi_clk));

	if (espi->cur_baud_rate != espi->init_baud_rate)
		PRT_INFO("spi%d: select %d round to %d\n",
				master->bus_num,
				espi->cur_baud_rate, espi->init_baud_rate);

	PRT_INFO("spi%d: probe succeed, irq:%d cs:%d clk in:%lu baud rate:%d\n",
		master->bus_num, espi->irq,
		master->num_chipselect,
		clk_get_rate(espi->spi_clk),
		espi->cur_baud_rate);

	return 0;

err_unregister_master:
	spi_unregister_master(master);
err_release_clk:
	lombo_spi_clk_exit(master);
err_release_gpio:
	lombo_spi_release_gpio(master);
err_free_irq:
	free_irq(espi->irq, espi);
err_free_ioremap:
	iounmap(espi->base_addr);
err_free_mem_region:
	release_mem_region(mem_res->start, resource_size(mem_res));
err_free_master:
	spi_master_put(master);

	return err;
}

/**
 * lombo_spi_remove - remove function of spi master.
 * @pdev: pointer to spi master platform device.
 *
 * Return 0 if success, 0! error.
 */
static int lombo_spi_remove(struct platform_device *pdev)
{
	struct spi_master *master = spi_master_get(platform_get_drvdata(pdev));
	struct lombo_spi *espi = spi_master_get_devdata(master);
	struct resource *mem_res;

	lombo_spi_hw_exit(master);

	lombo_spi_dma_release(master);

	spi_unregister_master(master);

	iounmap(espi->base_addr);

	mem_res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (mem_res)
		release_mem_region(mem_res->start, resource_size(mem_res));

	platform_set_drvdata(pdev, NULL);
	spi_master_put(master);

	return 0;
}

#ifdef CONFIG_PM_SLEEP
/**
 * lombo_spi_suspend - suspend function of spi master.
 * @dev: pointer to spi master device.
 *
 * Return 0 if success, 0! error.
 */
static int lombo_spi_suspend(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct spi_master *master = spi_master_get(platform_get_drvdata(pdev));

	lombo_spi_clk_exit(master);

	return 0;
}

/**
 * lombo_spi_resume - resume function of spi master.
 * @dev: pointer to spi master device.
 *
 * Return 0 if success, 0! error.
 */
static int lombo_spi_resume(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct spi_master *master = spi_master_get(platform_get_drvdata(pdev));

	lombo_spi_clk_init(master);

	return 0;
}

static const struct dev_pm_ops lombo_spi_pm_ops = {
	.suspend = lombo_spi_suspend,
	.resume  = lombo_spi_resume,
};
#endif /* CONFIG_PM_SLEEP */

static const struct of_device_id lombo_spi_match[] = {
	{ .compatible = "lombo,n7-spi", },
	{ .compatible = "lombo,n9-spi", },
	{},
};
MODULE_DEVICE_TABLE(of, lombo_spi_match);

static struct platform_driver lombo_spi_driver = {
	.probe  = lombo_spi_probe,
	.remove = lombo_spi_remove,
	.driver = {
		.name  = LOMBO_SPI_DEV_NAME,
		.owner = THIS_MODULE,
#ifdef CONFIG_PM_SLEEP
		.pm = &lombo_spi_pm_ops,
#endif
		.of_match_table = lombo_spi_match,
	},
};

static int __init lombo_spi_init(void)
{
	return platform_driver_register(&lombo_spi_driver);
}
module_init(lombo_spi_init);

static void __exit lombo_spi_exit(void)
{
	platform_driver_unregister(&lombo_spi_driver);
}
module_exit(lombo_spi_exit);

MODULE_AUTHOR("lomboswer@lombotech.com>");
MODULE_DESCRIPTION("Driver for LomboTech SPI Controller");
MODULE_LICENSE("GPL");
