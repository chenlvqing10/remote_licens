/*
 * spnd.c - spi nand driver code for LomboTech
 * spi nand driver driver code implement
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
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/time.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/platform_device.h>
#include <linux/of_platform.h>
#include <linux/highmem.h>
#include <linux/mm.h>
#include <linux/io.h>

#include <linux/spi/spi.h>

#define ENABLE_TRACE
#include <mach/debug.h>
#undef MOD_NAME
#define MOD_NAME	"SPNDPHY"

#include "include/spi_bus.h"
#include "include/nand_phy_api.h"
#include "include/nand_phy.h"
#include "include/extra.h"
#include "include/nftl.h"
#include "nand.h"

#define USE_SPI_TRANS_ASYNC	1

#define BITS_PER_WORD_8		(8)
#define BITS_PER_WORD_32	(32)

#define SPND_MIN_FREQ		(12000000)

#define SPND_STAGE_PRE		(0)
#define SPND_STAGE_POST		(1)

#ifdef ENABLE_TRACE
#define PRT_TRACE(x...)		PRT_DBG(x)
#else
#define PRT_TRACE(x...)
#endif

struct lombo_spnd {
	struct spi_device	*spi;
	void			*param;
	void			*param1;

	struct completion	completion;
	struct lombo_spi_operate_para	esop[2];
	struct spi_transfer	t[2];
	struct spi_message	m;
	u32			cur_trans_idx;

	struct spi_bus_trans	*cur_trans;

	u32			page_size;
	u32			max_freq;
	u32			freq;
	u32			spi_bw;
};

#ifdef SPND_TEST_WITH_BURN_LIB
#ifdef SPND_CHIP_TEST
static bool chip_test;
module_param(chip_test, bool, 0);
static bool force_erase;
module_param(force_erase, bool, 0);
#endif

static bool spnd_fmt;
module_param(spnd_fmt, bool, 0);

static int nftl_op = 20;
module_param(nftl_op, int, 0);

static int raw_rsvd = 4;
module_param(raw_rsvd, int, 0);
#else
static unsigned long lombo_npaddr;
#ifndef MODULE
static int __init setup_lombo_spnd(char *arg)
{
	return kstrtoul(arg, 16, &lombo_npaddr);
}
early_param("npaddr", setup_lombo_spnd);
#else	/* MODULE */
module_param(lombo_npaddr, ulong, 0);
#endif
#endif

/* temporary for test */
EXPORT_SYMBOL(get_nandphy);

/* temporary for test */
EXPORT_SYMBOL(_get_nftl);

/* temporary for test */
EXPORT_SYMBOL(_change_compressor);

/* temporary for test */
EXPORT_SYMBOL(_nftl_get_sector_size);

/* temporary for test */
EXPORT_SYMBOL(_nftl_get_sector_count);

/* temporary for test */
EXPORT_SYMBOL(_nftl_read_sector);

/* temporary for test */
EXPORT_SYMBOL(_nftl_write_sector);

/* temporary for test */
EXPORT_SYMBOL(_nftl_trim);

/* temporary for test */
EXPORT_SYMBOL(_nftl_flush);

struct lombo_spnd *g_spnd;

#ifdef SPND_HAS_EXTRA_CODE
#include "extra/extra.c"
#endif

/*
 * spi operations
 */
static void __trans_complete(void *arg)
{
	struct lombo_spnd *spnd = (struct lombo_spnd *)arg;

	complete(&spnd->completion);
}

static void __wait_for_trans_complete(struct lombo_spnd *spnd)
{
	wait_for_completion(&spnd->completion);
}

static void spnd_trans_flush_dcache(struct lombo_spnd *spnd, u32 stage)
{
	struct spi_bus_trans *trans = spnd->cur_trans;
	u8 *start;
	u8 *end;

	if (((stage == SPND_STAGE_PRE) &&
		(trans->trans_act == SPI_TRANS_READ)) ||
		((stage == SPND_STAGE_POST) &&
		(trans->trans_act == SPI_TRANS_WRITE)))
		return;

	if ((trans->bytes <= 16) ||
		(trans->bytes == spnd->page_size))
		return;

	if (!virt_addr_valid(trans->buf))
		return;

	start = trans->buf;
	end = (u8 *)PAGE_ALIGN((unsigned long)start + trans->bytes);

#if 0
	PRT_DBG("buf=0x%p,bytes=%d,cmd=%d\n",
		trans->buf, trans->bytes, trans->cmd);
#endif

	while (start < end) {
		struct page *page = virt_to_page(start);
		if (PageSlab(page)) {
			PRT_WARN("page is allocated in slab!\n");
			break;
		}

		flush_dcache_page(page);
		start += PAGE_SIZE;
	}
}

