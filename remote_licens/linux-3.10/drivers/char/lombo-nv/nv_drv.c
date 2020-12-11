/*
 *
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Authors:
 *	lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#include <linux/time.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/wait.h>
#include <linux/poll.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/clk.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/io.h>
#include <linux/uaccess.h>
#include <mach/csp.h>
#include "nv_drv.h"
#include "cv_reg.h"
#include "nn_reg.h"

#define DEVICE_NAME		"lombo-nv"
#define MAJOR_NN		256
#define MINOR_NN		100

#ifndef NV_NAME
#define NV_NAME			"nv"
#endif

#define NV_ERR(fmt, ...)				\
	pr_err("[" NV_NAME "] " fmt, ##__VA_ARGS__)
#define NV_WRN(fmt, ...)				\
	pr_warn("[" NV_NAME "] " fmt, ##__VA_ARGS__)
#define NV_INFO(fmt, ...)				\
	pr_info("[" NV_NAME "] " fmt, ##__VA_ARGS__)

#define NV_CLK_RATE		360000000

struct clk	*nv_ahb_gate;
struct clk	*nv_ahb_reset;
struct clk	*nv_axi_gate;
struct clk	*nv_module_clk;
struct clk	*pll_clk;

static int nv_irq_registered;
static void __iomem *nv_regs_base;
static unsigned int nv_irq_id;

static unsigned int cv_irq_flag;
static unsigned int nn_irq_flag;

static int cv_status = CV_IDLE;
static int nn_status = NN_IDLE;

static int clk_status;
static spinlock_t nv_spin_lock;

static DECLARE_WAIT_QUEUE_HEAD(wait_cv);
static DECLARE_WAIT_QUEUE_HEAD(wait_nn);

static struct of_device_id lombo_nv_match[] = {
	{ .compatible = "lombo,n7-ax", },
	{ },
};
MODULE_DEVICE_TABLE(of, lombo_nv_match);

static int nv_dev_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static int nv_dev_close(struct inode *node, struct file *filp)
{
	return 0;
}

static void enable_cv_irq(void)
{
	writel(0x32c80031, nv_regs_base + (CV_IRQ_EN));
}

static void disable_cv_irq(void)
{
	writel(0x00000000, nv_regs_base + (CV_IRQ_EN));
}

static void cv_reset(void)
{
	writel(0x80000000, nv_regs_base + CV_CTL);
}

static void enable_nn_irq(void)
{
	writel(0xffff0001, nv_regs_base + (NN_IRQ_EN));
}

static void disable_nn_irq(void)
{
	writel(0x00000000, nv_regs_base + (NN_IRQ_EN));
}

static void nn_reset(void)
{
	u32 rdval;
	u32 *reg_addr;

	/* AHB BUS Reset/ax Reset */
	reg_addr = (u32 *)0xf400A224;
	rdval = READREG32(reg_addr);
	NV_INFO("[%d] 0xf400A224 rdval:0x%x\n", __LINE__, rdval);

	WRITEREG32(reg_addr, (rdval & 0xffffdfff));
	udelay(1);
	WRITEREG32(reg_addr, (rdval | (1<<13)));
	rdval = READREG32(reg_addr);
	NV_INFO("[%d] 0xf400A224 rdval:0x%x\n", __LINE__, rdval);

	writel(0x80000000, nv_regs_base + NN_CTL);
	writel(0x00000000, nv_regs_base + NN_CTL);
}

static int nn_query_status(void)
{
	int cur_status;

	cur_status = nn_status;

	return cur_status;
}

static int cv_query_status(void)
{
	int ret_val;
	int cur_status;

	ret_val = readl(nv_regs_base + (CV_IRQ_PEND));
	if ((ret_val & 0x01) == 0x01) {
		NV_INFO("cv finish\n");
		writel(ret_val, nv_regs_base + CV_IRQ_CLEAR);
		cv_status = CV_FINISH;
	} else if ((ret_val & 0x20) == 0x20) {
		NV_INFO("cv time out\n");
		writel(ret_val, nv_regs_base + CV_IRQ_CLEAR);
		cv_status = CV_TIMEOUT;
	} else {
	}

	cur_status = cv_status;

	return cur_status;
}

