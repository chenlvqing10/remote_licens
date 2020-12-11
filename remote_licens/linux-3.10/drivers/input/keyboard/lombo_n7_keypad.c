/*
 * Lombo keypad driver
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

#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/pm.h>
#include <linux/pm_runtime.h>
#include <linux/suspend.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/sched.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/notifier.h>
#include <mach/csp.h>
#include <mach/debug.h>
#include <mach/common.h>

#define DEVICE_NAME	"gpadc-keypad"

/* register offset */
#define OFFSET_GPADC_VER		0x00000000
#define OFFSET_GPADC_GPAEN		0x00000010
#define OFFSET_GPADC_GPAC		0x00000014
#define OFFSET_GPADC_GPAIE		0x00000018
#define OFFSET_GPADC_GPAIS		0x0000001C
#define OFFSET_GPADC_GPAIC		0x00000020
#define OFFSET_GPADC_CDAT		0x00000024
#define OFFSET_GPADC_AIN0DAT		0x00000030
#define OFFSET_GPADC_AUX0HDAT		0x00000034
#define OFFSET_GPADC_AUX0TDAT		0x00000038
#define OFFSET_GPADC_AIN1DAT		0x00000040
#define OFFSET_GPADC_AUX1HDAT		0x00000044
#define OFFSET_GPADC_AUX1TDAT		0x00000048
#define OFFSET_GPADC_DEBUG0		0x000000F0
#define OFFSET_GPADC_DEBUG1		0x000000F4

/* macro define */
#define AUXIN0_T_VALUE		0xff0
#define AUXIN0_BIT		0x1
#define AUXIN1_BIT		0x2
#define KEY_1_L			0x15e
#define KEY_1_H			0x1c2
#define KEY_2_L			0x514
#define KEY_2_H			0x578
#define KEY_3_L			0x7d0
#define KEY_3_H			0x834
#define KEY_4_L			0x900
#define KEY_4_H			0x930
#define KEY_5_L			0x980
#define KEY_5_H			0x990

/* controller num */
#define CTL_NUM_0		0x0
#define CTL_NUM_1		0x1

/* gpadc vol */
#define RATE_VOL			0.8056640625
#define LIMIT_OFFSET		0.05
#define GPADC0_L			(2.2*(1-LIMIT_OFFSET)*1000)
#define GPADC0_R			(2.2*(1+LIMIT_OFFSET)*1000)
#define GPADC1_L			(1.1*(1-LIMIT_OFFSET)*1000)
#define GPADC1_R			(1.1*(1+LIMIT_OFFSET)*1000)
/* tell devices the system will enter standby */
#define LOMBO_PM_STANDBY	0x0007
/* tell devices the system will enter deepsleep */
#define LOMBO_PM_DEEPSLEEP	0x0008


/* global key mark */
static u32 g_key;

struct lombo_key_data {
	void __iomem		*base;
	struct clk		*clk_gate;
	struct clk		*clk_reset;
	struct clk		*clk_gpadc;
	struct clk		*clk_parent;
	int			irq;
	int			ctl_num;
	struct input_dev	*key_dev;
	struct work_struct	irq_work;
	bool			suspend_type;
};

struct lombo_key_data lo_key;

