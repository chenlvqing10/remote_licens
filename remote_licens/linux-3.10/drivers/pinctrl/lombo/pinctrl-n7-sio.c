/*
 * n7 sio pin support for Lombo pinctrl driver
 *
 * Copyright(c)2018 Lombo Tech Co.Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This file contains Lombo n7 specific static information to support
 * pinctrl driver, also includes the implementation of gpio interrupt support.
 */
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/kernel.h>
#include <linux/irq.h>
#include <linux/of_irq.h>
#include <linux/irqdomain.h>
#include <linux/irqchip/chained_irq.h>
#include <linux/io.h>
#include <linux/of_platform.h>
#include <linux/syscore_ops.h>
#include "pinctrl-lombo.h"

#define SIO_INT_CFG	0x40
#define SIO_INT_MASK	0x50
#define SIO_INT_PEND	0x54
#define SIO_INT_SAMP	0x58

#define GPIO_SIO_NAME	"sio"

#define EINT_SAMP_RTCCLK	0x0
#define EINT_SAMP_HFEOSC	0x1

struct platform_device *n7_sio_dev;

/**
 * n7 sio bank type define, contain five registers type.
 * Function configuration: 4-bits for per pin
 * Pull up or pull down configuration: 2-bits for per pin
 * Driver configuration: 3-bits for per pin
 * Data: 1-bit for per pin
 * Resistors configuration: 1-bit for per pin
 */
static const struct lombo_pin_bank_type n7_sio_bank_type = {
	.fld_width = {4, 2, 3, 1, 1,},
	.reg_offset = {0x0, 0x10, 0x20, 0x30, 0x34,},
};

#define PIN_BANK(pins, reg, id)		\
	{						\
		.type		= &n7_sio_bank_type,	\
		.pctl_offset	= reg,			\
		.npins		= pins,			\
		.eint_type	= EINT_TYPE_NONE,	\
		.name		= id			\
	}

#define PIN_BANK_GPIO(pins, reg, id, emask)		\
	{						\
		.type		= &n7_sio_bank_type,	\
		.pctl_offset	= reg,			\
		.npins		= pins,			\
		.eint_type	= EINT_TYPE_GPIO,	\
		.name		= id,			\
		.eint_mask	= emask			\
	}

/* n8 pin controller pin banks */
static const struct lombo_pin_bank_data n7_sio_pin_banks[] = {
	PIN_BANK_GPIO(8, 0x0, "sio", 0xFF),
};

/* config bank interrupt sample clock */
static inline void n7_sio_gpio_irq_set_samp(struct lombo_pin_bank *bank,
					    int div, int source)
{
	struct lombo_pinctrl_drv_data *d = bank->pctldrv;
	void __iomem *reg;
	u32 val;

	if (!strcmp(bank->name, GPIO_SIO_NAME))
		reg = d->regs_base + SIO_INT_SAMP;
	else {
		dev_err(d->dev, "%s not interrupt bank\n", __func__);
		return;
	}

	val = readl(reg);
	val |= 1 << 31;
	val |= div << 4;
	val |= source;
	writel(val, reg);
}

static inline void n7_sio_gpio_irq_set_mask(struct irq_data *data, bool mask)
{
	struct lombo_pin_bank *bank = irq_data_get_irq_chip_data(data);
	struct lombo_pinctrl_drv_data *d = bank->pctldrv;
	void __iomem *reg;
	u32 val;

	if (!strcmp(bank->name, GPIO_SIO_NAME))
		reg = d->regs_base + SIO_INT_MASK;
	else {
		dev_err(d->dev, "%s not interrupt bank\n", __func__);
		return;
	}

	val = readl(reg);
	if (mask)
		val |= 1 << (data->hwirq);
	else
		val &= ~(1 << (data->hwirq));
	writel(val, reg);
}

static void n7_sio_gpio_irq_unmask(struct irq_data *data)
{
	n7_sio_gpio_irq_set_mask(data, true);
}

static void n7_sio_gpio_irq_mask(struct irq_data *data)
{
	n7_sio_gpio_irq_set_mask(data, false);
}

