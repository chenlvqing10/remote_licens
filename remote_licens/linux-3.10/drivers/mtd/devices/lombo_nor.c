/*
 * lombo_nor.c - MTD SPI Driver for Nor
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
#include <linux/err.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/mutex.h>
#include <linux/math64.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/mod_devicetable.h>

#include <linux/mtd/cfi.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/of_platform.h>

#include <linux/spi/spi.h>
#include <linux/spi/flash.h>

#include <mach/debug.h>

#undef MOD_NAME
#define MOD_NAME "NOR"

/* Flash opcodes. */
#define	OPCODE_WREN		0x06 /* Write enable */
#define	OPCODE_RDSR		0x05 /* Read status register */
#define	OPCODE_WRSR		0x01 /* Write status register 1 byte */
#define	OPCODE_NORM_READ	0x03 /* Read data bytes (low frequency) */
#define	OPCODE_FAST_READ	0x0b /* Read data bytes (high frequency) */
#define	OPCODE_PP		0x02 /* Page program (up to 256 bytes) */
#define	OPCODE_BE_4K		0x20 /* Erase 4KiB block */
#define	OPCODE_BE_32K		0x52 /* Erase 32KiB block */
#define	OPCODE_SE		0xd8 /* Sector erase (usually 64KiB) */
#define	OPCODE_CHIP_ERASE	0xc7 /* Erase whole flash chip */
#define	OPCODE_RDID		0x9f /* Read JEDEC ID */

/* Used for SST flashes only. */
#define	OPCODE_BP		0x02 /* Byte program */
#define	OPCODE_WRDI		0x04 /* Write disable */
#define	OPCODE_AAI_WP		0xad /* Auto address increment word program */

/* Used for Macronix flashes only. */
#define	OPCODE_EN4B		0xb7 /* Enter 4-byte mode */
#define	OPCODE_EX4B		0xe9 /* Exit 4-byte mode */

/* Used for Spansion flashes only. */
#define	OPCODE_BRWR		0x17 /* Bank register write */

/* Status Register bits. */
#define WIP_BIT			(0)
#define WEL_BIT			(1)

/* meaning of other SR_* bits may differ between vendors */
#define	SR_BP0			4    /* Block protect 0 */
#define	SR_BP1			8    /* Block protect 1 */
#define	SR_BP2			0x10 /* Block protect 2 */
#define	SR_SRWD			0x80 /* SR write protect */

/* Define max times to check status register before we give up. */
#define	MAX_READY_WAIT_JIFFIES	(120 * HZ) /* 120s max chip erase */
#define	MAX_CMD_SIZE		5

#define JEDEC_MFR(_jedec_id)	((_jedec_id) >> 16)

#define BUF_READ_SIZE		(128 * 1024)

struct flash_info {
	/* JEDEC id zero means "no ID" (most older chips); otherwise it has
	 * a high byte of zero plus three data bytes: the manufacturer id,
	 * then a two byte device id.
	 */
	u32		jedec_id;
	u16             ext_id;

	/* The size listed here is what works with OPCODE_SE, which isn't
	 * necessarily called a "sector" by the vendor.
	 */
	unsigned	sector_size;
	u16		n_sectors;

	u16		page_size;
	u16		addr_width;

	u16		flags;
#define	SECT_4K		0x01		/* OPCODE_BE_4K works uniformly */
};

#define INFO(_jedec_id, _ext_id, _sector_size, _n_sectors, _flags)	\
	((kernel_ulong_t)&(struct flash_info) {				\
		.jedec_id = (_jedec_id),				\
		.ext_id = (_ext_id),					\
		.sector_size = (_sector_size),				\
		.n_sectors = (_n_sectors),				\
		.page_size = 256,					\
		.flags = (_flags),					\
	})

/* NOTE: double check command sets and memory organization when you add
 * more flash chips.  This current list focusses on newer chips, which
 * have been converging on command sets which including JEDEC ID.
 */
static const struct spi_device_id lombo_nor_ids[] = {
	/* GigaDevice */
	{ "gd25q32",     INFO(0xc84016, 0, 64 * 1024,  64, SECT_4K) },
	{ "gd25q64",     INFO(0xc84017, 0, 64 * 1024, 128, SECT_4K) },
	{ "gd25q128",    INFO(0xc84018, 0, 64 * 1024, 256,       0) },
	/* Macronix */
	{ "mx25l6405d",  INFO(0xc22017, 0, 64 * 1024, 128,       0) },
	/* Micron */
	{ "n25q128a11",  INFO(0x20bb18, 0, 64 * 1024, 256,       0) },
	{ "n25q128a13",  INFO(0x20ba18, 0, 64 * 1024, 256,       0) },
	/* Winbond -- w25x "blocks" are 64K, "sectors" are 4KiB */
	{ "w25q32",      INFO(0xef4016, 0, 64 * 1024,  64, SECT_4K) },
	{ "w25q64",      INFO(0xef4017, 0, 64 * 1024, 128, SECT_4K) },
	{ "w25q128",     INFO(0xef4018, 0, 64 * 1024, 256,       0) },
	{ },
};