static void csp_gpadc_init(struct lombo_key_data *data)
{
	reg_gpadc_gpac_t gpac_reg;
	reg_gpadc_aux0tdat_t aux0tdat_reg;
	reg_gpadc_aux1tdat_t aux1tdat_reg;
	reg_gpadc_gpaie_t gpaie_reg;
	reg_gpadc_gpaen_t gpaen_reg;

	/* gpadc control register */
	gpac_reg.val = readl(data->base + OFFSET_GPADC_GPAC);
	gpac_reg.bits.fss = 0;
	gpac_reg.bits.src = 1;
	gpac_reg.bits.sdae = 1;
	writel(gpac_reg.val, data->base + OFFSET_GPADC_GPAC);

	/* enable interrupt */
	gpaie_reg.val = readl(data->base + OFFSET_GPADC_GPAIE);
	gpaie_reg.bits.aux0ie = 1;
	gpaie_reg.bits.aux1ie = 1;
	writel(gpaie_reg.val, data->base + OFFSET_GPADC_GPAIE);

	/* gpadc threshold data register */
	aux0tdat_reg.val = readl(data->base + OFFSET_GPADC_AUX0TDAT);
	aux0tdat_reg.bits.auxin0tdat = AUXIN0_T_VALUE;
	writel(aux0tdat_reg.val, data->base + OFFSET_GPADC_AUX0TDAT);

	aux1tdat_reg.val = readl(data->base + OFFSET_GPADC_AUX1TDAT);
	aux1tdat_reg.bits.auxin1tdat = AUXIN0_T_VALUE;
	writel(aux1tdat_reg.val, data->base + OFFSET_GPADC_AUX1TDAT);

	/* gpadc control register */
	gpac_reg.val = readl(data->base + OFFSET_GPADC_GPAC);
#if 0
	if (data->ctl_num == CTL_NUM_0)
		gpac_reg.bits.auxin0e = 1;
	else
		gpac_reg.bits.auxin1e = 1;
#else
	gpac_reg.bits.auxin0e = 1;
	gpac_reg.bits.auxin1e = 1;
#endif
	writel(gpac_reg.val, data->base + OFFSET_GPADC_GPAC);

	/* gpadc set default value */
	writel(0x01, data->base + OFFSET_GPADC_DEBUG0);
	writel(0x0018FF00, data->base + OFFSET_GPADC_DEBUG1);
	writel(0x00, data->base + OFFSET_GPADC_DEBUG0);

	/* start gpdac */
	gpaen_reg.val = readl(data->base + OFFSET_GPADC_GPAEN);
	gpaen_reg.bits.gen = 1;
	writel(gpaen_reg.val, data->base + OFFSET_GPADC_GPAEN);
}

u32 lombo_read_ainx(struct lombo_key_data *lombo_key, u32 n)
{
	reg_gpadc_ain0dat_t ain0_data;
	reg_gpadc_ain1dat_t ain1_data;
	u32 key_val = 0;

	if (n == 0) {
		ain0_data.val = readl(lombo_key->base +
					 OFFSET_GPADC_AIN0DAT);
		key_val = ain0_data.bits.dat;
	} else if (n == 1) {
		ain1_data.val = readl(lombo_key->base +
					 OFFSET_GPADC_AIN1DAT);
		key_val = ain1_data.bits.dat;
	}

	return key_val;
}

static int key_flags0, key_flags1;

static void lombo_keypad_irq_work(struct work_struct *work)
{
	struct lombo_key_data *lombo_key = container_of(work,
					struct lombo_key_data, irq_work);
	u32 key_val0 = 0;
	u32 key_val1 = 0;

	key_val0 = lombo_read_ainx(lombo_key, 0);
	key_val1 = lombo_read_ainx(lombo_key, 1);
	while (!(key_val0 > AUXIN0_T_VALUE) || !(key_val1 > AUXIN0_T_VALUE)) {
			usleep_range(10, 20);
			key_val0 = lombo_read_ainx(lombo_key, 0);
			key_val1 = lombo_read_ainx(lombo_key, 1);
		}

	if (key_flags0 != 0 && key_val0 > AUXIN0_T_VALUE) {
		input_report_key(lombo_key->key_dev, g_key, 0);
		input_sync(lombo_key->key_dev);
		key_flags0 = 0;
		PRT_WARN("adc0:key %d release\n", g_key);
	}

	if (key_flags1 != 0 && key_val1 > AUXIN0_T_VALUE) {
		input_report_key(lombo_key->key_dev, g_key, 0);
		input_sync(lombo_key->key_dev);
		key_flags1 = 0;
		PRT_WARN("adc1:key %d release\n", g_key);
	}

}