static void n7_sio_gpio_irq_ack(struct irq_data *data)
{
	struct lombo_pin_bank *bank = irq_data_get_irq_chip_data(data);
	struct lombo_pinctrl_drv_data *d = bank->pctldrv;
	void __iomem *reg;

	if (!strcmp(bank->name, GPIO_SIO_NAME))
		reg = d->regs_base + SIO_INT_PEND;
	else {
		dev_err(d->dev, "%s not interrupt bank\n", __func__);
		return;
	}

	writel(1 << (data->hwirq), reg);
}

static int n7_sio_gpio_irq_set_type(struct irq_data *data,
				    unsigned int flow_type)
{
	struct lombo_pin_bank *bank = irq_data_get_irq_chip_data(data);
	struct lombo_pinctrl_drv_data *d = bank->pctldrv;
	void __iomem *reg;
	u32 nreg, shift, trigger, val;

	/* reg addr */
	if (!strcmp(bank->name, GPIO_SIO_NAME))
		reg = d->regs_base + SIO_INT_CFG;
	else {
		dev_err(d->dev, "%s not interrupt bank\n", __func__);
		return -EINVAL;
	}
	shift = data->hwirq * 4;
	nreg = shift / REG_BIT_WIDTH;
	shift = shift - nreg * REG_BIT_WIDTH;
	reg = reg + nreg * ADDR_STEP;

	/* interrupt type */
	switch (flow_type) {
	case IRQ_TYPE_LEVEL_LOW:
		trigger = EINT_TRIG_LOW_LEVEL;
		break;
	case IRQ_TYPE_LEVEL_HIGH:
		trigger = EINT_TRIG_HIGH_LEVEL;
		break;
	case IRQ_TYPE_EDGE_RISING:
		trigger = EINT_TRIG_RISING_EDGE;
		break;
	case IRQ_TYPE_EDGE_FALLING:
		trigger = EINT_TRIG_FALLING_EDGE;
		break;
	case IRQ_TYPE_EDGE_BOTH:
		trigger = EINT_TRIG_BOTH_EDGE;
		break;
	default:
		dev_err(d->dev, "eint trig type %d invalid\n", flow_type);
		return -EINVAL;
	}

	val = readl(reg);
	val &= ~(0xf << shift); /* clear the trig bits */
	val |= (trigger << shift); /* set the trig bits */
	writel(val, reg);
	return 0;
}

/* struct n7_sio_irq_chip: irq_chip for gpio interrupt */
static struct irq_chip n7_sio_irq_chip = {
	.name		= "GPIO-SIO",
	.irq_unmask	= n7_sio_gpio_irq_unmask,
	.irq_mask	= n7_sio_gpio_irq_mask,
	.irq_ack	= n7_sio_gpio_irq_ack,
	.irq_set_type	= n7_sio_gpio_irq_set_type,
};

int n7_sio_irq_domain_map(struct irq_domain *d, unsigned int virq,
			irq_hw_number_t hw)
{
	struct lombo_pin_bank *bank = d->host_data;

	irq_set_chip_and_handler(virq, &n7_sio_irq_chip, handle_level_irq);
	irq_set_chip_data(virq, bank);
	/* set_irq_flags(virq, IRQT_PROBE); */

	return 0;
}

const struct irq_domain_ops n7_sio_irq_domain_ops = {
	.map	= n7_sio_irq_domain_map,
	.xlate	= irq_domain_xlate_twocell,
};