/**
 * struct lombo_nor - Runtime info holder for SPI nor.
 * @spi: Pointer to the SPI device.
 * @lock: mutex lock for sync.
 * @mtd: mtd info for MTD sub-system.
 * @page_size: max write bytes for one program.
 * @addr_width: 3 bytes or 4 bytes address.
 * @erase_opcode: erase code, 4K erase or 64K erase.
 */
struct lombo_nor {
	struct spi_device	*spi;
	struct mutex		lock;
	struct mtd_info		mtd;
	u16			page_size;
	u16			addr_width;
	u8			erase_opcode;
};

static const char * const probes[] = { "ofpart", NULL };

/**
 * __read_status_cmd - read nor status without address
 * @spi: pointer to spi device.
 * @cmd: control command to send
 * @buf: buffer for read
 * @n_byte: number of bytes to read
 *
 * Return 0 if success, !0 error.
 */
static int __read_status_cmd(struct spi_device *spi,
				u8 cmd, u8 *buf, u32 n_byte)
{
	int err = 0;
	struct spi_transfer t;
	struct spi_message m;
	struct lombo_spi_operate_para esop;

	memset(&t, 0, sizeof(t));
	memset(&esop, 0, sizeof(esop));
	spi_message_init(&m);

	esop.inst = cmd;
	esop.ff = FRAME_FORMAT_STANDARD_SPI;

	t.bits_per_word = 8;
	t.len = n_byte;
	t.rx_buf = buf;
	t.esop = &esop;
	spi_message_add_tail(&t, &m);

	err = spi_sync(spi, &m);
	if (err < 0) {
		PRT_ERR("read status cmd:0x%x failed\n", cmd);
		return err;
	}

	return err;
}

/**
 * __send_ctrl_cmd - send control command
 * @spi: pointer to spi device.
 * @cmd: control command to send
 *
 * Return 0 if success, !0 error.
 */
static int __send_ctrl_cmd(struct spi_device *spi, u8 cmd)
{
	int err = 0;
	struct spi_transfer t;
	struct spi_message m;
	struct lombo_spi_operate_para esop;

	memset(&t, 0, sizeof(t));
	memset(&esop, 0, sizeof(esop));
	spi_message_init(&m);

	esop.inst = cmd;
	esop.ff = FRAME_FORMAT_STANDARD_SPI;

	t.bits_per_word = 8;
	t.esop = &esop;
	spi_message_add_tail(&t, &m);

	err = spi_sync(spi, &m);
	if (err < 0) {
		PRT_ERR("send control cmd:0x%x failed\n", cmd);
		return err;
	}

	return err;
}

/**
 * __send_erase_cmd - send erase command
 * @spi: pointer to spi device.
 * @cmd: erase command to send
 * @addr: nor address for erase
 *
 * Return 0 if success, !0 error.
 */
static int __send_erase_cmd(struct spi_device *spi, u8 cmd, u32 addr)
{
	int err = 0;
	struct spi_transfer t;
	struct spi_message m;
	struct lombo_spi_operate_para esop;

	memset(&t, 0, sizeof(t));
	memset(&esop, 0, sizeof(esop));
	spi_message_init(&m);

	esop.inst = cmd;
	esop.ff = FRAME_FORMAT_STANDARD_SPI;
	esop.addr_bit_len = 24;
	esop.addr[0] = addr;

	t.bits_per_word = 8;
	t.esop = &esop;
	spi_message_add_tail(&t, &m);

	err = spi_sync(spi, &m);
	if (err < 0) {
		PRT_ERR("send erase cmd:0x%x failed\n", cmd);
		return err;
	}

	return err;
}

/**
 * __read_buf_32 - read chip data into buffer in 32 bits read.
 * @spi: pointer to spi device.
 * @addr: nor address for data read
 * @buf: buffer to store data
 * @len: number of bytes to read
 *
 * Return 0 if success, !0 error.
 */
