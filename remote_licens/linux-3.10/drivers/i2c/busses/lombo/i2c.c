/*
 * i2c.c - I2C host driver code for LomboTech
 * i2c driver driver code implement
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

#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/time.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>
#include <linux/clk.h>
#include <linux/of_i2c.h>
#include <linux/pinctrl/consumer.h>

#include <mach/csp.h>

/* #define ENABLE_TRACE */
#include <mach/debug.h>
#undef MOD_NAME
#define MOD_NAME	"I2C"

#if (defined CONFIG_ARCH_LOMBO_N7) ||	\
	(defined CONFIG_ARCH_LOMBO_N9)
#include "n7/i2c_csp.h"
#include "n7/i2c_const.h"
#else
#error "No supported platform!"
#endif

#define I2C_MAX_CLK_PARENTS_CNT		3
#define I2C_IDLE_TIMEOUT		5000
#define I2C_HS_MASTERCODE_FREQ		400000

#define I2C_APB_GATE			"i2c_apb_gate"
#define I2C_APB_REST			"i2c_apb_reset"
#define I2C_MODULE_CLK			"i2c_module_clk"
#define I2C_CLK_PARENT0			"i2c_clk_parent0"
#define I2C_CLK_PARENT1			"i2c_clk_parent1"

#define I2C_IC_CLKRATE0			(24000000)
#define I2C_IC_CLKRATE1			(50000000)

#define lombo_i2c_chk_abrt(abrtsrc, mask)	\
	((abrtsrc & mask) == mask)

#ifdef msg
#undef msg
#endif

struct lombo_i2c {
	u32			baudrate;	/* the baudrate of the bus */

	u32			ic_clkrate;	/* frequence of ic_clk */
	u32			actual_rate;	/* actual baudrate of the bus */
	u32			speed_mode;

	u32			fs_spk_len;	/* FS mode max spike length */
	u32			hs_spk_len;	/* HS mode max spike length */
	/* to adjust the high period of SCL */
	u32			h_adj_cnt;
	/* to adjust the low period of SCL */
	u32			l_adj_cnt;
	/* start address of the msgs sequence */
	struct i2c_msg		*msgs;
	/* number of the msgs sequence */
	u32			msg_num;
	/* the current msg to send cmd */
	u32			msg_send;
	/* the msgs before it have already been read */
	u32			msg_read;
	/* the current byte of the current msg to send cmd */
	u32			msg_byte_send;
	/* he current byte of the current msg to be read */
	u32			msg_byte_read;
	/* count of the bytes need to read */
	u32			msg_pd_rd_cnt;
	int			msg_err;	/* Error number */
	/* for waiting transfer complete */
	wait_queue_head_t	wait;
	/* mark if the bus is suspended */
	unsigned int		suspended:1;

	unsigned int		irq;		/* irq number of the I2CC */

	u32			reg_base;
	void __iomem		*base;		/* base address of the I2CC */
	struct clk		*clk_i2c;	/* I2CC module clock */
	struct clk		*clk_gate;	/* I2CC apb clock gate */
	struct clk		*clk_reset;	/* I2CC apb clock reset */
	struct clk		*clk_parent0;	/* I2CC module clock parent0 */
	struct clk		*clk_parent1;	/* I2CC module clock parent1 */

	spinlock_t		lock;

	u32			imask;
	u32			int_status;
	u32			abrtsrc;

	/* driver module device structure */
	struct device		*dev;
	/* common i2c adapter structure */
	struct i2c_adapter	adap;

	struct pinctrl          *pctrl;		/* pinctrl of the I2CC */
};

static void lombo_i2c_wait_idle(struct lombo_i2c *i2c);

static int lombo_i2c_batch_cmds(struct lombo_i2c *i2c, u32 entries);

#ifdef CONFIG_OF
static const struct of_device_id lombo_i2c_match[] = {
	{ .compatible = "lombo,n7-i2c", .data = (void *)0 },
	{ .compatible = "lombo,n9-i2c", .data = (void *)0 },
	{},
};
MODULE_DEVICE_TABLE(of, lombo_i2c_match);
#endif

static void lombo_i2c_show_msg(struct lombo_i2c *i2c, u32 msg_seq)
{
	int i = 0;
	int remain = msg_seq;

	for (i = 0; i < i2c->msg_num; i++) {
		if (remain < i2c->msgs[i].len) {
			PRT_DBG("0x%08x: msg_seq=%d,msgs[%d].buf[%d] %s 0x%x\n",
				i2c->reg_base, msg_seq, i, remain,
				(i2c->msgs[i].flags & I2C_M_RD) ?
				"READ" : "WRITE",
				i2c->msgs[i].buf[remain]);
			return;
		}
		remain -= i2c->msgs[i].len;
	}

	PRT_ERR("invalid msg_seq (%d)\n", msg_seq);

}

/**
 * lombo_i2c_start_msgs - start transfer the messages
 * @i2c: point to i2c host structure
 * @msgs: start address of the messages to transfer
 * @num: number of the messages to transfer
 *
 * return 0 means ok; others means error
 */
static int lombo_i2c_start_msgs(struct lombo_i2c *i2c,
				struct i2c_msg *msgs, u32 num)
{
	unsigned long flags;
	int ret = 0;

	PRT_TRACE_BEGIN("i2c_base=%p,msgs=0x%x,num=%d\n", i2c->base, msgs, num);

	if ((i2c == NULL) || (msgs == NULL) || (num == 0)) {
		PRT_ERR("i2c=0x%p,msgs=0x%p,num=%u\n", i2c, msgs, num);
		return -EINVAL;
	}

#ifdef DEBUG
	PRT_DBG("%p\n", i2c->base);