static irqreturn_t lombo_keypad_irq_handler(int irq, void *data)
{
	u32 reg_val;
	u32 key_val0, key_val1;
	struct lombo_key_data *lombo_key = data;
	reg_gpadc_gpaie_t gpaie_reg;
	reg_gpadc_gpais_t gpais_reg;
	reg_gpadc_aux0hdat_t aux0hdat_reg;
	reg_gpadc_aux1hdat_t aux1hdat_reg;

	gpaie_reg.val = readl(lombo_key->base + OFFSET_GPADC_GPAIE);
	gpais_reg.val = readl(lombo_key->base + OFFSET_GPADC_GPAIS);
	reg_val = gpaie_reg.val & gpais_reg.val;
	if (key_flags0 == 0 && reg_val==1) {
		aux0hdat_reg.val = readl(lombo_key->base +
				 OFFSET_GPADC_AUX0HDAT);
		key_val0 = aux0hdat_reg.bits.auxin0shdat;
		if ((key_val0 > KEY_1_L) && (key_val0 < KEY_1_H)) {
			g_key = BTN_0;
			input_report_key(lombo_key->key_dev, g_key, 1);
			input_sync(lombo_key->key_dev);
			key_flags0 = 1;
			PRT_WARN("adc0:key %d press\n", g_key);
		} else if ((key_val0 > KEY_2_L) && (key_val0 < KEY_2_H)) {
			g_key = BTN_1;
			input_report_key(lombo_key->key_dev, g_key, 1);
			input_sync(lombo_key->key_dev);
			key_flags0 = 1;
			PRT_WARN("adc0:key %d press\n", g_key);
		} else if ((key_val0 > KEY_3_L) && (key_val0 < KEY_3_H)) {
			g_key = BTN_2;
			input_report_key(lombo_key->key_dev, g_key, 1);
			input_sync(lombo_key->key_dev);
			key_flags0 = 1;
			PRT_WARN("adc0:key %d press\n", g_key);
		} else if ((key_val0 > KEY_4_L) && (key_val0 < KEY_4_H)) {
			g_key = BTN_5;
			input_report_key(lombo_key->key_dev, g_key, 1);
			input_sync(lombo_key->key_dev);
			key_flags0 = 1;
			PRT_WARN("adc0:key %d press\n", g_key);
		} /* else if (...) {...} */
	}

	if (key_flags1 == 0 && reg_val == 2) {
		aux1hdat_reg.val = readl(lombo_key->base +
				 OFFSET_GPADC_AUX1HDAT);
		key_val1 = aux1hdat_reg.bits.auxin1hdat;
		if ((key_val1 > KEY_1_L) && (key_val1 < KEY_1_H)) {
			g_key = BTN_3;
			input_report_key(lombo_key->key_dev, g_key, 1);
			input_sync(lombo_key->key_dev);
			key_flags1 = 1;
			PRT_WARN("key %d press\n", g_key);
		} else if ((key_val1 > KEY_2_L) && (key_val1 < KEY_2_H)) {
			g_key = BTN_7;
			input_report_key(lombo_key->key_dev, g_key, 1);
			input_sync(lombo_key->key_dev);
			key_flags1 = 1;
			PRT_WARN("key %d press\n", g_key);
		} /* else if (...) {...} */
	}

	writel(reg_val, lombo_key->base + OFFSET_GPADC_GPAIC);

	schedule_work(&lombo_key->irq_work);

	return IRQ_HANDLED;
}

int gpadc_open(struct inode *inode, struct file *filp)
{
	pr_debug("enter gpadc_open!\n");

	return 0;
}

int gpadc_read(struct file *filp,  char __user *buf,
	size_t count, loff_t *offset)
{
	u32 gpadc_cap_d0, gpadc_cap_d1;
	u32 adc0_vol, adc1_vol;
	u32 data;
	ssize_t ret;

	pr_debug("enter gpadc_read!\n");

	gpadc_cap_d0 = readl(lo_key.base + OFFSET_GPADC_AIN0DAT);
	adc0_vol = RATE_VOL * gpadc_cap_d0;
	pr_info("gpadc0 >> address:0x%p, value:0x%p, vol=%d\n",
		(void *)(lo_key.base + OFFSET_GPADC_AIN0DAT),
		(void *)gpadc_cap_d0, adc0_vol);

	gpadc_cap_d1 = readl(lo_key.base + OFFSET_GPADC_AIN1DAT);
	adc1_vol = RATE_VOL * gpadc_cap_d1;
	pr_info("gpadc1 >> address:0x%p, value:0x%p, vol=%d\n",
		(void *)(lo_key.base + OFFSET_GPADC_AIN1DAT),
		(void *)gpadc_cap_d1, adc1_vol);

	if (adc0_vol < GPADC0_L || adc0_vol > GPADC0_R) {
		pr_info("gpadc0 test failed!\n");
		data = 2;
	} else if (adc1_vol < GPADC1_L || adc1_vol > GPADC1_R) {
		pr_info("gpadc1 test failed!\n");
		data = 3;
	} else {
		pr_info("gpadc test pass!\n");
		data = 1;
	}

	ret = put_user(data, (unsigned int __user *)buf) ?:
			sizeof(unsigned int);
	return ret;
}