static int __read_buf_32(struct spi_device *spi, u32 addr, size_t buf, u32 len)
{
	int err = 0;
	struct spi_transfer t;
	struct spi_message m;
	struct lombo_spi_operate_para esop;

	memset(&t, 0, sizeof(t));
	memset(&esop, 0, sizeof(esop));
	spi_message_init(&m);

	esop.inst = OPCODE_NORM_READ;
	esop.ff = FRAME_FORMAT_STANDARD_SPI;
	esop.addr_bit_len = 24;
	esop.addr[0] = addr;

	t.bits_per_word = 32;
	t.len = len;
	t.rx_buf = (u32 *)buf;
	t.esop = &esop;
	spi_message_add_tail(&t, &m);

	err = spi_sync(spi, &m);
	if (err < 0) {
		PRT_ERR("read from:0x%x to:0x%p, len:%d failed\n",
				addr, (size_t *)buf, len);
		return err;
	}

	return err;
}

/**
 * __read_buf_8 - read chip data into buffer in 8 bits read.
 * @spi: pointer to spi device.
 * @addr: nor address for data read
 * @buf: buffer to store data
 * @len: number of bytes to read
 *
 * Return 0 if success, !0 error.
 */
static int __read_buf_8(struct spi_device *spi, u32 addr, size_t buf, u32 len)
{
	int err = 0;
	struct spi_transfer t;
	struct spi_message m;
	struct lombo_spi_operate_para esop;
#ifdef CONFIG_ARCH_LOMBO_N7V0
	u8 tmp[4] = {0};
	u8 tmp2copy[4] = {0};
	int tmp_len = 4;
#endif

	memset(&t, 0, sizeof(t));
	memset(&esop, 0, sizeof(esop));
	spi_message_init(&m);

	esop.inst = OPCODE_NORM_READ;
	esop.ff = FRAME_FORMAT_STANDARD_SPI;
	esop.addr_bit_len = 24;
	esop.addr[0] = addr;

	t.bits_per_word = 8;
#ifdef CONFIG_ARCH_LOMBO_N7V0
	t.len = tmp_len;
	t.rx_buf = tmp;
#else
	t.len = len;
	t.rx_buf = (u8 *)buf;
#endif
	t.esop = &esop;
	spi_message_add_tail(&t, &m);

	err = spi_sync(spi, &m);
	if (err < 0) {
		PRT_ERR("read from:0x%x to:0x%p, len:%d failed\n",
				addr, (size_t *)buf, len);
		return err;
	}

#ifdef CONFIG_ARCH_LOMBO_N7V0
	tmp2copy[0] = tmp[3];
	tmp2copy[1] = tmp[2];
	tmp2copy[2] = tmp[1];
	tmp2copy[3] = tmp[0];

	memcpy((u8 *)buf, tmp2copy, len);
#endif

	return err;
}

/**
 * __read_buf - read chip data into buffer
 * @spi: pointer to spi device.
 * @addr: nor address for data read
 * @buf: buffer to store data
 * @len: number of bytes to read
 *
 * Return 0 if success, !0 error.
 */
static int __read_buf(struct spi_device *spi, u32 addr, size_t buf, u32 len)
{
	int err = 0;
	u32 nor_addr = addr;
	size_t dst_buf = buf;
	int div_len = len / 4;
	int mod_len = len & (4 - 1);

	if (addr > 0 && (addr % 4) != 0)
		PRT_ERR("addr=0x%x, len=%d\n", addr, len);

	if (div_len > 0) {
		/* read nor to buffer */
		err = __read_buf_32(spi, nor_addr, dst_buf, 4 * div_len);
		if (err) {
			PRT_ERR("__read_buf_32 err, ret=%d\n", err);
			err = __LINE__;
			goto out;
		}

		nor_addr += 4 * div_len;
		dst_buf  += 4 * div_len;
	}

	if (mod_len != 0) {
		/* read nor to buffer */
		err = __read_buf_8(spi, nor_addr, dst_buf, mod_len);
		if (err) {
			PRT_ERR("__read_buf_8 err, ret=%d\n", err);
			err = __LINE__;
			goto out;
		}
	}

out:
	return err;
}

/**
 * lombo_nor_read_buf - read chip data into buffer
 * @spi: pointer to spi device.
 * @addr: nor address for data read
 * @buf: buffer to store data
 * @len: number of bytes to read
 *
 * Return 0 if success, !0 error.
 */