static irqreturn_t nv_interrupt(unsigned long data)
{
	int cv_irq_val;
	int nn_irq_val;

	int cv_end = 1;
	int nn_end = 1;

	disable_nn_irq();

	cv_irq_val = readl(nv_regs_base + (CV_IRQ_PEND));
	nn_irq_val = readl(nv_regs_base + (NN_IRQ_PEND));

	if ((cv_irq_val & 0x01) == 0x01) {
		/* NV_INFO("cv finish\n"); */
		writel(cv_irq_val, nv_regs_base + CV_IRQ_CLEAR);

		cv_irq_flag = 1;
		cv_status = CV_FINISH;

		wake_up(&wait_cv);
	} else if ((cv_irq_val & 0x20) == 0x20) {
		/* NV_WRN("cv time out\n"); */
		writel(cv_irq_val, nv_regs_base + CV_IRQ_CLEAR);

		cv_irq_flag = 1;
		cv_status = CV_TIMEOUT;

		wake_up(&wait_cv);
	} else {
		cv_end = 0;
	}

	if ((nn_irq_val & 0x01) == 0x01) {
		/* NV_INFO("nn finish\n"); */
		writel(nn_irq_val, nv_regs_base + NN_IRQ_CLEAR);

		nn_irq_flag = 1;
		nn_status = NN_FINISH;

		wake_up(&wait_nn);
	} else if ((nn_irq_val & 0x20) == 0x20) {
		/* NV_WRN("nn time out\n"); */
		writel(nn_irq_val, nv_regs_base + NN_IRQ_CLEAR);

		nn_irq_flag = 1;
		nn_status = NN_TIMEOUT;

		wake_up(&wait_nn);
	} else {
		nn_end = 0;
	}

	if ((cv_end + nn_end) == 0)
		NV_ERR("finish error %x,%x\n", cv_irq_val, nn_irq_val);

	return IRQ_HANDLED;
}

static int enable_nv_clk(void)
{
	unsigned long flags;
	int res = -EFAULT;

	spin_lock_irqsave(&nv_spin_lock, flags);

	if (clk_status == 0) {
		res = 0;
		goto out;
	}

	clk_status = 0;

	if ((NULL == nv_module_clk) || (IS_ERR(nv_module_clk)))
		NV_ERR("nv_module_clk is invalid\n");
	else
		clk_prepare_enable(nv_module_clk);

out:
	spin_unlock_irqrestore(&nv_spin_lock, flags);
	return 0;
}

static int disable_nv_clk(void)
{
	unsigned long flags;
	int res = -EFAULT;

	spin_lock_irqsave(&nv_spin_lock, flags);

	if (clk_status == 1) {
		res = 1;
		goto out;
	}

	clk_status = 1;

	if ((NULL == nv_module_clk) || (IS_ERR(nv_module_clk)))
		NV_ERR("nv_module_clk is invalid\n");
	else
		clk_disable_unprepare(nv_module_clk);

out:
	spin_unlock_irqrestore(&nv_spin_lock, flags);
	return 0;
}