static int __spi_xfer_rw_buf(struct lombo_spnd *spnd,
				u32 frame_fmt, u32 bits_per_word,
				struct spi_bus_trans *spi_trans,
				u32 addr, void *buf, u32 bytes)
{
	struct lombo_spi_operate_para *esop;
	struct spi_transfer *t;
	struct spi_message *m;
	u32 trans_act = 0;
	int ret = 0;

	PRT_TRACE_BEGIN("frame_fmt=%d,bits_per_word=%d\n"
			"addr=%d,buf=0x%p,bytes=%d\n"
			"cmd=0x%02x,addr_bits=%d,dummy_bits=%d\n",
			frame_fmt, bits_per_word,
			addr, buf, bytes, spi_trans->cmd,
			spi_trans->addr_bits, spi_trans->dummy_bits);

#if 1
	if ((bits_per_word != BITS_PER_WORD_8) &&
		(bits_per_word != BITS_PER_WORD_32)) {
		PRT_ERR("invalid bits_per_word (%d)\n", bits_per_word);
		return -EINVAL;
	}
#endif

	t = &spnd->t[spnd->cur_trans_idx];
	esop = &spnd->esop[spnd->cur_trans_idx];
	m = &spnd->m;

	esop->inst = spi_trans->cmd;
	esop->ff = frame_fmt;
	esop->addr_bit_len = spi_trans->addr_bits;
	esop->addr[0] = addr;
	esop->wait_cycle = spi_trans->dummy_bits;

	t->bits_per_word = bits_per_word;
	t->len = bytes;
	trans_act = spi_trans->trans_act & SPI_TRANS_ACT_MASK;
	if (trans_act == SPI_TRANS_WRITE)
		t->tx_buf = buf;
	else if (trans_act == SPI_TRANS_READ)
		t->rx_buf = buf;
	t->esop = esop;
	t->speed_hz = spnd->freq;
	spi_message_add_tail(t, m);

	if (ret < 0) {
		PRT_ERR("frame_fmt=%d,bits_per_word=%d\n"
			"addr=0x%x,buf=0x%p,bytes=%d\n"
			"cmd=0x%02x,addr_bits=%d,dummy_bits=%d\n",
			frame_fmt, bits_per_word,
			addr, buf, bytes, spi_trans->cmd,
			spi_trans->addr_bits, spi_trans->dummy_bits);
	}
	PRT_TRACE_END("\n");
	return ret;
}

static int __spi_xfer_poll_cmd(struct lombo_spnd *spnd,
				struct spi_bus_trans *spi_trans,
				 u8 *buf)
{
	struct spi_device *spi = spnd->spi;
	struct lombo_spi_operate_para esop;
	struct spi_transfer t;
	struct spi_message m;
	int ret = 0;

	PRT_TRACE_BEGIN("poll_cmd=0x%x,poll_addr=0x%x,poll_bytes=%d,buf=0x%p\n",
			spi_trans->poll_cmd, spi_trans->poll_addr,
			spi_trans->poll_bytes, buf);

	memset(&t, 0, sizeof(t));
	memset(&esop, 0, sizeof(esop));
	spi_message_init(&m);

	esop.inst = spi_trans->poll_cmd;
	esop.ff = FRAME_FORMAT_STANDARD_SPI;
	esop.addr_bit_len = 8;
	esop.addr[0] = spi_trans->poll_addr;

	t.bits_per_word = BITS_PER_WORD_8;
	t.len = spi_trans->poll_bytes;
	t.rx_buf = buf;
	t.esop = &esop;
	t.speed_hz = spnd->freq;
	spi_message_add_tail(&t, &m);

	ret = spi_sync(spi, &m);
	if (ret < 0) {
		PRT_ERR("poll_cmd=0x%02x,poll_addr=%d,poll_bytes=%d,buf=0x%p\n",
			spi_trans->poll_cmd, spi_trans->poll_addr,
			spi_trans->poll_bytes, buf);
	}
	PRT_TRACE_END("\n");
	return ret;
}