static int lombo_nor_read_buf(struct spi_device *spi,
				u32 addr, u8 *buf, u32 len)
{
	int err = 0;
	u32 nor_addr = addr;
	size_t dst_buf = (size_t)buf;
	int div_len = len / (BUF_READ_SIZE);
	int mod_len = len & (BUF_READ_SIZE - 1);
	int i;

	for (i = 0; i < div_len; i++) {
		/* read nor to buffer */
		err = __read_buf(spi, nor_addr, dst_buf, BUF_READ_SIZE);
		if (err) {
			PRT_ERR("__read_buf err, ret=%d\n", err);
			err = __LINE__;
			goto out;
		}

		nor_addr += BUF_READ_SIZE;
		dst_buf  += BUF_READ_SIZE;
	}

	if (mod_len != 0) {
		/* read nor to buffer */
		err = __read_buf(spi, nor_addr, dst_buf, mod_len);
		if (err) {
			PRT_ERR("__read_buf err, ret=%d\n", err);
			err = __LINE__;
			goto out;
		}
	}

out:
	return err;
}

/**
 * __write_buf_32 - write buffer to chip in 32 bits write.
 * @spi: pointer to spi device.
 * @addr: nor address for data write
 * @buf: data buffer
 * @len: number of bytes to write
 *
 * Return 0 if success, !0 error.
 */
static int __write_buf_32(struct spi_device *spi,
				u32 addr, size_t buf, u32 len)
{
	int err = 0;
	struct spi_transfer t;
	struct spi_message m;
	struct lombo_spi_operate_para esop;

	memset(&t, 0, sizeof(t));
	memset(&esop, 0, sizeof(esop));
	spi_message_init(&m);

	esop.inst = OPCODE_PP;
	esop.ff = FRAME_FORMAT_STANDARD_SPI;
	esop.addr_bit_len = 24;
	esop.addr[0] = addr;

	t.bits_per_word = 32;
	t.len = len;
	t.tx_buf = (u32 *)buf;
	t.esop = &esop;
	spi_message_add_tail(&t, &m);

	err = spi_sync(spi, &m);
	if (err < 0) {
		PRT_ERR("write from:0x%p to:0x%x, len:%d failed\n",
				(size_t *)buf, addr, len);
		return err;
	}

	return err;
}

/**
 * __write_buf_8 - write buffer to chip in 8 bits write.
 * @spi: pointer to spi device.
 * @addr: nor address for data write
 * @buf: data buffer
 * @len: number of bytes to write
 *
 * Return 0 if success, !0 error.
 */
static int __write_buf_8(struct spi_device *spi, u32 addr, size_t buf, u32 len)
{
	int err = 0;
	struct spi_transfer t;
	struct spi_message m;
	struct lombo_spi_operate_para esop;
#ifdef CONFIG_ARCH_LOMBO_N7V0
	u8 *tmp = (u8 *)buf;
	u8 tmp2write[4] = {0};
	int tmp_len = 4;

	if (len == 1) {
		tmp2write[0] = tmp[0];
	} else if (len == 2) {
		tmp2write[0] = tmp[1];
		tmp2write[1] = tmp[0];
	} else if (len == 3) {
		tmp2write[0] = tmp[2];
		tmp2write[1] = tmp[1];
		tmp2write[2] = tmp[0];
	}
#endif

	memset(&t, 0, sizeof(t));
	memset(&esop, 0, sizeof(esop));
	spi_message_init(&m);

	esop.inst = OPCODE_PP;
	esop.ff = FRAME_FORMAT_STANDARD_SPI;
	esop.addr_bit_len = 24;
#ifdef CONFIG_ARCH_LOMBO_N7V0
	esop.addr[0] = addr + (tmp_len - len);
#else
	esop.addr[0] = addr;
#endif

	t.bits_per_word = 8;
	t.len = len;
#ifdef CONFIG_ARCH_LOMBO_N7V0
	t.tx_buf = tmp2write;
#else
	t.tx_buf = (u8 *)buf;
#endif
	t.esop = &esop;
	spi_message_add_tail(&t, &m);

	err = spi_sync(spi, &m);
	if (err < 0) {
		PRT_ERR("write from:0x%p to:0x%x, len:%d failed\n",
				(size_t *)buf, addr, len);
		return err;
	}

	return err;
}

/**
 * lombo_read_sr - read the status register.
 * @flash: pointer to lombo nor.
 *
 * Return the status register value if success, <0 error.
 */
static int lombo_read_sr(struct lombo_nor *flash)
{
	int err = 0;
	u8 val = 0;

	err = __read_status_cmd(flash->spi, OPCODE_RDSR, &val, 1);
	if (err) {
		PRT_ERR("read status register error\n");
		err = -1;
		return err;
	}

	return val;
}

/**
 * lombo_wait_till_ready - read status register until ready, or timeout occurs.
 * @flash: pointer to lombo nor.
 * @status_bit: status bit for check
 * @ready_status: target status, 0 or 1
 *
 * Return 0 if success, !0 error.
 */