void n7_sio_bank_irq_handler(unsigned int irq, struct irq_desc *desc)
{
	struct irq_chip *chip = irq_get_chip(irq);
	struct lombo_pin_bank *bank = irq_get_handler_data(irq);
	struct lombo_pinctrl_drv_data *d = bank->pctldrv;
	void __iomem *preg, *mreg;
	unsigned int pend, mask, irq_no, bit;

	WARN_ON(unlikely(NULL == bank));

	/* reg addr */
	if (!strcmp(bank->name, GPIO_SIO_NAME)) {
		preg = d->regs_base + SIO_INT_PEND;
		mreg = d->regs_base + SIO_INT_MASK;
	} else {
		dev_err(d->dev, "%s not interrupt bank\n", __func__);
		return;
	}

	chained_irq_enter(chip, desc);

	pend = readl(preg);
	mask = readl(mreg);
	pend = pend & mask & bank->eint_mask;

	while (pend) {
		bit = fls(pend) - 1;
		pend &= ~(1 << bit);

		irq_no = irq_find_mapping(bank->irq_domain, bit);
		if (!irq_no) {
			dev_err(d->dev, "%s: irq %s-%d(0x%x) unexpected!\n",
				__func__, bank->name, bit, (int)BIT(bit));
			/*
			 * normally, pending will be cleared in
			 * generic_handle_irq below, but it will not be called,
			 * so we must clear manually.
			 */
			writel(BIT(bit), preg);
			continue;
		}
		generic_handle_irq(irq_no);
	}

	chained_irq_exit(chip, desc);
}

static int n7_sio_eint_init(struct lombo_pinctrl_drv_data *pdata)
{
	struct device *dev = pdata->dev;
	struct lombo_pin_bank *bank;
	int n, eint_pin_num, clk_src, clk_div, ret;

	for (n = 0; n < pdata->nbanks; n++) {
		bank = &pdata->pin_banks[n];
		if (!bank->of_node) {
			dev_err(dev, "no bank %d node\n", n);
			continue;
		}
		if (bank->eint_type != EINT_TYPE_GPIO)
			continue;

		/* get sample time property from dts */
		ret = of_property_read_u32(bank->of_node, "clk-src", &clk_src);
		if (ret < 0) {
			dev_err(dev, "clk-src property invalid\n");
			clk_src = EINT_SAMP_HFEOSC;
		}

		ret = of_property_read_u32(bank->of_node, "clk-src-div",
					   &clk_div);
		if (ret < 0) {
			dev_err(dev, "clk-src-div property invalid\n");
			clk_div = 0;
		}

		dev_info(dev, "bank%d clk: %d, div: %d\n", n, clk_src, clk_div);

		/* before enable GPIO bank interrupt, configure sample clk */
		n7_sio_gpio_irq_set_samp(bank, clk_div, clk_src);

		bank->irq = irq_of_parse_and_map(bank->of_node, 0);
		if (!bank->irq) /* no "interrupts" property */
			continue;

		irq_set_handler_data(bank->irq, bank);
		irq_set_chained_handler(bank->irq, n7_sio_bank_irq_handler);

		eint_pin_num = fls(bank->eint_mask);
		bank->irq_domain = irq_domain_add_linear(bank->of_node,
				eint_pin_num, &n7_sio_irq_domain_ops, bank);
		if (!bank->irq_domain) {
			dev_err(dev, "irq domain %d add failed\n", n);
			return -ENXIO;
		}
	}

	return 0;
}

/* n8 pin controller pinctrl driver data */
struct lombo_pin_ctrl n7_sio_pin_ctrl[] = {
	{
		.pin_banks	= n7_sio_pin_banks,
		.nbanks		= ARRAY_SIZE(n7_sio_pin_banks),
		.eint_gpio_init	= n7_sio_eint_init,
	},
};

static int n7_sio_pinctrl_probe(struct platform_device *pdev)
{
	n7_sio_dev = pdev;
	return lombo_pinctrl_probe(pdev, n7_sio_pin_ctrl);
}

#ifdef CONFIG_PM

/**
 * save pinctrl state for suspend for a pin controller.
 *
 * Save data for all banks handled by this device.
 */
static void n7_sio_pinctrl_suspend_dev(struct lombo_pinctrl_drv_data *drv_data)
{
	void __iomem *regs_base = drv_data->regs_base;
	int i;

	for (i = 0; i < drv_data->nbanks; i++) {
		struct lombo_pin_bank *bank = &drv_data->pin_banks[i];
		void __iomem *reg = regs_base + bank->pctl_offset;
		int num;

		for (num = 0; num < BANK_REGISTERS_NUM; num++)
			bank->pm_save[num] = readl(reg + num * ADDR_STEP);
	}
}