	{
		int i, j;
		for (i = 0; i < num; i++) {
			PRT_DBG("%p: msgs[%d].len=%d\n",
				i2c->base, i, msgs[i].len);

			for (j = 0; j < msgs[i].len; j++)
				PRT_DBG("%p: msgs[%d].buf[%d]=0x%02x\n",
					i2c->base, i, j, msgs[i].buf[j]);
		}
	}
#endif

	spin_lock_irqsave(&i2c->lock, flags);

	i2c->msgs = msgs;
	i2c->msg_num = num;
	i2c->msg_send = 0;
	i2c->msg_read = 0;
	i2c->msg_byte_send = 0;
	i2c->msg_byte_read = 0;
	i2c->msg_pd_rd_cnt = 0;

	i2c->msg_err = 0;
	i2c->int_status = 0;
	spin_unlock_irqrestore(&i2c->lock, flags);

	lombo_i2c_wait_idle(i2c);

	spin_lock_irqsave(&i2c->lock, flags);

	/*
	 * clear all int
	 */
	csp_i2c_clr_tx_abrt(i2c->base, I2C_TX_ABTSRC_ALL_MASK);
	csp_i2c_clr_pd(i2c->base, I2C_INT_ALL_MASK);

	/*
	 * transfer configure
	 */
	/* set tar address */
	csp_i2c_set_tar_addr(i2c->base, msgs[0].addr,
				!!(msgs[0].flags & I2C_M_TEN));
	/* set TX/RX FIFO threshold */
	csp_i2c_set_fifo_thresh(i2c->base, I2C_TX_FIFO_DEPTH / 2,
				I2C_RX_FIFO_DEPTH / 2);
	/* enable_i2c */
	csp_i2c_enable(i2c->base, 1);

	i2c->imask = I2C_INT_ALL_ERR | I2C_INT_TX_EMPTY |
			I2C_INT_RX_FULL | I2C_INT_STOP_DET;

	/*
	 * we push cmds as much as possible to TX_FIFO before
	 * enable any interrupts.
	 */
	ret = lombo_i2c_batch_cmds(i2c, I2C_TX_FIFO_DEPTH);
	if (ret != 0) {
		PRT_ERR("%p\n", i2c->base);
		goto out;
	}

	csp_i2c_clr_pd(i2c->base, I2C_INT_TX_EMPTY | I2C_INT_RX_FULL);
	/* enable TX_EMPTY, RX_FULL, STOP_DET interrupts */
	csp_i2c_enable_ints(i2c->base, i2c->imask);

	mmiowb();
out:
	spin_unlock_irqrestore(&i2c->lock, flags);
	PRT_TRACE_END("\n");
	return ret;
}

/**
 * lombo_i2c_send_cmds - send commands to i2c host TX_FIFO with/without STOP
 * @i2c: point to i2c host structure
 * @cmd: read/write command (I2C_CMD_READ/I2C_CMD_WRITE)
 * @bytes: start address of the bytes flow
 * @count: count of the bytes to handle
 * @stop: 0,withou STOP; 1, send STOP when the last byte has been transfered
 */
static void lombo_i2c_send_cmds(struct lombo_i2c *i2c, u32 cmd, u8 *bytes,
				u32 count, u32 stop)
{
	u32 idx = 0;

	PRT_TRACE_BEGIN("i2c=0x%p,cmd=%d,bytes=0x%p,count=%d,stop=%d\n",
			i2c, cmd, bytes, count, stop);

	for (idx = 0; idx < count - 1; idx++) {
		csp_i2c_send_cmd(i2c->base, cmd, bytes[idx],
			I2C_NO_STOP, I2C_NO_RESTART);
	}

	csp_i2c_send_cmd(i2c->base, cmd, bytes[count - 1],
			stop, I2C_NO_RESTART);
	PRT_TRACE_END("\n");
}

/**
 * lombo_i2c_batch_cmds - send batch commands to TX_FIFO, as much as possible
 * @i2c: point to i2c host structure
 * @entries: available entries of the TX_FIFO
 *
 * return 0 means ok; others means error
 */
static int lombo_i2c_batch_cmds(struct lombo_i2c *i2c, u32 entries)
{
	struct i2c_msg *msg = NULL;
	u32 msg_send = 0;
	u32 msg_byte_send = 0;
	u32 stop = I2C_NO_STOP;
	u32 batch_cnt = 0;
	u32 done = 0;
	u32 cmd = 0;
	int ret = 0;

	if (entries > I2C_TX_FIFO_DEPTH) {
		PRT_ERR("%p: etries=%d\n", i2c->base, entries);
		return -EINVAL;
	}

	PRT_TRACE_BEGIN("i2c=0x%p,entries=%d\n", i2c, entries);
	PRT_DBG("%p: etries=%d\n", i2c->base, entries);

	if (i2c->msg_send == i2c->msg_num) {
		PRT_WARN("0x%p: all messages have been sent\n", i2c->base);
		PRT_WARN("0x%p: msg_send=%d,msg_num=%d\n",
			i2c->base, i2c->msg_send, i2c->msg_num);
		i2c->imask &= ~I2C_INT_TX_EMPTY;
		csp_i2c_disable_ints(i2c->base, I2C_INT_TX_EMPTY);
		goto out;
	}

	while (done < entries) {
		msg_send = i2c->msg_send;
		msg_byte_send = i2c->msg_byte_send;

		msg = &i2c->msgs[msg_send];

		batch_cnt = msg->len - msg_byte_send;
		if (batch_cnt > entries - done)
			batch_cnt = entries - done;

		PRT_DBG("%p: batch_cnt=%d\n", i2c->base, batch_cnt);
		if (batch_cnt == 0)
			break;

		/*
		 * It is the last byte of the transfer,
		 * so after this no command need to be sent,
		 * mark STOP and disable TX_EMPTY.
		 */
		if ((msg_send == i2c->msg_num - 1) &&
			(msg_byte_send + batch_cnt == msg->len)) {
			stop = I2C_SEND_STOP;
			i2c->imask &= ~I2C_INT_TX_EMPTY;
			csp_i2c_disable_ints(i2c->base, I2C_INT_TX_EMPTY);
		}

		if (msg->flags & I2C_M_RD) {
			cmd = I2C_CMD_READ;
			i2c->msg_pd_rd_cnt += batch_cnt;
			PRT_DBG("msg_pd_rd_cnt=%d\n", i2c->msg_pd_rd_cnt);
		} else
			cmd = I2C_CMD_WRITE;

		lombo_i2c_send_cmds(i2c, cmd, &msg->buf[msg_byte_send],
				batch_cnt, stop);

		i2c->msg_byte_send += batch_cnt;
		/* current message is all sent, walk to next */
		if (i2c->msg_byte_send == msg->len) {
			i2c->msg_send++;
			i2c->msg_byte_send = 0;
		}
		done += batch_cnt;

		if (stop == I2C_SEND_STOP) {
			if (i2c->msg_send != i2c->msg_num) {
				PRT_WARN("0x%p: stop not at transfer end\n",
					i2c->base);
			}
			break;
		}
	}

	if (done > entries) {
		ret = -EINVAL;
		PRT_ERR("%p: etries=%d\n", i2c->base, entries);
	}

out:
	PRT_TRACE_END("\n");
	return ret;
}