static int __spi_xfer_read_write(struct lombo_spnd *spnd, u32 frame_fmt,
				u32 sync, struct spi_bus_trans *spi_trans)
{
	u32 bits_per_word = 0;
	int ret = 0;

	PRT_TRACE_BEGIN("frame_fmt=%d,sync=%d\n", frame_fmt, sync);

	memset(&spnd->t, 0, sizeof(spnd->t));
	memset(&spnd->esop, 0, sizeof(spnd->esop));
	spi_message_init(&spnd->m);

	spnd->cur_trans_idx = 0;

	/*
	 * Using BITS_PER_WORD_32 may get better performance in cpu mode,
	 * but split the transfer may cause some unexpected error, so
	 * use BITS_PER_WORD_8 when the data len is not multiple of 4 to
	 * ensure the data is sent in one transfer
	 */
	if (spi_trans->bytes % 4 != 0)
		bits_per_word = BITS_PER_WORD_8;
	else
		bits_per_word = BITS_PER_WORD_32;

	ret = __spi_xfer_rw_buf(spnd, frame_fmt, bits_per_word,
				spi_trans, spi_trans->addr,
				spi_trans->buf, spi_trans->bytes);
	if (ret != 0) {
		PRT_ERR("\n");
		goto out;
	}

	spnd_trans_flush_dcache(spnd, SPND_STAGE_PRE);

	if (sync == 0) {
		INIT_COMPLETION(spnd->completion);

		spnd->m.complete = __trans_complete;
		spnd->m.context = spnd;

		ret = spi_async(spnd->spi, &spnd->m);
	} else {
		ret = spi_sync(spnd->spi, &spnd->m);
	}

	if (ret != 0)
		PRT_ERR("\n");

out:
	PRT_TRACE_END("\n");
	return ret;
}

static int __spi_xfer_control(struct lombo_spnd *spnd,
				struct spi_bus_trans *spi_trans)
{
	int ret = 0;

	memset(&spnd->t, 0, sizeof(spnd->t));
	memset(&spnd->esop, 0, sizeof(spnd->esop));
	spi_message_init(&spnd->m);

	spnd->cur_trans_idx = 0;

	ret = __spi_xfer_rw_buf(spnd, FRAME_FORMAT_STANDARD_SPI,
				BITS_PER_WORD_8, spi_trans,
				spi_trans->addr, NULL, 0);
	if (ret != 0)
		goto out;

	ret = spi_sync(spnd->spi, &spnd->m);

out:
	if (ret != 0) {
		PRT_ERR("ret=%d,frame_fmt=%d\n",
			ret, FRAME_FORMAT_STANDARD_SPI);
		goto out;
	}
	return ret;
}