static int lombo_wait_till_ready(struct lombo_nor *flash,
				u8 status_bit, u8 ready_status)
{
	unsigned long deadline;
	int sr;

	deadline = jiffies + MAX_READY_WAIT_JIFFIES;

	do {
		sr = lombo_read_sr(flash);
		if (sr < 0)
			break;

		if (((sr >> status_bit) & 0x1) == ready_status)
			return 0;

		cond_resched();

	} while (!time_after_eq(jiffies, deadline));

	return 1;
}

/**
 * lombo_write_enable - set write enable latch with Write Enable command.
 * @flash: pointer to lombo nor.
 *
 * Return 0 if success, !0 error.
 */
static int lombo_write_enable(struct lombo_nor *flash)
{
	int err = 0;

	err = __send_ctrl_cmd(flash->spi, OPCODE_WREN);
	if (err) {
		PRT_ERR("write enable error\n");
		goto out;
	}

out:
	return err;
}

/**
 * lombo_nor_write_buf - write buffer to chip
 * @spi: pointer to spi device.
 * @addr: nor address for data write
 * @buf: data buffer
 * @len: number of bytes to write
 *
 * Return 0 if success, !0 error.
 */
static int lombo_nor_write_buf(struct spi_device *spi,
				u32 addr, size_t buf, u32 len)
{
	struct lombo_nor *flash = dev_get_drvdata(&spi->dev);
	int err = 0;
	u32 nor_addr = addr;
	size_t src_buf = buf;
	int div_len = len / 4;
	int mod_len = len & (4 - 1);

	if (addr > 0 && (addr % 4) != 0)
		PRT_ERR("addr=0x%x, len=%d\n", addr, len);

	if (div_len > 0) {
		/* write buffer to nor */
		err = __write_buf_32(spi, nor_addr, src_buf, 4 * div_len);
		if (err) {
			PRT_ERR("__write_buf_32 err, ret=%d\n", err);
			err = __LINE__;
			goto out;
		}

		nor_addr += 4 * div_len;
		src_buf  += 4 * div_len;
	}

	if (mod_len != 0) {
		if (div_len > 0) {
			if (lombo_wait_till_ready(flash, WIP_BIT, 0)) {
				PRT_ERR("\n");
				err = __LINE__;
				goto out;
			}

			if (lombo_write_enable(flash)) {
				PRT_ERR("\n");
				err = __LINE__;
				goto out;
			}

			if (lombo_wait_till_ready(flash, WEL_BIT, 1)) {
				PRT_ERR("\n");
				err = __LINE__;
				goto out;
			}
		}

		/* write buffer to nor */
		err = __write_buf_8(spi, nor_addr, src_buf, mod_len);
		if (err) {
			PRT_ERR("__write_buf_8 err, ret=%d\n", err);
			err = __LINE__;
			goto out;
		}
	}

out:
	return err;
}

#if 0
/**
 * __write_status_cmd - write nor status without address.
 * @spi: pointer to spi device.
 * @cmd: control command to send
 * @buf: buffer for write
 * @n_byte: number of bytes to write
 *
 * write nor register data without address.
 */
static int __write_status_cmd(struct spi_device *spi,
				u8 cmd, u8 *buf, u32 n_byte)
{
	int err = 0;
	struct spi_transfer t;
	struct spi_message m;
	struct lombo_spi_operate_para esop;

	memset(&t, 0, sizeof(t));
	memset(&esop, 0, sizeof(esop));
	spi_message_init(&m);

	esop.inst = cmd;
	esop.ff = FRAME_FORMAT_STANDARD_SPI;

	t.bits_per_word = 8;
	t.len = n_byte;
	t.tx_buf = buf;
	t.esop = &esop;
	spi_message_add_tail(&t, &m);

	err = spi_sync(spi, &m);
	if (err < 0) {
		PRT_ERR("write status cmd:0x%x failed\n", cmd);
		return err;
	}

	return err;
}

/**
 * lombo_write_sr - write status register 1 byte.
 * @flash: pointer to lombo nor.
 * @val: value to write.
 *
 * Return 0 if success, !0 error.
 */
static int lombo_write_sr(struct lombo_nor *flash, u8 val)
{
	int err = 0;

	err = __write_status_cmd(flash->spi, OPCODE_WRSR, &val, 1);
	if (err) {
		pr_err("write sr error\n");
		return err;
	}

	return 0;
}
#endif

/**
 * lombo_erase_chip - erase the whole flash memory.
 * @flash: pointer to lombo nor.
 *
 * Return 0 if success, !0 error.
 */