/**
 * lombo_i2c_batch_read - read bytes from TX_FIFO, as much as possible
 * @i2c: point to i2c host structure
 * @entries: available entries of the RX_FIFO
 *
 * return 0 means ok; others means error
 */
static int lombo_i2c_batch_read(struct lombo_i2c *i2c, u32 entries)
{
	struct i2c_msg *msg = NULL;
	u32 msg_read = 0;
	u32 msg_byte_read = 0;
	u32 batch_cnt = 0;
	u32 done = 0;
	u32 idx = 0;
	u8 *buf = NULL;
	int ret = 0;

	if ((entries > i2c->msg_pd_rd_cnt) ||
		(entries > I2C_RX_FIFO_DEPTH)) {
		PRT_ERR("%p: etries=%d,msg_pd_rd_cnt=%d\n",
			i2c->base, entries, i2c->msg_pd_rd_cnt);
		return -EINVAL;
	}

	PRT_TRACE_BEGIN("i2c=0x%p,entries=%d\n", i2c, entries);
	PRT_DBG("%p: etries=%d,msg_pd_rd_cnt=%d\n",
		i2c->base, entries, i2c->msg_pd_rd_cnt);

	if (i2c->msg_read == i2c->msg_num) {
		PRT_WARN("0x%p: all messages have been read\n", i2c->base);
		PRT_WARN("0x%p: msg_send=%d,msg_num=%d\n",
			i2c->base, i2c->msg_send, i2c->msg_num);
		i2c->imask &= ~I2C_INT_RX_FULL;
		csp_i2c_disable_ints(i2c->base, I2C_INT_RX_FULL);
		goto out;
	}

	while (done < entries) {
		msg_read = i2c->msg_read;
		msg_byte_read = i2c->msg_byte_read;

		msg = &i2c->msgs[msg_read];

		/* skip the write message */
		if ((msg->flags & I2C_M_RD) == 0) {
			if (msg_byte_read != 0) {
				PRT_ERR("read bytes in write message!\n");
				ret = -EINVAL;
				goto out;
			}

			i2c->msg_read++;
			continue;
		}

		batch_cnt = msg->len - msg_byte_read;
		if (batch_cnt > entries - done)
			batch_cnt = entries - done;

		buf = &msg->buf[msg_byte_read];
		for (idx = 0; idx < batch_cnt; idx++)
			buf[idx] = csp_i2c_read_byte(i2c->base);

		i2c->msg_byte_read += batch_cnt;
		/* current message is completed, walk to next */
		if (i2c->msg_byte_read == msg->len) {
			i2c->msg_byte_read = 0;
			i2c->msg_read++;
		}
		done += batch_cnt;

		/* all data have been read  */
		if (i2c->msg_read == i2c->msg_num)
			break;
	}

	if (done != entries)
		ret = -EIO;

	/* update the pending read count */
	i2c->msg_pd_rd_cnt -= done;

out:
	if (ret != 0)
		PRT_ERR("%p: etries=%d,done=%d\n", i2c->base, entries, done);

	PRT_TRACE_END("\n");
	return ret;
}

/**
 * lombo_i2c_master_complete
 * @i2c: point to i2c host structure
 * @ret: return code, 0 means ok; others means error
 *
 * complete the message and wake up the caller, using the given return code,
 * or zero to mean ok.
*/
static inline void lombo_i2c_master_complete(struct lombo_i2c *i2c, int ret)
{
	PRT_TRACE_BEGIN("i2c=0x%p,ret=%d\n", i2c, ret);
	PRT_DBG("%p: master_complete %d\n", i2c->base, ret);

	i2c->msgs = NULL;
	i2c->msg_num = 0;
	i2c->msg_err = ret;

	wake_up(&i2c->wait);
	PRT_TRACE_END("\n");
}