static long nv_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	unsigned long flags;
	rsz_cfg_t cv_param;
	nn_param_t nn_param;
	int cur_status = 0;
	int ret = 0;
	int left_time = 0;
	nv_register reg;
	unsigned int rdval;

	switch (cmd) {
	case CV_RUN: {
		spin_lock_irqsave(&nv_spin_lock, flags);

		cur_status = cv_query_status();
		if ((readl(nv_regs_base + (CV_IRQ_EN)) & 0x00000031) &&
		    (cur_status == CV_FINISH)) {
			disable_cv_irq();
			spin_unlock_irqrestore(&nv_spin_lock, flags);
			return CV_CHECK_RESULT;
		} else if ((readl(nv_regs_base + (CV_IRQ_EN)) & 0x00000031) &&
			 (cur_status == CV_TIMEOUT || cur_status == CV_ERR)) {
			disable_cv_irq();
		} else if (cur_status == CV_BUSY) {
			spin_unlock_irqrestore(&nv_spin_lock, flags);

			return CV_BUSY;
		}

		if (cur_status == CV_TIMEOUT || cur_status == CV_ERR) {
			cv_reset();
			NV_WRN("cv reset\n");
		}

		ret = copy_from_user(&cv_param,
				     (void __user *)arg, sizeof(rsz_cfg_t));

		cv_register_set(nv_regs_base, cv_param);
		cv_status = CV_BUSY;

		spin_unlock_irqrestore(&nv_spin_lock, flags);

		return 0;
	}

	case NN_RUN: {
		/* NV_WRN("NN_RUN\n"); */
		spin_lock_irqsave(&nv_spin_lock, flags);

		cur_status = nn_query_status();
		if ((readl(nv_regs_base + (NN_IRQ_EN)) & 0x000000f1) &&
		    (cur_status == NN_FINISH)) {
			NV_WRN("1a, %08x\n",
				readl(nv_regs_base + (NN_IRQ_EN)));

			spin_unlock_irqrestore(&nv_spin_lock, flags);

			return NN_CHECK_RESULT;
		} else if ((readl(nv_regs_base + (NN_IRQ_EN)) & 0x000000f1) &&
			   (cur_status == NN_TIMEOUT || cur_status == NN_ERR)) {
			NV_WRN("2a\n");
		} else if (cur_status == NN_BUSY) {
			NV_WRN("3a\n");
			spin_unlock_irqrestore(&nv_spin_lock, flags);
			return NN_BUSY;
		}

		if (cur_status == NN_TIMEOUT || cur_status == NN_ERR) {
			NV_WRN("4a\n");
			nn_reset();
			NV_WRN("nn reset!\n");
		}

		/* printk("%s %d\n", __func__, __LINE__); */
		ret = copy_from_user(&nn_param,
				     (void __user *)arg, sizeof(nn_param_t));

		if (nv_irq_registered == 1)
			enable_nn_irq();

		nn_set_reg(nv_regs_base, nn_param);
		nn_status = NN_BUSY;

		spin_unlock_irqrestore(&nv_spin_lock, flags);

		return 0;
	}

	case GET_CV_STATUS: {
		/* spin_lock_irqsave(&nv_spin_lock, flags); */
		cur_status = cv_query_status();
		if ((cur_status != CV_BUSY)) {
			disable_cv_irq();

			/* spin_unlock_irqrestore(&nv_spin_lock, flags); */
			return cur_status;
		}

		if (nv_irq_registered == 1) {
			enable_cv_irq();
			left_time = wait_event_timeout(wait_cv,
						       cv_irq_flag,
						       300);
			cv_irq_flag = 0;
			if (unlikely(left_time) == 0) {
				cv_status = CV_TIMEOUT;
				disable_cv_irq();

				NV_WRN("cv irq timeout!\n");

				return CV_FINISH;
			} else {
				cv_status = CV_FINISH;
				disable_cv_irq();

				return CV_FINISH;
			}
		} else {
			int time_counts = 10000;
			do {
				cur_status = cv_query_status();
				if (cur_status != CV_BUSY) {
					disable_cv_irq();
					return cur_status;
				}

				if (time_counts-- < 0) {
					cv_status = CV_TIMEOUT;
					disable_cv_irq();

					return CV_TIMEOUT;
				}
			} while (1);
		}
	}

	case GET_NN_STATUS: {
		/* NV_WRN("GET_NN_STATUS!\n"); */

		cur_status = nn_query_status();
		if ((cur_status != NN_BUSY)) {
			rdval = readl((u32 *)(0xf1210000 + 0x3fc));
			if (rdval & 0x00040004)
				NV_WRN("s1\n");

			nn_irq_flag = 0;

			return cur_status;
		}

		if (nv_irq_registered == 1) {
			left_time = wait_event_timeout(wait_nn,
						       nn_irq_flag,
						       300);

			nn_irq_flag = 0;
			if (unlikely(left_time == 0)) {
				NV_WRN("nn wait_event timeout!\n");

				nn_status = NN_FINISH;

				nn_reset();

				return NN_FINISH;
			} else {
				nn_status = NN_FINISH;

				rdval = readl((u32 *)(0xf1210000 + 0x3fc));
				if (rdval & 0x00040004)
					NV_WRN("s2\n");

				return NN_FINISH;
			}
		} else {
			int time_counts = 100000 * 1000;

			do {
				cur_status = nn_query_status();
				if (cur_status != NN_BUSY)
					return cur_status;

				if (time_counts-- < 0) {
					nn_status = NN_TIMEOUT;
					NV_WRN("s3\n");
					return NN_TIMEOUT;
				}
			} while (1);
		}
	}

	case NV_ENABLE_CLK: {
		NV_INFO("nv enable clk!\n");
		enable_nv_clk();

		return 0;
	}

	case NV_DISABLE_CLK: {
		NV_INFO("nv disable clk!\n");
		disable_nv_clk();

		return 0;
	}

	case NV_WRITE_REGISTER: {
		spin_lock_irqsave(&nv_spin_lock, flags);
		if (copy_from_user(&reg,
				   (void __user *)arg,
				   sizeof(nv_register))) {
				NV_ERR("nv from user err!\n");
				return -EFAULT;
		}

		writel(reg.value, nv_regs_base + reg.addr);
		NV_INFO("nv write reg addr 0x%x = 0x%x\n",
					(u32)(nv_regs_base + reg.addr),
					(u32)reg.value);

		spin_unlock_irqrestore(&nv_spin_lock, flags);
		return 0;
	}

	case NV_READ_REGISTER: {
		unsigned int rdata = 0x5a5a5a5a;

		spin_lock_irqsave(&nv_spin_lock, flags);
		if (copy_from_user(&reg,
				   (void __user *)arg,
				   sizeof(nv_register))) {
			NV_ERR("nv from user err!\n");
			return -EFAULT;
		}

		rdata = readl(nv_regs_base + reg.addr);
		NV_INFO("nv read register 0x%x = 0x%x\n",
					(u32)(nv_regs_base + reg.addr),
					(u32)rdata);

		spin_unlock_irqrestore(&nv_spin_lock, flags);

		return rdata;
	}

	default: {
		NV_WRN("[SYS] do not supprot this ioctrl now!\n");
		return ERR_CMD;
	}
	}
}