static int __spi_xfer(unsigned long handle, struct spi_bus_trans *spi_trans,
		u32 sync, u32 *status)
{
	ktime_t start, now;
	struct lombo_spnd *spnd = (struct lombo_spnd *)handle;
	u32 frame_fmt = 0;
	u32 tmp_status = 0;
	u32 to_cnt = 0;
	u8 tranc_act = 0;
	int ret = 0;

	PRT_TRACE_BEGIN("trans_act=%d,transtype=%d,cmd=0x%02x,addr=%d\n"
			"addr_bits=%d,bytes=%d,dummy_bits=%d,sync=%d\n",
			spi_trans->trans_act, spi_trans->transtype,
			spi_trans->cmd, spi_trans->addr,
			spi_trans->addr_bits, spi_trans->bytes,
			spi_trans->dummy_bits, sync);

	spnd->cur_trans = spi_trans;

	if (spi_trans->cmd == 0) {
		PRT_WARN("\n");
		goto out;
	}

	/* async poll, the cmd is issue, and should poll the status now */
	if (spi_trans->trans_act & SPI_TRANS_TO_POLL) {
		if (!(spi_trans->trans_act & SPI_TRANS_ASYNC_POLL)) {
			ret = -EINVAL;
			goto out;
		}
		goto poll;
	}

	switch (spi_trans->transtype) {
	case SPI_BUS_TRANS_1_1_1:
		frame_fmt = FRAME_FORMAT_STANDARD_SPI;
		break;
	case SPI_BUS_TRANS_1_1_2:
		frame_fmt = FRAME_FORMAT_DUAL_SPI;
		break;
	case SPI_BUS_TRANS_1_1_4:
		frame_fmt = FRAME_FORMAT_QUAD_SPI;
		break;
	default:
		PRT_ERR("invalid transtype (%d)\n", spi_trans->transtype);
		ret = -EINVAL;
		goto out;
	}

	tranc_act = spi_trans->trans_act & SPI_TRANS_ACT_MASK;
	switch (tranc_act) {
	case SPI_TRANS_READ:
	case SPI_TRANS_WRITE:
		ret = __spi_xfer_read_write(spnd, frame_fmt,
						sync, spi_trans);
		if (ret != 0) {
			PRT_ERR("frame_fmt=%d\n", frame_fmt);
			/* TODO, reset the spi bus and retry */
			if (status != NULL)
				*status = TRANS_BUS_ERR;
			goto out;
		}
		break;
	case SPI_TRANS_NO_DATA:
	case SPI_TRANS_CTL_READ:
	case SPI_TRANS_CTL_PROG:
	case SPI_TRANS_CTL_ERASE:
	case SPI_TRANS_CTL_DIESEL:
		ret = __spi_xfer_control(spnd, spi_trans);
		if (ret != 0) {
			PRT_ERR("frame_fmt=%d\n", frame_fmt);
			/* TODO, reset the spi bus and retry */
			if (status != NULL)
				*status = TRANS_BUS_ERR;
			goto out;
		}
		break;
	default:
		PRT_ERR("invalid trans action (%d)\n", spi_trans->trans_act);
		goto out;
	}

	if (spi_trans->poll_cmd == 0)
		goto out;

	/* async poll, not poll here */
	if (spi_trans->trans_act & SPI_TRANS_ASYNC_POLL)
		goto out;

poll:
	if (sync == 0) {
		PRT_ERR("\n");
		ret = -EINVAL;
		goto out;
	}

	PRT_TRACE("poll_cmd=0x%x,poll_addr=0x%x,poll_bytes=%d,poll_val=0x%x\n",
			spi_trans->poll_cmd, spi_trans->poll_addr,
			spi_trans->poll_bytes, spi_trans->poll_val);
	PRT_TRACE("poll_mask=%x,poll_timeout=%d,poll_delay=%d\n",
			spi_trans->poll_mask, spi_trans->poll_timeout,
			spi_trans->poll_delay);

	start = now = ktime_get();

	do {
		ret = __spi_xfer_poll_cmd(spnd, spi_trans,
					(u8 *)&tmp_status);
		if (ret != 0) {
			PRT_ERR("frame_fmt=%d\n", frame_fmt);
			/* TODO, reset the spi bus and retry */
			if (status != NULL)
				*status = TRANS_BUS_ERR;
			goto out;
		}

		if ((tmp_status & spi_trans->poll_mask) ==
			(spi_trans->poll_val))
			break;

		if (spi_trans->poll_delay != 0)
			usleep_range(spi_trans->poll_delay,
					2 * spi_trans->poll_delay);
		now = ktime_get();
		if (ktime_us_delta(now, start) >= spi_trans->poll_timeout)
			to_cnt++;
	} while (to_cnt <= 1);

	if ((tmp_status & spi_trans->poll_mask) != (spi_trans->poll_val)) {
		PRT_ERR("poll_timeout=%d,tmp_status=%x\n",
			spi_trans->poll_timeout, tmp_status);

		/* return the timeout flag and the last status */
		if (status != NULL)
			*status = tmp_status | TRANS_WAIT_TIMEOUT;
		ret = -EIO;
		goto out;
	}

	if (status != NULL)
		*status = tmp_status;

out:
	PRT_TRACE_END("tmp_status=%x\n", tmp_status);
	return ret;
}

static int spi_xfer(unsigned long handle, struct spi_bus_trans *spi_trans,
		u32 *status)
{
	return __spi_xfer(handle, spi_trans, 1, status);
}

static int spi_xfer_async(unsigned long handle,
			struct spi_bus_trans *spi_trans)
{
#if USE_SPI_TRANS_ASYNC
	return __spi_xfer(handle, spi_trans, 0, NULL);
#else
	return __spi_xfer(handle, spi_trans, 1, NULL);
#endif
}