/**
 * lombo_i2c_stop - stop i2c transfer, using the given return code
 * @i2c: point to i2c host structure
 * @ret: return code, 0 means ok; others means error
*/
static inline void lombo_i2c_stop(struct lombo_i2c *i2c, int ret)
{
	PRT_TRACE_BEGIN("i2c=0x%p,ret=%d\n", i2c, ret);
	PRT_DBG("%p: STOP\n", i2c->base);

	if (ret != 0) {
		i2c->abrtsrc = csp_i2c_get_abrt(i2c->base);

		PRT_ERR("%p: baudrate=%d,ic_clkrate=%d\n",
			i2c->base, i2c->baudrate, i2c->ic_clkrate);

		if (i2c->msgs != NULL) {
			u32 rx_flush_cnt = 0;

			PRT_ERR("%p: device address 0x%02x\n",
				i2c->base, i2c->msgs[0].addr);

			rx_flush_cnt =
				(i2c->abrtsrc & I2C_TX_FLUSH_CNT_MASK) >>
				I2C_TX_FLUSH_CNT_SHIFT;
			if (csp_i2c_chk_pd(i2c->base, I2C_INT_TX_ABRT)) {
				PRT_ERR("%p: rx_flush_cnt %d\n",
					i2c->base, rx_flush_cnt);
				lombo_i2c_show_msg(i2c, rx_flush_cnt);
			}
		}

		if (lombo_i2c_chk_abrt(i2c->abrtsrc, I2C_ABT_MST_ARB_LOST)) {
			/* deal with arbitration loss */
			PRT_ERR("%p: deal with arbitration loss\n",
				i2c->base);
		}

		if (lombo_i2c_chk_abrt(i2c->abrtsrc, I2C_ABT_MST_DIS))
			PRT_ERR("%p: master disabled\n", i2c->base);

		if (lombo_i2c_chk_abrt(i2c->abrtsrc, I2C_ABT_10B_RD_NORSTRT))
			PRT_ERR("%p: restart is disbled with 10bit address\n",
				i2c->base);

		if (lombo_i2c_chk_abrt(i2c->abrtsrc, I2C_ABT_SB_NORSTRT))
			PRT_ERR("%p: send START when restart is disbled\n",
				i2c->base);

		if (lombo_i2c_chk_abrt(i2c->abrtsrc, I2C_ABT_HS_NORSTRT))
			PRT_ERR("%p: in HS mode when restart is disbled\n",
				i2c->base);

		if (lombo_i2c_chk_abrt(i2c->abrtsrc, I2C_ABT_SB_ACKDET))
			PRT_ERR("%p: START byte is acknowledged\n",
				i2c->base);

		if (lombo_i2c_chk_abrt(i2c->abrtsrc, I2C_ABT_HS_ACKDET))
			PRT_ERR("%p: HS master code is acknowledged\n",
				i2c->base);

		if (lombo_i2c_chk_abrt(i2c->abrtsrc, I2C_ABT_GCALL_READ))
			PRT_ERR("%p: treated General Call as read\n",
				i2c->base);

		if (lombo_i2c_chk_abrt(i2c->abrtsrc, I2C_ABT_GCALL_NAK))
			PRT_ERR("%p: sent General Call but no ask\n",
				i2c->base);

		if (lombo_i2c_chk_abrt(i2c->abrtsrc, I2C_ABT_TXDATA_NAK))
			PRT_ERR("%p: no ask for the sent data\n",
				i2c->base);

		if (lombo_i2c_chk_abrt(i2c->abrtsrc, I2C_ABT_10ADDR2_NAK))
			PRT_ERR("%p: no ask for the 2nd address byte\n",
				i2c->base);

		if (lombo_i2c_chk_abrt(i2c->abrtsrc, I2C_ABT_10ADDR1_NAK))
			PRT_ERR("%p: no ask for the 1nd byte of 10bit addr\n",
				i2c->base);

		if (lombo_i2c_chk_abrt(i2c->abrtsrc, I2C_ABT_7B_ADDR_NAK))
			PRT_ERR("%p: no ask for the 7bit address\n",
				i2c->base);

		csp_i2c_dumpregs(i2c->base);
	}

	csp_i2c_disable_ints(i2c->base, I2C_INT_ALL_MASK);
	csp_i2c_clr_pd(i2c->base, I2C_INT_ALL_MASK);
	csp_i2c_enable(i2c->base, 0);
	lombo_i2c_master_complete(i2c, ret);
	PRT_TRACE_END("");
}