static int lombo_erase_chip(struct lombo_nor *flash)
{
	PRT_DBG("%lldKiB\n", (long long)(flash->mtd.size >> 10));

	/* Wait until finished previous write command. */
	if (lombo_wait_till_ready(flash, WIP_BIT, 0)) {
		PRT_ERR("wait WIP timeout\n");
		return 1;
	}

	/* Send write enable, then erase commands. */
	if (lombo_write_enable(flash)) {
		PRT_ERR("write enable failed before erase chip\n");
		return 1;
	}

	if (lombo_wait_till_ready(flash, WEL_BIT, 1)) {
		PRT_ERR("wait WEL timeout\n");
		return 1;
	}

	return __send_ctrl_cmd(flash->spi, OPCODE_CHIP_ERASE);
}

/**
 * lombo_erase_sector - erase one sector of flash memory at offset.
 * @flash: pointer to lombo nor.
 * @offset: offset in flash.
 *
 * Return 0 if success, !0 error.
 */
static int lombo_erase_sector(struct lombo_nor *flash, u32 offset)
{
	PRT_DBG("%dKiB at 0x%08x\n", flash->mtd.erasesize / 1024, offset);

	/* Wait until finished previous write command. */
	if (lombo_wait_till_ready(flash, WIP_BIT, 0)) {
		PRT_ERR("wait WIP timeout\n");
		return 1;
	}

	/* Send write enable, then erase commands. */
	if (lombo_write_enable(flash)) {
		PRT_ERR("write enable failed before erase sector\n");
		return 1;
	}

	if (lombo_wait_till_ready(flash, WEL_BIT, 1)) {
		PRT_ERR("wait WEL timeout\n");
		return 1;
	}

	return __send_erase_cmd(flash->spi, flash->erase_opcode, offset);
}

/* MTD implementation */

/**
 * lombo_nor_erase - erase an address range on the flash chip.
 * @mtd: pointer to mtd info.
 * @instr: pointer to erase info.
 *
 * Return 0 if success, !0 error.
 */
static int lombo_nor_erase(struct mtd_info *mtd, struct erase_info *instr)
{
	struct lombo_nor *flash = container_of(mtd, struct lombo_nor, mtd);
	u32 addr, len;
	uint32_t rem;

	PRT_DBG("at 0x%llx, len %lld\n", (long long)instr->addr,
				(long long)instr->len);

	div_u64_rem(instr->len, mtd->erasesize, &rem);
	if (rem) {
		PRT_ERR("\n");
		return -EINVAL;
	}

	addr = instr->addr;
	len = instr->len;

	mutex_lock(&flash->lock);

	/* whole-chip erase? */
	if (len == flash->mtd.size) {
		if (lombo_erase_chip(flash)) {
			PRT_ERR("\n");
			instr->state = MTD_ERASE_FAILED;
			mutex_unlock(&flash->lock);
			return -EIO;
		}

	/* REVISIT in some cases we could speed up erasing large regions
	 * by using OPCODE_SE instead of OPCODE_BE_4K.  We may have set up
	 * to use "small sector erase", but that's not always optimal.
	 */

	/* "sector"-at-a-time erase */
	} else {
		while (len) {
			if (lombo_erase_sector(flash, addr)) {
				PRT_ERR("\n");
				instr->state = MTD_ERASE_FAILED;
				mutex_unlock(&flash->lock);
				return -EIO;
			}

			addr += mtd->erasesize;
			len -= mtd->erasesize;
		}
	}

	mutex_unlock(&flash->lock);

	instr->state = MTD_ERASE_DONE;
	mtd_erase_callback(instr);

	return 0;
}

/**
 * lombo_nor_read - read an address range from the flash chip.
 * @mtd: pointer to mtd info.
 * @from: flash address from read.
 * @len: length for read.
 * @retlen: actual length for read.
 * @buf: buffer for read.
 *
 * Return 0 if success, !0 error.
 */
static int lombo_nor_read(struct mtd_info *mtd, loff_t from, size_t len,
	size_t *retlen, u_char *buf)
{
	struct lombo_nor *flash = container_of(mtd, struct lombo_nor, mtd);
	int err = 0;

	PRT_DBG("from 0x%08x, len %zd\n", (u32)from, len);

	mutex_lock(&flash->lock);

	/* Wait till previous write/erase is done. */
	if (lombo_wait_till_ready(flash, WIP_BIT, 0)) {
		PRT_ERR("\n");
		*retlen = 0;
		err = __LINE__;
		goto out;
	}

	err = lombo_nor_read_buf(flash->spi, from, buf, len);
	if (err) {
		PRT_ERR("\n");
		*retlen = 0;
		err = __LINE__;
		goto out;
	}

	*retlen = len;

out:
	mutex_unlock(&flash->lock);

	return err;
}