static const struct file_operations nv_fops = {
	.owner          = THIS_MODULE,
	.unlocked_ioctl = nv_ioctl,
	.open           = nv_dev_open,
	.release        = nv_dev_close
};

static struct miscdevice nv_dev = {
	/* .minor  = MISC_DYNAMIC_MINOR, */
	.minor  = 100,
	.name   = DEVICE_NAME,
	.fops   = &nv_fops
};

static int nv_probe(struct platform_device *pdev)
{
	struct device_node *node;
	int ret;
	u32 rdval;
	u32 *reg_addr;
	/* struct resource	*res = NULL; */
	node = of_find_compatible_node(NULL, NULL, "lombo,n7-ax");

	nv_irq_id = irq_of_parse_and_map(node, 0);
	NV_WRN("irq_id = %d\n", nv_irq_id);

	ret = request_irq(nv_irq_id,
			  (irq_handler_t)nv_interrupt,
			  0,
			  DEVICE_NAME,
			  NULL);
	if (ret < 0) {
		NV_ERR("Request nv Irq error! return %d\n", ret);
		return -EINVAL;
	} else {
		nv_irq_registered = 1;
		NV_INFO("Request nv Irq success!\n");
		NV_INFO("irq_id = %d, nv_irq_registered = %d\n",
			 nv_irq_id, nv_irq_registered);
	}

	spin_lock_init(&nv_spin_lock);

#if 1
	nv_regs_base = (u32 *)0xf1700000;
	NV_INFO("nv base addr 0x%p\n", nv_regs_base);
#endif

#if 0
	/* get & enable ahb, axi gate */
	nv_ahb_gate = of_clk_get(node, 0);
	if (IS_ERR(nv_ahb_gate)) {
		NV_ERR("get nv ahb gate err\n");
		return -EINVAL;
	}

	nv_ahb_reset = of_clk_get(node, 1);
	if (IS_ERR(nv_ahb_reset)) {
		NV_ERR("get nv ahb reset err\n");
		return -EINVAL;
	}

	nv_axi_gate = of_clk_get(node, 2);
	if (IS_ERR(nv_axi_gate)) {
		NV_ERR("get nv_axi_gate err\n");
		return -EINVAL;
	}

	clk_prepare_enable(nv_ahb_gate);
	clk_prepare_enable(nv_ahb_reset);
	clk_prepare_enable(nv_axi_gate);

	/* get the nv clk, set parent clk & clk rate */
	nv_module_clk = of_clk_get(node, 3);
	if (IS_ERR(nv_module_clk)) {
		NV_ERR("failed to get nv clk  !\n");
		return -EINVAL;
	}

	ret = clk_prepare_enable(nv_module_clk);
	if (ret) {
		NV_ERR("enable nv clk error\n");
		return -EINVAL;
	}

	pll_clk = clk_get(NULL, "perh0_pll_vco");
	if (IS_ERR(pll_clk)) {
		NV_ERR("failed to get nv pll\n");
		return -EINVAL;
	}

	ret = clk_set_parent(nv_module_clk, pll_clk);
	if (ret) {
		NV_ERR("set parent clk error\n");
		return -EINVAL;
	}

	ret = clk_set_rate(nv_module_clk, NV_CLK_RATE);
	if (ret) {
		NV_ERR("set nv clk rate err\n");
		return -EINVAL;
	}
#endif

/* enable ax pll */
#if 1
	/* ahb bus enable gating */
	reg_addr = (u32 *)0xf400A204;
	rdval = READREG32(reg_addr);
	NV_INFO("[%d] 0xf400A204 rdval:0x%x\n", __LINE__, rdval);
	WRITEREG32(reg_addr, rdval | (1<<13));
	rdval = READREG32(reg_addr);
	NV_INFO("[%d] 0xf400A204 rdval:0x%x\n", __LINE__, rdval);

	/* AHB BUS Reset/ax Reset */
	reg_addr = (u32 *)0xf400A224;
	rdval = READREG32(reg_addr);
	NV_INFO("[%d] 0xf400A224 rdval:0x%x\n", __LINE__, rdval);
	WRITEREG32(reg_addr, (rdval & 0xffffdfff));
	udelay(1);
	WRITEREG32(reg_addr, (rdval | (1<<13)));
	rdval = READREG32(reg_addr);
	NV_INFO("[%d] 0xf400A224 rdval:0x%x\n", __LINE__, rdval);

	/* AXI BUS CLOCK GATING */
	reg_addr = (u32 *)0xf400A340;
	rdval = READREG32(reg_addr);
	NV_INFO("[%d] 0xf400A340 rdval:0x%x\n", __LINE__, rdval);
	WRITEREG32(reg_addr, (rdval | (1<<12)));
	rdval = READREG32(reg_addr);
	NV_INFO("[%d] 0xf400A340 rdval:0x%x\n", __LINE__, rdval);

#if 1
	/* for adpll&prcm test */
	/* enable tp0pll fac  ax clk src 720M */
	reg_addr = (u32 *)0xf400a764;
	rdval = READREG32(reg_addr);
	NV_INFO("%s [%d] 0xf400a764 rdval:0x%x\n", __func__, __LINE__, rdval);
	rdval = 0x00101101;   /* 16 x 24M(base clk) = 384M */
	WRITEREG32(reg_addr, rdval);
	rdval = READREG32(reg_addr);
	NV_INFO("%s [%d] 0xf400a764 rdval:0x%x\n", __func__, __LINE__, rdval);

	/* ax power enable tppll ldo */
	reg_addr = (u32 *)0xf1204088;
	rdval = READREG32(reg_addr);
	NV_INFO("%s [%d] 0xf400a764 rdval:0x%x\n", __func__, __LINE__, rdval);
	rdval = 0x82;
	WRITEREG32(reg_addr, rdval);
	rdval = READREG32(reg_addr);
	NV_INFO("%s [%d] 0xf400a764 rdval:0x%x\n", __func__, __LINE__, rdval);

	/* enable tp0pll */
	reg_addr = (u32 *)0xf400a760;
	rdval = READREG32(reg_addr);
	NV_INFO("%s [%d] 0xf400a760 rdval:0x%x\n", __func__, __LINE__, rdval);
	WRITEREG32(reg_addr, 0);
	rdval = READREG32(reg_addr);
	NV_INFO("%s [%d] 0xf400a760 rdval:0x%x\n", __func__, __LINE__, rdval);

	/*
	 * 384M ax clk,
	 * ax module freq division processing (0<<4, current is 0)
	 */
	reg_addr = (u32 *)0xf400abd0;
	rdval = 0;
	WRITEREG32(reg_addr, rdval | (1<<0) | (2<<4) | (1<<31)); /* 400M */
	rdval = READREG32(reg_addr);
	NV_INFO("%s [%d] 0xf400abd0 rdval:0x%x\n", __func__, __LINE__, rdval);
#endif

#endif

	ret = misc_register(&nv_dev);
	if (ret >= 0)
		NV_INFO("nv device register success! %d\n", ret);
	else {
		NV_ERR("nv device register failed! %d\n", ret);
		return -EINVAL;
	}

	return 0;
}