/**
 * lombo_i2c_irq
 *
 * top level IRQ servicing routine
*/
static irqreturn_t lombo_i2c_irq(int irqno, void *dev_id)
{
	struct lombo_i2c *i2c = dev_id;
	u32 int_status = 0;
	irqreturn_t result = IRQ_HANDLED;
	int ret = 0;

	PRT_TRACE_BEGIN("irqno=%d,dev_id=0x%p", irqno, dev_id);
	spin_lock(&i2c->lock);

	if (csp_i2c_chk_err(i2c->base) != 0) {
		PRT_ERR("%p: transfer error!\n", i2c->base);
		lombo_i2c_stop(i2c, -EIO);
		goto out;
	}

	int_status = csp_i2c_get_int_pd(i2c->base);
	if (!int_status) {
		PRT_WARN("%p: no i2c interrupt pendings\n", i2c->base);
		result = IRQ_NONE;
		goto out;
	}

	/* STOP_DET, finish the transfer */
	if ((int_status & I2C_INT_STOP_DET) != 0) {
		/*
		 * clear the pending bits that to be handled
		 */
		csp_i2c_clr_pd(i2c->base,
				I2C_INT_RX_FULL | I2C_INT_TX_EMPTY |
				I2C_INT_STOP_DET);
		csp_i2c_disable_ints(i2c->base,
				I2C_INT_RX_FULL | I2C_INT_TX_EMPTY |
				I2C_INT_STOP_DET);

		/* read the data from RX_FIFO, if any */
		if (csp_i2c_chk_status(i2c->base, I2C_STATUS_RFNE) == 1) {
			u32 rd_entries = 0;

			rd_entries = csp_i2c_get_rx_fl(i2c->base);
			ret = lombo_i2c_batch_read(i2c, rd_entries);
			if (ret != 0) {
				PRT_ERR("%p\n", i2c->base);
				lombo_i2c_stop(i2c, -EIO);
				goto out;
			}

			/* after read, the pending read count should be 0 */
			if (i2c->msg_pd_rd_cnt != 0) {
				PRT_ERR("%p: i2c->msg_pd_rd_cnt=%d\n",
					i2c->base, i2c->msg_pd_rd_cnt);
				lombo_i2c_stop(i2c, -EIO);
				goto out;
			}
		}
		lombo_i2c_stop(i2c, 0);
		goto out;
	}

	/*
	 * When TX_EMPTY interrupt is enable (means that there are commands
	 * to be sent), and TX_EMPTY is raised, than we continue to send the
	 * rest of the commands.
	 */
	if (((i2c->imask & I2C_INT_TX_EMPTY) != 0) &&
		((int_status & I2C_INT_TX_EMPTY) != 0)) {
		/*
		 * disable TX_EMPTY interrupt before sending the commands
		 * to avoid the interrupt raise unexecptedly.
		 */
		csp_i2c_disable_ints(i2c->base, I2C_INT_TX_EMPTY);
		ret = lombo_i2c_batch_cmds(i2c, I2C_TX_FIFO_DEPTH / 2);
		if (ret != 0) {
			PRT_ERR("%p\n", i2c->base);
			lombo_i2c_stop(i2c, -EIO);
			goto out;
		}
		csp_i2c_clr_pd(i2c->base, I2C_INT_TX_EMPTY);
		/* if there are commands to be sent, enable the TX_EMPTY int */
		if ((i2c->imask & I2C_INT_TX_EMPTY) != 0)
			csp_i2c_enable_ints(i2c->base, I2C_INT_TX_EMPTY);
	}

	/*
	 * RX_FULL pending raised means the RX_FIFO has valid data more than
	 * the RX_FIFO threshold (I2C_RX_FIFO_DEPTH / 2 - 1)
	 */
	if (((i2c->imask & I2C_INT_RX_FULL) != 0) &&
		((int_status & I2C_INT_RX_FULL) != 0)) {
		csp_i2c_disable_ints(i2c->base, I2C_INT_RX_FULL);
		ret = lombo_i2c_batch_read(i2c, I2C_TX_FIFO_DEPTH / 2);
		if (ret != 0) {
			PRT_ERR("%p\n", i2c->base);
			lombo_i2c_stop(i2c, -EIO);
			goto out;
		}
		csp_i2c_clr_pd(i2c->base, I2C_INT_RX_FULL);
		/* if there are data to be read, enable the RX_FULL int */
		if ((i2c->imask & I2C_INT_RX_FULL) != 0)
			csp_i2c_enable_ints(i2c->base, I2C_INT_RX_FULL);
	}

 out:
	PRT_DBG("end %p\n", i2c->base);
	spin_unlock(&i2c->lock);
	PRT_TRACE_END("\n");
	return result;
}

/**
 * lombo_i2c_wait_idle
 *
 * wait for the i2c bus to become idle.
*/
static void lombo_i2c_wait_idle(struct lombo_i2c *i2c)
{
	ktime_t start, now;
	unsigned long delay;
	int spins;

	PRT_TRACE_BEGIN("i2c=0x%08x", i2c);

	/* ensure the stop has been through the bus */

	PRT_DBG("%p: waiting for bus idle\n", i2c->base);

	start = now = ktime_get();

	/*
	 * Most of the time, the bus is already idle within a few usec of the
	 * end of a transaction.  However, really slow i2c devices can stretch
	 * the clock, delaying STOP generation.
	 *
	 * On slower SoCs this typically happens within a very small number of
	 * instructions so busy wait briefly to avoid scheduling overhead.
	 */
	spins = 3;
	while (--spins &&
		(csp_i2c_chk_status(i2c->base, I2C_STATUS_ACTIVITY) != 0))
			cpu_relax();

	/*
	 * If we do get an appreciable delay as a compromise between idle
	 * detection latency for the normal, fast case, and system load in the
	 * slow device case, use an exponential back off in the polling loop,
	 * up to 1/10th of the total timeout, then continue to poll at a
	 * constant rate up to the timeout.
	 */
	delay = 1;
	while ((csp_i2c_chk_status(i2c->base, I2C_STATUS_ACTIVITY) != 0) &&
	       ktime_us_delta(now, start) < I2C_IDLE_TIMEOUT) {
		usleep_range(delay, 2 * delay);
		if (delay < I2C_IDLE_TIMEOUT / 10)
			delay <<= 1;
		now = ktime_get();
	}

	if (csp_i2c_chk_status(i2c->base, I2C_STATUS_ACTIVITY) != 0)
		PRT_WARN("%p: timeout waiting for bus idle\n", i2c->base);
	PRT_TRACE_END("\n");
}

/* lombo_i2c_doxfer - starts an i2c transfer
 * @i2c: point to i2c host structure
 * @msgs: start address of the messages to transfer
 * @num: number of the messages to transfer
 *
 * return message count when success; return error code when failed
 *   TODO: return 0 when address NAK
 */
static int lombo_i2c_doxfer(struct lombo_i2c *i2c,
			      struct i2c_msg *msgs, int num)
{
	unsigned long timeout;
	int ret;

	PRT_TRACE_BEGIN("i2c=0x%08x,msgs=0x%08x,num=%d", i2c, msgs, num);

	if (i2c->suspended)
		return -EIO;

	ret = lombo_i2c_start_msgs(i2c, msgs, num);
	if (ret != 0) {
		PRT_ERR("%p: cannot get bus (error %d)\n", i2c->base, ret);
		ret = -EAGAIN;
		goto out;
	}

	timeout = wait_event_timeout(i2c->wait, i2c->msg_num == 0, HZ * 5);

	if (i2c->msg_err != 0)
		ret = i2c->msg_err;
	else
		ret = num;

	/* having these next two as dev_err() makes life very
	 * noisy when doing an i2cdetect */

	if (timeout == 0) {
		PRT_DBG("%p: timeout\n", i2c->base);
		lombo_i2c_stop(i2c, -EIO);
	} else if (ret != num)
		PRT_DBG("%p: incomplete xfer (%d)\n", i2c->base, ret);