/**
 * lombo_nor_write - write an address range to the flash chip.
 * @mtd: pointer to mtd info.
 * @to: flash address to write.
 * @len: length for read.
 * @retlen: actual length for write.
 * @buf: buffer for write.
 *
 * Return 0 if success, !0 error.
 */
static int lombo_nor_write(struct mtd_info *mtd, loff_t to, size_t len,
	size_t *retlen, const u_char *buf)
{
	struct lombo_nor *flash = container_of(mtd, struct lombo_nor, mtd);
	u32 page_offset, page_size;
	int err = 0;

	PRT_DBG("to 0x%08x, len %zd, buf=%p\n", (u32)to, len, buf);

	mutex_lock(&flash->lock);

	/* Wait until finished previous write command. */
	if (lombo_wait_till_ready(flash, WIP_BIT, 0)) {
		PRT_ERR("\n");
		*retlen = 0;
		err = __LINE__;
		goto out;
	}

	if (lombo_write_enable(flash)) {
		PRT_ERR("\n");
		*retlen = 0;
		err = __LINE__;
		goto out;
	}

	if (lombo_wait_till_ready(flash, WEL_BIT, 1)) {
		PRT_ERR("\n");
		*retlen = 0;
		err = __LINE__;
		goto out;
	}

	page_offset = to & (flash->page_size - 1);
	/* do all the bytes fit onto one page? */
	if (page_offset + len <= flash->page_size) {
		err = lombo_nor_write_buf(flash->spi, to, (size_t)buf, len);
		if (err) {
			PRT_ERR("\n");
			*retlen = 0;
			err = __LINE__;
			goto out;
		}

		*retlen = len;
	} else {
		u32 i;

		/* the size of data remaining on the first page */
		page_size = flash->page_size - page_offset;

		err = lombo_nor_write_buf(flash->spi, to,
				(size_t)buf, page_size);
		if (err) {
			PRT_ERR("\n");
			*retlen = 0;
			err = __LINE__;
			goto out;
		}

		*retlen = page_size;

		/* write everything in flash->page_size chunks */
		for (i = page_size; i < len; i += page_size) {
			page_size = len - i;
			if (page_size > flash->page_size)
				page_size = flash->page_size;

			if (lombo_wait_till_ready(flash, WIP_BIT, 0)) {
				PRT_ERR("\n");
				*retlen = 0;
				err = __LINE__;
				goto out;
			}

			if (lombo_write_enable(flash)) {
				PRT_ERR("\n");
				*retlen = 0;
				err = __LINE__;
				goto out;
			}

			if (lombo_wait_till_ready(flash, WEL_BIT, 1)) {
				PRT_ERR("\n");
				*retlen = 0;
				err = __LINE__;
				goto out;
			}

			err = lombo_nor_write_buf(flash->spi, to + i,
						(size_t)buf + i, page_size);
			if (err) {
				PRT_ERR("\n");
				*retlen = 0;
				err = __LINE__;
				goto out;
			}

			*retlen += page_size;
		}
	}

out:
	mutex_unlock(&flash->lock);

	return err;
}

/**
 * lombo_jedec_probe - probe flash by read jedec id.
 * @spi: Pointer to the SPI device.
 *
 * Return the pointer to spi device id if success, otherwise ERR_PTR.
 */
static const struct spi_device_id *lombo_jedec_probe(struct spi_device *spi)
{
	int			tmp;
	u8			code = OPCODE_RDID;
	u8			id[5];
	u32			jedec;
	u16                     ext_jedec;
	struct flash_info	*info;

	/* JEDEC also defines an optional "extended device information"
	 * string for after vendor-specific data, after the three bytes
	 * we use here.  Supporting some chips might require using it.
	 */
	tmp = __read_status_cmd(spi, code, id, 5);
	if (tmp < 0) {
		pr_debug("%s: error %d reading JEDEC ID\n",
				dev_name(&spi->dev), tmp);
		return ERR_PTR(tmp);
	}

	jedec = id[0];
	jedec = jedec << 8;
	jedec |= id[1];
	jedec = jedec << 8;
	jedec |= id[2];

	ext_jedec = id[3] << 8 | id[4];

	for (tmp = 0; tmp < ARRAY_SIZE(lombo_nor_ids) - 1; tmp++) {
		info = (void *)lombo_nor_ids[tmp].driver_data;
		if (info->jedec_id == jedec) {
			if (info->ext_id != 0 && info->ext_id != ext_jedec)
				continue;
			return &lombo_nor_ids[tmp];
		}
	}

	PRT_ERR("unrecognized JEDEC id 0x%06x\n", jedec);

	return ERR_PTR(-ENODEV);
}