static int gpadc_release(struct inode *inode, struct file *file)
{
	pr_debug("enter gpadc_release!\n");

	return 0;
}

static const struct file_operations gpadc_fops = {
	.owner = THIS_MODULE,
	.open = gpadc_open,
	.read = gpadc_read,
	.release = gpadc_release,
};

static struct miscdevice gpadc_dev = {
	.minor  = MISC_DYNAMIC_MINOR,
	.name   = DEVICE_NAME,
	.fops	= &gpadc_fops
};

static int lombo_notifier_call_key(struct notifier_block *nb,
			unsigned long action, void *data)
{
	if (action == LOMBO_PM_DEEPSLEEP)
		lo_key.suspend_type = true;
	else if (action == LOMBO_PM_STANDBY)
		lo_key.suspend_type = false;

	return 0;
}

struct notifier_block pm_key_notify = {
	.notifier_call = lombo_notifier_call_key,
};

static int lombo_keypad_probe(struct platform_device *pdev)
{
	int ret;
	int num;
	struct resource *res;
	struct device_node *np = pdev->dev.of_node;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	lo_key.base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(lo_key.base)) {
		dev_err(&pdev->dev, "err ioremap invalid\n");
		return PTR_ERR(lo_key.base);
	}

	/* clk resource */
	lo_key.clk_gate = devm_clk_get(&pdev->dev, "apb_gpadc_gate");
	lo_key.clk_reset = devm_clk_get(&pdev->dev, "apb_gpadc_reset");
	lo_key.clk_gpadc = devm_clk_get(&pdev->dev, "gpadc_clk");
	lo_key.clk_parent = devm_clk_get(&pdev->dev, "osc24m");
	if (IS_ERR(lo_key.clk_gate) || IS_ERR(lo_key.clk_reset)
		|| IS_ERR(lo_key.clk_gpadc) || IS_ERR(lo_key.clk_parent)) {
		dev_err(&pdev->dev, "get adc key gate/reset/module clk failed\n");
		return -EINVAL;
	}

	ret = clk_set_parent(lo_key.clk_gpadc, lo_key.clk_parent);
	if (ret) {
		dev_err(&pdev->dev, "set parent failed\n");
		return -EINVAL;
	}

	ret = clk_prepare_enable(lo_key.clk_reset);
	if (ret) {
		dev_err(&pdev->dev, "enable adc key reset assert failed\n");
		return ret;
	}

	ret = clk_prepare_enable(lo_key.clk_gate);
	if (ret) {
		clk_disable_unprepare(lo_key.clk_reset);
		dev_err(&pdev->dev, "enable adc key gate clk failed\n");
		return ret;
	}

	ret = clk_prepare_enable(lo_key.clk_gpadc);
	if (ret) {
		clk_disable_unprepare(lo_key.clk_gate);
		clk_disable_unprepare(lo_key.clk_reset);
		dev_err(&pdev->dev, "enable adc key clk failed\n");
		return ret;
	}

	lo_key.irq = platform_get_irq(pdev, 0);
	if (lo_key.irq < 0) {
		dev_err(&pdev->dev, "err missing IRQ\n");
		goto err;
	}

	/* get controller num */
	if (of_property_read_u32(np, "controller_num", &num)) {
		dev_warn(&pdev->dev, "no controller num proper, use default\n");
		lo_key.ctl_num = 0;
	} else
		lo_key.ctl_num = num;

	/* init gpadc */
	csp_gpadc_init(&lo_key);

	/* alloc input device */
	lo_key.key_dev = input_allocate_device();
	if (!lo_key.key_dev) {
		dev_err(&pdev->dev, "input allocate device error\n");
		goto err;
	}

	lo_key.key_dev->name = "lombo-key";
	lo_key.key_dev->evbit[0] = BIT_MASK(EV_SYN) | BIT_MASK(EV_KEY);

	lo_key.key_dev->keybit[BIT_WORD(BTN_0)] |= BIT_MASK(BTN_0);
	lo_key.key_dev->keybit[BIT_WORD(BTN_1)] |= BIT_MASK(BTN_1);
	lo_key.key_dev->keybit[BIT_WORD(BTN_2)] |= BIT_MASK(BTN_2);
	lo_key.key_dev->keybit[BIT_WORD(BTN_3)] |= BIT_MASK(BTN_3);
	lo_key.key_dev->keybit[BIT_WORD(BTN_4)] |= BIT_MASK(BTN_4);

	ret = input_register_device(lo_key.key_dev);
	if (ret) {
		dev_err(&pdev->dev, "input device register error\n");
		goto err;
	}

	INIT_WORK(&lo_key.irq_work, lombo_keypad_irq_work);

	ret = devm_request_irq(&pdev->dev, lo_key.irq,
				lombo_keypad_irq_handler,
				IRQF_DISABLED | IRQF_NO_SUSPEND,
				"lombo-keypad", &lo_key);
	if (ret)
		goto err;

	dev_info(&pdev->dev, "controller %d %d\n", lo_key.ctl_num, ret);
	misc_register(&gpadc_dev);

	register_pm_notifier(&pm_key_notify);

	return 0;