	/*
	 * TODO: return 0 when address NAK,
	 *   so that we can distinguish whether the device is busy or
	 *   errors occur.
	 */

 out:
	PRT_TRACE_END("\n");
	return ret;
}

/**
 * lombo_i2c_xfer
 *
 * @i2c: point to i2c host structure
 * @msgs: start address of the messages to transfer
 * @num: number of the messages to transfer
 *
 * first port of call from the i2c bus code when an message needs
 * transferring across the i2c bus.
 *
 * return message count when success; return error code when failed
 *   TODO: return 0 when address NAK
 */
static int lombo_i2c_xfer(struct i2c_adapter *adap,
			struct i2c_msg *msgs, int num)
{
	struct lombo_i2c *i2c = (struct lombo_i2c *)adap->algo_data;
	int retry;
	int ret;

	pm_runtime_get_sync(&adap->dev);
	clk_prepare_enable(i2c->clk_gate);
	clk_prepare_enable(i2c->clk_i2c);

	PRT_TRACE_BEGIN("adap=0x%08x,msgs=0x%08x,num=%d", adap, msgs, num);
	for (retry = 0; retry < adap->retries; retry++) {

		ret = lombo_i2c_doxfer(i2c, msgs, num);

		if (ret != -EAGAIN) {
			clk_disable_unprepare(i2c->clk_i2c);
			clk_disable_unprepare(i2c->clk_gate);
			pm_runtime_put(&adap->dev);
			return ret;
		}

		PRT_DBG("%p: Retrying transmission (%d)\n", i2c->base, retry);

		udelay(100);
	}

	clk_disable_unprepare(i2c->clk_i2c);
	clk_disable_unprepare(i2c->clk_gate);
	pm_runtime_put(&adap->dev);
	PRT_TRACE_END("\n");
	return -EREMOTEIO;
}

/* declare our i2c functionality */
static u32 lombo_i2c_func(struct i2c_adapter *adap)
{
	return I2C_FUNC_I2C | I2C_FUNC_SMBUS_EMUL | I2C_FUNC_NOSTART;
}

/* i2c bus registration info */

static const struct i2c_algorithm lombo_i2c_algorithm = {
	.master_xfer		= lombo_i2c_xfer,
	.functionality		= lombo_i2c_func,
};

#ifndef CONFIG_ARCH_LOMBO_N7V0_FPGA
static int lombo_i2c_ic_clkrate(struct lombo_i2c *i2c)
{
	struct clk *clk_parent;
	int ret = 0;

	if ((i2c->baudrate == 0) || (i2c->baudrate > 3400000)) {
		PRT_ERR("%p: i2c->baudrate=%d\n", i2c->base, i2c->baudrate);
		return -EINVAL;
	}

	if (i2c->baudrate <= 1000000) {
		clk_parent = i2c->clk_parent0;
		i2c->ic_clkrate = I2C_IC_CLKRATE0;
	} else {
		clk_parent = i2c->clk_parent1;
		i2c->ic_clkrate = I2C_IC_CLKRATE1;
	}

	ret = clk_set_parent(i2c->clk_i2c, clk_parent);
	if (ret != 0)
		return ret;

	ret = clk_set_rate(i2c->clk_i2c, i2c->ic_clkrate);
	if (ret != 0)
		return ret;

	i2c->ic_clkrate = clk_get_rate(i2c->clk_i2c);

	if (i2c->baudrate * 24 <= i2c->ic_clkrate)
		i2c->actual_rate = i2c->baudrate;
	else
		i2c->actual_rate = i2c->ic_clkrate / 24;

	return 0;
}
#else
static int lombo_i2c_ic_clkrate(struct lombo_i2c *i2c)
{
	if ((i2c->baudrate == 0) || (i2c->baudrate > 3400000)) {
		PRT_ERR("%p: i2c->baudrate=%d\n", i2c->base, i2c->baudrate);
		return -EINVAL;
	}

	i2c->ic_clkrate = 24000000;

	if (i2c->baudrate * 24 <= i2c->ic_clkrate)
		i2c->actual_rate = i2c->baudrate;
	else
		i2c->actual_rate = i2c->ic_clkrate / 24;

	return 0;
}
#endif

/* lombo_i2c_init
 *
 * initialise the controller
*/