/**
 * lombo_nor_probe - probe function of spi nor.
 * @spi: Pointer to the SPI device.
 *
 * Return 0 if success, 0! error.
 */
static int lombo_nor_probe(struct spi_device *spi)
{
	const struct spi_device_id *id = lombo_nor_ids;
	struct lombo_nor *flash;
	struct flash_info *info;
	struct mtd_part_parser_data ppdata;
	struct device_node __maybe_unused *np = spi->dev.of_node;
	const struct spi_device_id *jid;

#ifdef CONFIG_MTD_OF_PARTS
	if (!of_device_is_available(np))
		return -ENODEV;
#endif

	info = (void *)id->driver_data;
	if (info->jedec_id) {
		jid = lombo_jedec_probe(spi);
		if (IS_ERR(jid)) {
			PRT_ERR("jedec probe failed\n");
			return PTR_ERR(jid);
		} else if (jid != id) {
			PRT_INFO("found %s\n", jid->name);
			id = jid;
			info = (void *)jid->driver_data;
		}
	}

	flash = kzalloc(sizeof(*flash), GFP_KERNEL);
	if (!flash) {
		PRT_ERR("malloc flash failed\n");
		return -ENOMEM;
	}

	flash->spi = spi;
	mutex_init(&flash->lock);
	dev_set_drvdata(&spi->dev, flash);

	flash->mtd.name = dev_name(&spi->dev);
	flash->mtd.type = MTD_NORFLASH;
	flash->mtd.writesize = 1;
	flash->mtd.flags = MTD_CAP_NORFLASH;
	flash->mtd.size = info->sector_size * info->n_sectors;
	flash->mtd._erase = lombo_nor_erase;
	flash->mtd._read = lombo_nor_read;
	flash->mtd._write = lombo_nor_write;

	/* prefer "small sector" erase if possible */
	if (info->flags & SECT_4K) {
		flash->erase_opcode = OPCODE_BE_4K;
		flash->mtd.erasesize = 4096;
	} else {
		flash->erase_opcode = OPCODE_SE;
		flash->mtd.erasesize = info->sector_size;
	}

	ppdata.of_node = spi->dev.of_node;
	flash->mtd.dev.parent = &spi->dev;
	flash->page_size = info->page_size;
	flash->mtd.writebufsize = flash->page_size;

	/* if the device exceeds 16MiB, unsupported now */
	if (flash->mtd.size > 0x1000000) {
		PRT_ERR("capacity must be less than 16MiB\n");
		kfree(flash);
		return -EINVAL;
	}

	PRT_INFO("%s (%lld Kbytes)\n", id->name,
				(long long)flash->mtd.size >> 10);

	PRT_INFO("mtd name=%s, size=0x%llx(%lldMiB) erasesize=0x%.8x(%uKiB)\n",
		flash->mtd.name,
		(long long)flash->mtd.size, (long long)(flash->mtd.size >> 20),
		flash->mtd.erasesize, flash->mtd.erasesize / 1024);

	if (mtd_device_parse_register(&flash->mtd, probes, &ppdata, NULL, 0)) {
		PRT_ERR("register mtd device failed\n");
		kfree(flash);
		return -EINVAL;
	}

	return 0;
}

/**
 * lombo_nor_remove - remove function of spi nor.
 * @spi: Pointer to the SPI device.
 *
 * Return 0 if success, 0! error.
 */
static int lombo_nor_remove(struct spi_device *spi)
{
	struct lombo_nor *flash = dev_get_drvdata(&spi->dev);
	int status = 0;

	/* Clean up MTD stuff. */
	status = mtd_device_unregister(&flash->mtd);
	if (status == 0)
		kfree(flash);

	return 0;
}

static const struct of_device_id lombo_nor_match[] = {
	{ .compatible = "lombo,nor", },
	{},
};
MODULE_DEVICE_TABLE(of, lombo_nor_match);

static struct spi_driver lombo_nor_driver = {
	.driver = {
		.name  = "lombo_nor",
		.owner = THIS_MODULE,
		.of_match_table = lombo_nor_match,
	},
	.probe  = lombo_nor_probe,
	.remove = lombo_nor_remove,
};
module_spi_driver(lombo_nor_driver);

MODULE_AUTHOR("lomboswer@lombotech.com>");
MODULE_DESCRIPTION("Lombo MTD SPI driver for nor flash");
MODULE_LICENSE("GPL");