err:
	dev_err(&pdev->dev, "lombo_keypad_probe end with error\n");
	clk_disable_unprepare(lo_key.clk_gpadc);
	clk_disable_unprepare(lo_key.clk_reset);
	clk_disable_unprepare(lo_key.clk_gate);
	return -EINVAL;
}

static int lombo_keypad_remove(struct platform_device *pdev)
{
	misc_deregister(&gpadc_dev);
	return 0;
}

#ifdef CONFIG_PM
static int lombo_keypad_suspend(struct device *dev)
{
	struct clk *null_clk, *rtc_clk;
	reg_gpadc_gpac_t gpac_reg;

	null_clk = clk_get(NULL, "null_clk");
	rtc_clk = clk_get(NULL, "osc32k");
	if (lo_key.suspend_type) {
		clk_set_parent(lo_key.clk_gpadc, null_clk);
		clk_disable_unprepare(lo_key.clk_gpadc);
		clk_disable_unprepare(lo_key.clk_reset);
		clk_disable_unprepare(lo_key.clk_gate);
	} else {
		gpac_reg.val = readl(lo_key.base + OFFSET_GPADC_GPAC);
		gpac_reg.bits.src = 0;
		gpac_reg.bits.sdae = 0;
		writel(gpac_reg.val, lo_key.base + OFFSET_GPADC_GPAC);
		clk_set_parent(lo_key.clk_gpadc, rtc_clk);
	}

	return 0;
}

static int lombo_keypad_resume(struct device *dev)
{
	reg_gpadc_gpac_t gpac_reg;

	clk_set_parent(lo_key.clk_gpadc, lo_key.clk_parent);
	if (lo_key.suspend_type) {
		clk_prepare_enable(lo_key.clk_reset);
		clk_prepare_enable(lo_key.clk_gate);
		clk_prepare_enable(lo_key.clk_gpadc);
		csp_gpadc_init(&lo_key);
	} else {
		gpac_reg.val = readl(lo_key.base + OFFSET_GPADC_GPAC);
		gpac_reg.bits.src = 1;
		gpac_reg.bits.sdae = 1;
		writel(gpac_reg.val, lo_key.base + OFFSET_GPADC_GPAC);
	}

	return 0;
}

static const struct dev_pm_ops lombo_key_pm_ops = {
	.suspend = lombo_keypad_suspend,
	.resume = lombo_keypad_resume,
};
#endif

#ifdef CONFIG_OF
static const struct of_device_id lombo_keypad_dt_match[] = {
	{ .compatible = "lombo,n7-adc-keypad" },
	{},
};
MODULE_DEVICE_TABLE(of, lombo_keypad_dt_match);
#endif

static struct platform_driver lombo_keypad_driver = {
	.probe		= lombo_keypad_probe,
	.remove		= lombo_keypad_remove,
	.driver		= {
		.name	= "lombo-keypad",
		.owner	= THIS_MODULE,
		#ifdef CONFIG_OF
		.of_match_table = of_match_ptr(lombo_keypad_dt_match),
		#endif
		#ifdef CONFIG_PM
		.pm = &lombo_key_pm_ops,
		#endif
	},
};
module_platform_driver(lombo_keypad_driver);

MODULE_DESCRIPTION("LomboTech keypad driver");
MODULE_AUTHOR("lomboswer@lombotech.com");
MODULE_AUTHOR("lomboswer@lombotech.com");
MODULE_LICENSE("GPL");