/**
 * restore pinctrl state from suspend for a pin controller.
 *
 * Restore one of the banks that was saved during suspend.
 *
 */
static void n7_sio_pinctrl_resume_dev(struct lombo_pinctrl_drv_data *drv_data)
{
	void __iomem *regs_base = drv_data->regs_base;
	int i;

	for (i = 0; i < drv_data->nbanks; i++) {
		struct lombo_pin_bank *bank = &drv_data->pin_banks[i];
		void __iomem *reg = regs_base + bank->pctl_offset;
		int num;

		for (num = 0; num < BANK_REGISTERS_NUM; num++)
			writel(bank->pm_save[num], reg + num * ADDR_STEP);
	}
}

/**
 * save soc pin state for suspend, save data for all banks.
 */
static int n7_sio_pinctrl_suspend(void)
{
	struct lombo_pinctrl_drv_data *drv_data;

	drv_data = platform_get_drvdata(n7_sio_dev);
	if (drv_data)
		n7_sio_pinctrl_suspend_dev(drv_data);

	return 0;
}

/**
 * restore soc pin state for resume, restore data for all banks.
 */
static void n7_sio_pinctrl_resume(void)
{
	struct lombo_pinctrl_drv_data *drv_data;

	drv_data = platform_get_drvdata(n7_sio_dev);
	if (drv_data)
		n7_sio_pinctrl_resume_dev(drv_data);
}

static void n7_sio_pinctrl_shutdown(void)
{
	int i;
	void __iomem *regs_base;
	u32 val;
	struct lombo_pinctrl_drv_data *drv_data;

	drv_data = platform_get_drvdata(n7_sio_dev);
	if (drv_data) {
		regs_base = drv_data->regs_base;

		for (i = 0; i < drv_data->nbanks; i++) {
			struct lombo_pin_bank *bank = &drv_data->pin_banks[i];
			void __iomem *reg = regs_base + bank->pctl_offset;
			writel(0x0, reg + SIO_INT_CFG);
			writel(0x0, reg + SIO_INT_MASK);
			val = readl(reg + SIO_INT_PEND);
			writel(val, reg + SIO_INT_PEND);
			val = readl(reg + SIO_INT_SAMP);
			val &= ~BIT(31);
			writel(val, reg + SIO_INT_SAMP);
		}
	}
}

#else
#define n7_sio_pinctrl_suspend		NULL
#define n7_sio_pinctrl_resume		NULL
#define n7_sio_pinctrl_shutdown		NULL
#endif

static struct syscore_ops n7_sio_pinctrl_syscore_ops = {
	.suspend = n7_sio_pinctrl_suspend,
	.resume	 = n7_sio_pinctrl_resume,
	.shutdown = n7_sio_pinctrl_shutdown,
};

static const struct of_device_id n7_sio_pinctrl_dt_match[] = {
	{ .compatible = "lombo,n7-sio-pinctrl",
		.data = n7_sio_pin_ctrl },

	{},
};
/* MODULE_DEVICE_TABLE(of, n7_sio_pinctrl_dt_match); */

static struct platform_driver n7_sio_pinctrl_driver = {
	.probe  = n7_sio_pinctrl_probe,
	.driver = {
		.name = "n7-sio-pinctrl",
		.of_match_table = n7_sio_pinctrl_dt_match,
	},
};

/* pinctrl driver register must at an earlier initcall level */
static int __init n7_sio_pinctrl_drv_register(void)
{
	/* register syscore ops for register save/restore */
	register_syscore_ops(&n7_sio_pinctrl_syscore_ops);

	return platform_driver_register(&n7_sio_pinctrl_driver);
}
postcore_initcall(n7_sio_pinctrl_drv_register);

static void __exit n7_sio_pinctrl_drv_unregister(void)
{
	platform_driver_unregister(&n7_sio_pinctrl_driver);
}
module_exit(n7_sio_pinctrl_drv_unregister);

MODULE_AUTHOR("lomboswer@lombotech.com");
MODULE_DESCRIPTION("Driver for LomboTech pinctrl controller");
MODULE_LICENSE("GPL");