static int lombo_i2c_init(struct lombo_i2c *i2c)
{
	u32 half_cycl_cnt = 0;
	u32 h_delta_cnt = 0;
	u32 mc_half_cycl_cnt = 0;
	u32 hs_h_delta_cnt = 0;
	int ret = 0;

	/*
	 * ic clock configure
	 */
	ret = lombo_i2c_ic_clkrate(i2c);
	if (ret != 0)
		return -EINVAL;

	/*
	 * baudrate and bus speed mode configure
	 */

	/* BaudRate_cfg
	 * SS	freq <= 100K
	 * FS	100K <= freq <= 400K
	 * FS+	400K <= freq <= 1M
	 * HS	1M <= freq <= 3.4M
	 */
	half_cycl_cnt = i2c->ic_clkrate / i2c->actual_rate / 2;
	h_delta_cnt = i2c->fs_spk_len + 3 + i2c->h_adj_cnt;

	mc_half_cycl_cnt = i2c->ic_clkrate / I2C_HS_MASTERCODE_FREQ / 2;
	hs_h_delta_cnt = i2c->hs_spk_len + 3 + i2c->h_adj_cnt;

	if (i2c->actual_rate <= 100000) {
		i2c->speed_mode = I2C_SS_MODE;
		csp_i2c_set_speed_mode(i2c->base, I2C_SS_MODE);
		csp_i2c_set_ss_baudrate(i2c->base,
			half_cycl_cnt - h_delta_cnt,
			half_cycl_cnt - i2c->l_adj_cnt);
	} else if (i2c->actual_rate <= 1000000) {
		i2c->speed_mode = I2C_FS_MODE;
		csp_i2c_set_speed_mode(i2c->base, I2C_FS_MODE);
		csp_i2c_set_fs_baudrate(i2c->base,
			half_cycl_cnt - h_delta_cnt,
			half_cycl_cnt - i2c->l_adj_cnt);
	} else {
		i2c->speed_mode = I2C_HS_MODE;
		hs_h_delta_cnt = i2c->hs_spk_len + 3 + i2c->h_adj_cnt;
		csp_i2c_set_speed_mode(i2c->base, I2C_HS_MODE);
		csp_i2c_set_hs_baudrate(i2c->base,
			half_cycl_cnt - hs_h_delta_cnt,
			half_cycl_cnt - i2c->l_adj_cnt);

		csp_i2c_set_fs_baudrate(i2c->base,
			mc_half_cycl_cnt - h_delta_cnt,
			mc_half_cycl_cnt - i2c->l_adj_cnt);
	}

	/*
	 * spike len configure
	 */
	csp_i2c_cfg_spk_len(i2c->base, i2c->fs_spk_len, i2c->hs_spk_len);

	/*
	 * timing
	 */

	/* Tx_hold & Rx_hold
	 * SS_mode 3.45us > hold_time > 300ns =  7.3 cycle(24MHz)
	 * FS_mode 0.9 us > hold_time > 300ns
	 * HS_mode 0.45us > hold_time > 300ns
	 */
	/* setup_time
	 * SS_mode   set_time > 250ns =  6 cycle (24Mhz)
	 * FS_mode   set_time > 100ns
	 * HS_mode   set_time > 50 ns
	 */
	csp_i2c_set_timing(i2c->base, half_cycl_cnt / 2,
			half_cycl_cnt / 2, half_cycl_cnt / 2);

	/*
	 * operate mode
	 */

	/* master-en & slave-dis */
	csp_i2c_set_master_mode(i2c->base);

	/* restart function enable */
	csp_i2c_enable_restart(i2c->base, 1);

	/* 在RX_FIFO满时，保持SCL线为低电平，让从机暂停发送数据 */
	csp_i2c_enable_rx_ffh(i2c->base, 1);

#if 0	/* default, no need to set */
	/*
	 * INT_behavior
	 */

	/* 仅在master活跃时发起STOP_DET中断 */
	csp_i2c_master_stop_det(i2c->base, I2C_STOP_DET_IN_MASTER_ACTIV);

	/* 当TX_FIFO为空，且最后取出的命令已完成时发起TX_EMPTY中断 */
	csp_i2c_tx_empty_mode(i2c->base, I2C_TX_EMPTY_IN_CMD_DONE);
#endif

	PRT_INFO("%p: bus frequency set to %d KHz\n",
		i2c->base, i2c->actual_rate / 1000);

	return 0;
}

#ifdef CONFIG_OF
/* lombo_i2c_parse_dt
 *
 * Parse the device tree node and retreive the platform data.
*/
static int
lombo_i2c_parse_dt(struct device_node *np, struct lombo_i2c *i2c)
{
	int ret = 0;

	if (!np)
		return -ENODEV;

	of_property_read_u32(np, "fs_spk_len", &i2c->fs_spk_len);
	of_property_read_u32(np, "hs_spk_len", &i2c->fs_spk_len);
	ret = of_property_read_u32(np, "max_bus_freq", &i2c->baudrate);
	if ((ret < 0) || (i2c->baudrate == 0)) {
		PRT_ERR("invalid max_bus_freq (%d)\n", i2c->baudrate);
		return -EINVAL;
	}

	return ret;
}
#else
static int
lombo_i2c_parse_dt(struct device_node *np, struct lombo_i2c *i2c)
{
	return 0;
}
#endif