static int spi_bus_sync(unsigned long handle)
{
#if USE_SPI_TRANS_ASYNC
	struct lombo_spnd *spnd = (struct lombo_spnd *)handle;

	PRT_TRACE_BEGIN("\n");
	__wait_for_trans_complete(spnd);
	spnd_trans_flush_dcache(spnd, SPND_STAGE_POST);
	PRT_TRACE_END("\n");
#endif
	return 0;
}

static struct spi_bus_ops spi_ops = {
	0,
	1,
	spi_xfer,
	spi_xfer_async,
	spi_bus_sync,
};

/*
 * os hooks
 */
static void *os_malloc(unsigned long size)
{
	return kmalloc(size, GFP_KERNEL);
}

static void os_free(void *ptr)
{
	kfree(ptr);
}

static void *os_malloc_buf(unsigned long size)
{
	return (void *)__get_free_pages(GFP_KERNEL, get_order(size));
}

static void os_free_buf(void *ptr, unsigned long size)
{
	free_pages((unsigned long)ptr, get_order(size));
}

static void *os_memset(void *s, int c, unsigned long count)
{
	return memset(s, c, count);
}

static void *os_memcpy(void *dst, void *src, unsigned long count)
{
	return memcpy(dst, src, count);
}

static void printf_debug(const char *fmt, ...)
{
	char tmp_buf[256];
	va_list	ap;

	va_start(ap, fmt);
	vsnprintf(tmp_buf, 255, fmt, ap);
	va_end(ap);

	pr_info("%s", tmp_buf);
}

static void printf_error(const char *fmt, ...)
{
	char tmp_buf[256];
	va_list	ap;

	va_start(ap, fmt);
	vsnprintf(tmp_buf, 255, fmt, ap);
	va_end(ap);

	pr_err("%s", tmp_buf);
}

static unsigned long os_lock_open(void)
{
	void *mlock;

	mlock = kzalloc(sizeof(struct mutex), GFP_KERNEL);
	if (mlock != NULL)
		mutex_init(mlock);

	return (unsigned long)mlock;
}

static void os_lock_close(unsigned long ld)
{
	kfree((void *)ld);
}

static void os_lock(unsigned long ld)
{
	struct mutex *mlock = (struct mutex *)ld;

	mutex_lock(mlock);
}

static void os_unlock(unsigned long ld)
{
	struct mutex *mlock = (struct mutex *)ld;

	mutex_unlock(mlock);
}

static int os_strlen(const char *str)
{
	return strlen(str);
}

static int os_strncmp(const char *cs, const char *ct, int len)
{
	return strncmp(cs, ct, len);
}

static void os_bug(void)
{
	BUG();
}

static void os_assert(int cond)
{
	BUG_ON(!cond);
}

static int os_vsnprintf(char *buf, size_t size, const char *fmt, void *args)
{
	unsigned long rem_nsec;
	u64 ts = 0;
	int len = 0;

	ts = local_clock();
	rem_nsec = do_div(ts, 1000000000);

	len = sprintf(buf, "[%5lu.%06lu] ", (unsigned long)ts, rem_nsec / 1000);
	if (len > 32)
		len = 0;

	return vsnprintf(buf + len, size - len, fmt, *((va_list *)args));
}

static void os_delay(unsigned long us)
{
	usleep_range(us, us + 20);
}

static struct nandphy_hook os_hook = {
	os_malloc,	/* .os_malloc */
	os_free,	/* .os_free */
	os_malloc_buf,	/* .os_malloc_buf */
	os_free_buf,	/* .os_free_buf */
	printf_debug,	/* .printf_debug */
	printf_error,	/* .printf_error */
	os_memset,	/* .os_memset */
	os_memcpy,	/* .os_memcpy */
	os_strlen,	/* .strlen */
	os_strncmp,	/* .strncmp */
	os_lock_open,	/* .lock_open */
	os_lock_close,	/* .lock_close */
	os_lock,	/* .lock */
	os_unlock,	/* .unlock */
	os_bug,		/* .bug */
	os_assert,	/* .assert */
	os_vsnprintf,	/* .os_vsnprintf */
	os_delay,	/* .os_delay */
};

