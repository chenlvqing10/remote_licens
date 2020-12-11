/*
 * pwm-irq.c - pwm irq realization
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

#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/irqdomain.h>
#include <linux/irqchip/chained_irq.h>
#include <linux/err.h>

#include "csp/csp_pwm.h"
#include "pwm.h"

static void pwm_irq_mask(struct irq_data *data)
{
	void __iomem *reg = irq_data_get_irq_chip_data(data);

	csp_pwm_irq_endisable((int)data->hwirq, reg, true);
}

static void pwm_irq_unmask(struct irq_data *data)
{
	void __iomem *reg = irq_data_get_irq_chip_data(data);

	csp_pwm_irq_endisable((int)data->hwirq, reg, false);
}

static int pwm_irq_set_type(struct irq_data *data, unsigned int flow_type)
{
	PRT_INFO("no operation\n");
	return 0;
}

static struct irq_chip pwm_irq_chip = {
	.name		= "PWM",
	.irq_mask	= pwm_irq_mask,
	.irq_unmask	= pwm_irq_unmask,
	.irq_set_type	= pwm_irq_set_type,
};

struct pwm_irq_data {
	struct irq_domain *domain;
	void __iomem *reg;
};

int pwm_irq_domain_map(struct irq_domain *d, unsigned int virq,
			irq_hw_number_t hw)
{
	struct pwm_irq_data *pdata = d->host_data;

	irq_set_chip_and_handler(virq, &pwm_irq_chip, handle_level_irq);
	irq_set_chip_data(virq, pdata->reg);
	set_irq_flags(virq, IRQF_VALID | IRQF_PROBE);

	return 0;
}

const struct irq_domain_ops pwm_irq_domain_ops = {
	.map	= pwm_irq_domain_map,
	.xlate	= irq_domain_xlate_onecell,
};

void pwm_irq_handler(unsigned int irq, struct irq_desc *desc)
{
	struct irq_chip *pchip = irq_get_chip(irq);
	struct pwm_irq_data *pdata = irq_get_handler_data(irq);
	unsigned int irq_no;
	u32 pend, bit;

	chained_irq_enter(pchip, desc);

	pend = csp_pwm_get_irq_pend_regval(pdata->reg);
	while (pend) {
		bit = fls(pend) - 1;
		pend &= ~(1 << bit);

		/* clear pending bit */
		csp_pwm_irq_clr_pend(bit, pdata->reg);

		/* get the linux irq number */
		irq_no = irq_find_mapping(pdata->domain, bit);
		WARN_ON(!irq_no);

		/* handle the irq, call user callback */
		generic_handle_irq(irq_no);
	}

	chained_irq_exit(pchip, desc);
}

int pwm_irq_init(struct platform_device *pdev, void __iomem *reg)
{
	struct lombo_pwm_chip *pwmc = platform_get_drvdata(pdev);
	struct pwm_irq_data *pdata;

	pdata = kzalloc(sizeof(*pdata), GFP_KERNEL);
	if (!pdata) {
		PRT_ERR("kzalloc failed\n");
		return -ENOMEM;
	}

	pdata->reg = reg;

	pdata->domain = irq_domain_add_linear(pdev->dev.of_node, PWM_IRQ_NUM,
				&pwm_irq_domain_ops, pdata);
	if (NULL == pdata->domain) {
		PRT_ERR("add domain failed\n");
		kfree(pdata);
		return -ENOMEM;
	}

	irq_set_chained_handler(pwmc->irq, pwm_irq_handler);
	irq_set_handler_data(pwmc->irq, pdata);
	return 0;
}

void pwm_irq_exit(struct platform_device *pdev, void __iomem *reg)
{
	struct lombo_pwm_chip *pwmc = platform_get_drvdata(pdev);
	struct pwm_irq_data *pdata;
	int i, irq;

	pdata = irq_get_handler_data(pwmc->irq);

	/* dispose the pwm irqs mapping, free the irq_desc */
	for (i = 0; i < PWM_IRQ_NUM; i++) {
		irq = irq_find_mapping(pdata->domain, i);
		if (irq > 0)
			irq_dispose_mapping(irq);
	}

	irq_domain_remove(pdata->domain);
	kfree(pdata);
}