/* lombo_i2c_probe
 *
 * called by the bus driver when a suitable device is found
*/
static int lombo_i2c_probe(struct platform_device *pdev)
{
	struct lombo_i2c *i2c;
	struct resource *res;
	int ret;

	if (!pdev->dev.of_node) {
		dev_err(&pdev->dev, "no of_node of the bus\n");
		return -EINVAL;
	}

	i2c = devm_kzalloc(&pdev->dev, sizeof(struct lombo_i2c), GFP_KERNEL);
	if (!i2c) {
		dev_err(&pdev->dev, "no memory for state\n");
		return -ENOMEM;
	}

	strlcpy(i2c->adap.name, "lombo-i2c", sizeof(i2c->adap.name));
	i2c->adap.owner   = THIS_MODULE;
	i2c->adap.algo    = &lombo_i2c_algorithm;
	i2c->adap.retries = 2;
	i2c->adap.class   = I2C_CLASS_HWMON | I2C_CLASS_SPD;

	spin_lock_init(&i2c->lock);
	init_waitqueue_head(&i2c->wait);

	/* find the clock and enable it */

	i2c->dev = &pdev->dev;

	ret = lombo_i2c_parse_dt(pdev->dev.of_node, i2c);
	if (ret != 0) {
		dev_err(&pdev->dev, "lombo_i2c_parse_dt failed\n");
		return ret;
	}

	i2c->clk_gate = devm_clk_get(&pdev->dev, I2C_APB_GATE);
	if (IS_ERR_OR_NULL(i2c->clk_gate)) {
		dev_err(&pdev->dev, "failed to get apb i2c clock gate!\n");
		return PTR_ERR(i2c->clk_gate);
	}

	i2c->clk_reset = devm_clk_get(&pdev->dev, I2C_APB_REST);
	if (IS_ERR_OR_NULL(i2c->clk_reset)) {
		dev_err(&pdev->dev, "failed to get apb i2c clock reset!\n");
		return PTR_ERR(i2c->clk_reset);
	}

	i2c->clk_i2c = devm_clk_get(&pdev->dev, I2C_MODULE_CLK);
	if (IS_ERR_OR_NULL(i2c->clk_i2c)) {
		dev_err(&pdev->dev, "failed to get i2c module clock!\n");
		return PTR_ERR(i2c->clk_i2c);
	}

	i2c->clk_parent0 = devm_clk_get(i2c->dev, I2C_CLK_PARENT0);
	if (IS_ERR_OR_NULL(i2c->clk_parent0)) {
		PRT_ERR("%p: failed to get i2c_clk_parent0!\n",
			i2c->base);
		return PTR_ERR(i2c->clk_parent0);
	}

	i2c->clk_parent1 = devm_clk_get(i2c->dev, I2C_CLK_PARENT1);
	if (IS_ERR_OR_NULL(i2c->clk_parent1)) {
		PRT_ERR("%p: failed to get i2c_clk_parent1!\n",
			i2c->base);
		return PTR_ERR(i2c->clk_parent1);
	}

	clk_prepare_enable(i2c->clk_reset);

	dev_dbg(&pdev->dev, "clock source %p\n", i2c->clk_i2c);

	/* map the registers */

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	i2c->base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(i2c->base)) {
		dev_err(&pdev->dev, "failed to remap i2c IO!\n");
		return PTR_ERR(i2c->base);
	}
	i2c->reg_base = (u32)res->start;

	dev_dbg(&pdev->dev, "registers %p (%p)\n",
		i2c->base, res);

	/* setup info block for the i2c core */

	i2c->adap.algo_data = i2c;
	i2c->adap.dev.parent = &pdev->dev;

	i2c->pctrl = devm_pinctrl_get_select_default(i2c->dev);

	/* initialise the i2c controller */
	clk_prepare_enable(i2c->clk_gate);
	ret = lombo_i2c_init(i2c);
	clk_disable_unprepare(i2c->clk_gate);
	if (ret != 0) {
		dev_err(&pdev->dev, "I2C controller init failed\n");
		return ret;
	}
	/* find the IRQ for this unit (note, this relies on the init call to
	 * ensure no current IRQs pending
	 */

	i2c->irq = ret = platform_get_irq(pdev, 0);
	if (ret <= 0) {
		dev_err(&pdev->dev, "cannot find IRQ\n");
		return ret;
	}

	ret = devm_request_irq(&pdev->dev, i2c->irq, lombo_i2c_irq, 0,
			       dev_name(&pdev->dev), i2c);
	if (ret != 0) {
		dev_err(&pdev->dev, "cannot claim IRQ %d\n", i2c->irq);
		return ret;
	}

	i2c->adap.dev.of_node = pdev->dev.of_node;

	ret = i2c_add_adapter(&i2c->adap);
	if (ret < 0) {
		dev_err(&pdev->dev, "failed to add bus to i2c core\n");
		return ret;
	}

	of_i2c_register_devices(&i2c->adap);
	platform_set_drvdata(pdev, i2c);

	pm_runtime_enable(&pdev->dev);
	pm_runtime_enable(&i2c->adap.dev);

	dev_info(&pdev->dev, "%s: Lombo I2C adapter\n",
		dev_name(&i2c->adap.dev));
	return 0;
}

/* lombo_i2c_remove
 *
 * called when device is removed from the bus
*/

static int lombo_i2c_remove(struct platform_device *pdev)
{
	struct lombo_i2c *i2c = platform_get_drvdata(pdev);

	pm_runtime_disable(&i2c->adap.dev);
	pm_runtime_disable(&pdev->dev);

	i2c_del_adapter(&i2c->adap);

	clk_disable_unprepare(i2c->clk_gate);

	return 0;
}

#ifdef CONFIG_PM_SLEEP
static int  lombo_i2c_suspend_noirq(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct lombo_i2c *i2c = platform_get_drvdata(pdev);
	struct clk *clk_null;

	clk_null = clk_get(NULL, "null_clk");
	clk_set_parent(i2c->clk_i2c, clk_null);
	clk_put(clk_null);

	clk_disable_unprepare(i2c->clk_reset);
	i2c->suspended = 1;

	return 0;
}

static int  lombo_i2c_resume(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct lombo_i2c *i2c = platform_get_drvdata(pdev);

	clk_prepare_enable(i2c->clk_reset);
	clk_prepare_enable(i2c->clk_gate);
	lombo_i2c_init(i2c);
	clk_disable_unprepare(i2c->clk_gate);
	i2c->suspended = 0;

	return 0;
}
#endif

#ifdef CONFIG_PM
static const struct dev_pm_ops lombo_i2c_dev_pm_ops = {
#ifdef CONFIG_PM_SLEEP
	.suspend_noirq = lombo_i2c_suspend_noirq,
	.resume_early = lombo_i2c_resume,
#endif
};

#define LOMBO_DEV_PM_OPS (&lombo_i2c_dev_pm_ops)
#else
#define LOMBO_DEV_PM_OPS NULL
#endif

static struct platform_driver lombo_i2c_driver = {
	.probe		= lombo_i2c_probe,
	.remove		= lombo_i2c_remove,
	.driver		= {
		.owner	= THIS_MODULE,
		.name	= "lombo-i2c",
		.pm	= LOMBO_DEV_PM_OPS,
		.of_match_table = of_match_ptr(lombo_i2c_match),
	},
};

static int __init lombo_i2c_adap_init(void)
{
	return platform_driver_register(&lombo_i2c_driver);
}
subsys_initcall(lombo_i2c_adap_init);

static void __exit lombo_i2c_exit(void)
{
	platform_driver_unregister(&lombo_i2c_driver);
}
module_exit(lombo_i2c_exit);

MODULE_DESCRIPTION("LOMBO I2C Bus driver");
MODULE_AUTHOR("lomboswer <lomboswer@lombotech.com>");
MODULE_LICENSE("GPL");