#ifndef SPND_TEST_WITH_BURN_LIB
static int __spnd_get_param0(void *param, u32 param_len)
{
	void *param_src = NULL;

	if (lombo_npaddr == 0)
		return -EINVAL;

	param_src = ioremap(lombo_npaddr, param_len);
	memcpy(param, param_src, param_len);

	return 0;
}

static int spnd_init(struct lombo_spnd *spnd)
{
	u32 param_len = 0;
	u32 param1_len = 0;
	int ret = 0;

	nandphy_preinit(&os_hook, &spi_ops);

	param_len = nandphy_get_param_len();

	spnd->param = os_malloc_buf(param_len);
	if (NULL == spnd->param) {
		PRT_ERR("\n");
		ret = -ENOMEM;
		goto out;
	}

	/* TODO get param0 */
	ret = __spnd_get_param0(spnd->param, param_len);
	if (ret != 0) {
		PRT_ERR("\n");
		goto out;
	}

	spnd->freq = SPND_MIN_FREQ;

	ret = nandphy_init(&os_hook, &spi_ops, spnd->param);
	if (ret) {
		PRT_ERR("\n");
		ret = -ENODEV;
		goto out;
	}

	spnd->freq = nandphy_get_freq();

	if ((spnd->max_freq > 0) && (spnd->freq > spnd->max_freq)) {
		PRT_DBG("spi max_freq %d, flash max_freq %d\n",
			spnd->max_freq, spnd->freq);
		spnd->freq = spnd->max_freq;
	}

	if (spnd->freq < SPND_MIN_FREQ) {
		PRT_DBG("flash max_freq %d too small, set to %d\n",
			spnd->freq, SPND_MIN_FREQ);
		spnd->freq = SPND_MIN_FREQ;
	}

	PRT_INFO("spi bus width %d, frequency %d\n", spnd->spi_bw, spnd->freq);

#ifdef NANDPHY_DEBUG
	phy_dump_nandparam();
	phy_dump_snc_param();
#endif

#if 1
	param1_len = nandphy_get_param1_len();
	spnd->param1 = os_malloc_buf(param1_len);
	if (NULL == spnd->param1) {
		PRT_ERR("\n");
		ret = -ENOMEM;
		goto out;
	}

	/* for booting */
	ret = nandphy_init1(spnd->param1);
	if (ret) {
		PRT_ERR("\n");
		ret = -EIO;
		goto out;
	}
#else
	/* TODO get param1 */
	__spnd_get_param1();

	ret = nandphy_init2(spnd->param1);
	if (ret) {
		ret = -EIO;
		goto out;
	}
#endif

	spnd->page_size = nandphy_get_page_size();

#ifdef NANDPHY_DEBUG
	phy_block_dump_mapping();
#endif

out:
	if (ret != 0) {
		if (NULL != spnd->param) {
			os_free_buf(spnd->param, nandphy_get_param_len());
			spnd->param = NULL;
		}

		if (NULL != spnd->param1) {
			os_free_buf(spnd->param1, nandphy_get_param1_len());
			spnd->param1 = NULL;
		}
	}
	return ret;
}
#endif

static void spnd_exit(struct lombo_spnd *spnd)
{
	nandphy_exit();

	if (NULL != spnd->param) {
		os_free_buf(spnd->param, nandphy_get_param_len());
		spnd->param = NULL;
	}

	if (NULL != spnd->param1) {
		os_free_buf(spnd->param1, nandphy_get_param1_len());
		spnd->param = NULL;
	}
}

#ifdef CONFIG_OF
/* lombo_spnd_parse_dt
 *
 * Parse the device tree node and retreive the platform data.
*/
static int
lombo_spnd_parse_dt(struct device_node *np, struct lombo_spnd *spnd)
{
	int ret = 0;

	if (!np)
		return -ENODEV;

	if (of_property_read_u32(np, "spi-max-frequency",
		&spnd->max_freq) < 0)
		spnd->max_freq = 0;

	if (of_property_read_u32(np, "spi-bus-width", &spnd->spi_bw) < 0)
		spnd->spi_bw = 4;

	if ((spnd->spi_bw != 1) && (spnd->spi_bw != 2) &&
		(spnd->spi_bw != 4)) {
		PRT_WARN("invalid bus width (%d)\n", spnd->spi_bw);
		spnd->spi_bw = 4;
	}

	return ret;
}
#endif