static int nv_suspend(struct platform_device *pdev, pm_message_t state)
{
	int ret = 0;
	int times = 0;

	while (times < 30) {
		if (cv_status != CV_BUSY || nn_status != NN_BUSY)
			break;
		else {
			times--;
			mdelay(1);
		}
	}

	ret = disable_nv_clk();

	if (ret < 0) {
		NV_ERR("nv clk disable somewhere error!\n");
		return -EFAULT;
	}

	NV_INFO("[NV] standby suspend finish!\n");

	return 0;
}

static int nv_resume(struct platform_device *pdev)
{
	int ret = 0;

	ret = enable_nv_clk();
	if (ret < 0) {
		NV_ERR("[NV] nv clk enable somewhere error!\n");
		return -EFAULT;
	}

	NV_INFO("[NV] standby resume finish!\n");

	return 0;
}

static int nv_remove(struct platform_device *pdev)
{
	if (NULL == nv_ahb_gate || IS_ERR(nv_ahb_gate)) {
		NV_ERR("[NV] nv_ahb_gate handle is invalid,just return!\n");
	} else {
		clk_put(nv_ahb_gate);
		nv_ahb_gate = NULL;
	}

	if (NULL == nv_ahb_reset || IS_ERR(nv_ahb_reset)) {
		NV_ERR("[NV] nv_ahb_reset handle is invalid,just return!\n");
	} else {
		clk_put(nv_ahb_reset);
		nv_ahb_reset = NULL;
	}

	if (NULL == nv_axi_gate || IS_ERR(nv_axi_gate)) {
		NV_WRN("[NV] nv_axi_gate handle is invalid,just return!\n");
	} else {
		clk_put(nv_axi_gate);
		nv_axi_gate = NULL;
	}

	if (NULL == nv_module_clk || IS_ERR(nv_module_clk)) {
		NV_WRN("[NV] nv_moduleclk handle is invalid,just return!\n");
	} else {
		clk_put(nv_module_clk);
		nv_module_clk = NULL;
	}

	if (NULL == pll_clk || IS_ERR(pll_clk)) {
		NV_WRN("[NV] nv pll_clk handle is invalid,just return!\n");
	} else {
		clk_put(pll_clk);
		pll_clk = NULL;
	}

	free_irq(nv_irq_id, NULL);
	iounmap(nv_regs_base);

	misc_deregister(&nv_dev);
	NV_INFO("nv device has been removed!\n");

	return 0;
}

static struct platform_driver nv_driver = {
	.probe		= nv_probe,
	.remove		= nv_remove,
	.suspend	= nv_suspend,
	.resume		= nv_resume,

	.driver		= {
		.name	= "lombo-nv",
		.owner	= THIS_MODULE,
		.of_match_table = lombo_nv_match,
	},

};

static int __init nv_init(void)
{
	return platform_driver_register(&nv_driver);
}

static void __exit nv_exit(void)
{
	platform_driver_unregister(&nv_driver);
};

module_init(nv_init);
module_exit(nv_exit);
MODULE_LICENSE("GPL");