/* lombo_spnd_probe
 *
 * called by the bus driver when a suitable device is found
*/
static int lombo_spnd_probe(struct spi_device *spi)
{
	struct lombo_spnd *spnd = NULL;
	int ret = 0;

	PRT_INFO("==========entered!\n");

	if (!spi->dev.of_node) {
		dev_err(&spi->dev, "no of_node of the device\n");
		return -EINVAL;
	}

	spnd = devm_kzalloc(&spi->dev, sizeof(*spnd), GFP_KERNEL);
	if (!spnd) {
		dev_err(&spi->dev, "no memory for state\n");
		return -ENOMEM;
	}

	spnd->spi = spi;

	ret = lombo_spnd_parse_dt(spi->dev.of_node, spnd);
	if (ret != 0) {
		dev_err(&spi->dev, "lombo_spnd_parse_dt failed\n");
		return ret;
	}

	init_completion(&spnd->completion);

	spi_ops.handle = (unsigned long)spnd;
	spi_ops.bus_width = spnd->spi_bw;

	g_spnd = spnd;

#ifdef SPND_CHIP_TEST
	if (chip_test) {
		ret = spnd_chip_test();
		if (ret != 0) {
			dev_err(&spi->dev, "spnd_chip_test failed\n");
			return ret;
		}
		return 0;
	}

	if (force_erase) {
		ret = spnd_chip_force_erase();
		if (ret != 0) {
			dev_err(&spi->dev, "spnd_chip_force_erase failed\n");
			return ret;
		}
		return 0;
	}
#endif

#ifdef SPND_TEST_WITH_BURN_LIB
	if (spnd_fmt) {
		ret = spnd_format(1);
		if (ret != 0) {
			dev_err(&spi->dev, "spnd_format failed\n");
			return ret;
		}
		return 0;
	}
#endif

#ifdef SPND_TEST_WITH_BURN_LIB
	ret = spnd_init_with_lib(spnd);
#else
	ret = spnd_init(spnd);
#endif
	if (ret != 0) {
		dev_err(&spi->dev, "spnd_init failed\n");
		nand_debug_init(0);
		return 0;
	}

	dev_set_drvdata(&spi->dev, spnd);

#if 1
	ret = _register_nftl();
	nand_debug_init(1);
	if (ret) {
		dev_err(&spi->dev, "_register_nftl failed\n");
		return 0;
	}

	ret = nandblk_init();
	if (ret != 0)
		dev_err(&spi->dev, "nandblk_init failed\n");
#endif
	PRT_INFO("==========probed!\n");

	return 0;
}

/* lombo_spnd_remove
 *
 * called when device is removed from the bus
*/

static int lombo_spnd_remove(struct spi_device *spi)
{
	struct lombo_spnd *spnd = dev_get_drvdata(&spi->dev);

	PRT_INFO("==========remove!\n");

	nand_debug_exit();
	if (spnd != NULL) {
#if 1
		nandblk_exit();
		_deregister_nftl();
#endif
		spnd_exit(spnd);
	} else
		spnd_exit(g_spnd);

	return 0;
}

#ifdef CONFIG_OF
static const struct of_device_id lombo_spnd_match[] = {
	{ .compatible = "lombo,spnd", },
	{},
};
MODULE_DEVICE_TABLE(of, lombo_spnd_match);
#endif

static struct spi_driver lombo_spnd_driver = {
	.driver		= {
		.name	= "lombo-spnd",
		.owner	= THIS_MODULE,
		.of_match_table = of_match_ptr(lombo_spnd_match),
	},
	.probe		= lombo_spnd_probe,
	.remove		= lombo_spnd_remove,
};

static int __init lombo_spnd_driver_init(void)
{
	return spi_register_driver(&lombo_spnd_driver);
}
late_initcall(lombo_spnd_driver_init);


static void __exit lombo_spnd_driver_exit(void)
{
	return spi_unregister_driver(&lombo_spnd_driver);
}
module_exit(lombo_spnd_driver_exit);

MODULE_AUTHOR("lomboswer <lomboswer@lombotech.com>");
MODULE_DESCRIPTION("LOMBO SPI Nand driver");
MODULE_LICENSE("GPL");
