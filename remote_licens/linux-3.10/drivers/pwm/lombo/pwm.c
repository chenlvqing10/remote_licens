/*
 * pwm.c - pwm driver code
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

#include <linux/module.h>
#include <linux/err.h>
#include <linux/of_address.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/pwm.h>

#include "csp/csp_pwm.h"
#include "pwm.h"

struct lombo_pwm_chip *to_lombo_pwm_chip(struct pwm_chip *chip)
{
	return container_of(chip, struct lombo_pwm_chip, chip);
}

int lombo_pwm_request(struct pwm_chip *chip,
				 struct pwm_device *pwm)
{
	/* struct lombo_pwm_chip *pwmc = to_lombo_pwm_chip(chip); */
	struct pwm_data *pdata;

	pdata = kzalloc(sizeof(struct pwm_data), GFP_KERNEL);
	if (!pdata) {
		PRT_ERR("alloc pwm_data failed\n");
		return -ENOMEM;
	}

	pwm_set_chip_data(pwm, pdata);
	return 0;
}

void lombo_pwm_free(struct pwm_chip *chip, struct pwm_device *pwm)
{
	/* struct lombo_pwm_chip *pwmc = to_lombo_pwm_chip(chip); */
	struct pwm_data *pdata = pwm_get_chip_data(pwm);

	kfree(pdata);
}

int lombo_pwm_enable(struct pwm_chip *chip, struct pwm_device *pwm)
{
	struct lombo_pwm_chip *pwmc = to_lombo_pwm_chip(chip);
	struct n7_pwm_clk_cfg cfg;
	struct pwm_data *pdata = pwm_get_chip_data(pwm);

	cfg.src = CLKSRC_HFEOSC;
	cfg.gating = 1;
	cfg.div = 1;
	csp_pwm_clk_config(pwm->hwpwm, pwmc->reg, &cfg);
	csp_pwm_control(pwm->hwpwm, PWM_PO_EN, pwmc->reg, 1);
	csp_pwm_cnt_endisable(pwm->hwpwm, pwmc->reg, true);

	csp_pwm_set_store(pwm->hwpwm, pwmc->reg, pdata->period_cnt);
	csp_pwm_set_cmp(pwm->hwpwm, pwmc->reg, pdata->duty_cnt);

	PRT_WARN("...\n");
	return 0;
}

void lombo_pwm_disable(struct pwm_chip *chip, struct pwm_device *pwm)
{
	struct lombo_pwm_chip *pwmc = to_lombo_pwm_chip(chip);
	struct n7_pwm_clk_cfg cfg;

	csp_pwm_cnt_endisable(pwm->hwpwm, pwmc->reg, false);

	cfg.src = CLKSRC_NULL;
	cfg.gating = 0;
	cfg.div = 1;
	csp_pwm_control(pwm->hwpwm, PWM_PO_EN, pwmc->reg, 0);
	csp_pwm_clk_config(pwm->hwpwm, pwmc->reg, &cfg);

	PRT_WARN("...\n");
}

int lombo_pwm_set_polarity(struct pwm_chip *chip,
				      struct pwm_device *pwm,
				      enum pwm_polarity polarity)
{
	struct lombo_pwm_chip *pwmc = to_lombo_pwm_chip(chip);
	struct pwm_data *pdata = pwm_get_chip_data(pwm);

	pdata->polarity = polarity;

	csp_pwm_control(pwm->hwpwm, PWM_PO_POLARITY_SET,
		pwmc->reg, (u32)polarity);
	csp_pwm_control(pwm->hwpwm, PWM_PO_EN, pwmc->reg, 1);

	PRT_WARN("...\n");
	return 0;
}

/**
 * convert_ns_to_cnt - convert ns to pwm count
 * @clk_rate: pwm's clk rate, that is, pwm count for 1s
 * @ns: nanosecond count
 *
 * return pwm count corresponding to ns
 */
unsigned int convert_ns_to_cnt(int clk_rate, int ns)
{
	u64 tmp = (u64)ns * clk_rate;

	do_div(tmp, NSEC_PER_SEC);
	return (unsigned int)tmp;
}

int lombo_pwm_config(struct pwm_chip *chip, struct pwm_device *pwm,
				int duty_ns, int period_ns)
{
	struct pwm_data *pdata = pwm_get_chip_data(pwm);
	struct lombo_pwm_chip *pwmc = to_lombo_pwm_chip(chip);
	int clk_rate, duty_cnt, period_cnt;

	/* get clock rate, and calca duty count and period count */
	clk_rate = 24000000;
	duty_cnt = convert_ns_to_cnt(clk_rate, duty_ns);
	period_cnt = convert_ns_to_cnt(clk_rate, period_ns);
	PRT_INFO("pwm %d, clk rate %d\n", (int)pwm->hwpwm, clk_rate);
	PRT_INFO("duty_ns %d, perid_ns %d, duty_cnt %d, perid_cnt %d\n",
		duty_ns, period_ns, duty_cnt, period_cnt);

	/*
	 * if duty_cnt/peroid_cnt is 0(invalid), maybe the clk rate or input ns
	 * too small. so warn here
	 */
	if (!duty_cnt || !period_cnt)
		PRT_DBG("duty_cnt(0x%08x) or period_cnt(0x%08x) is 0",
			duty_cnt, period_cnt);

	/*
	 * if duty_cnt/period_cnt exceed 0xffff, the reg cnt field will
	 * overflow, so we should decrease clk rate or input duty_ns/peroid_ns,
	 * to meet the case
	 */
	if ((duty_cnt & 0xffff0000) || (period_cnt & 0xffff0000)) {
		PRT_ERR("pwm %d, clk rate %d, duty_cnt(0x%08x)\n\t"
			"or period_cnt(0x%08x) invlaid, please re-config\n\t"
			"clk, or change the input duty_ns/peroid_ns para\n",
			(int)pwm->hwpwm, clk_rate, duty_cnt, period_cnt);
		return -EINVAL;
	}

	pdata->duty_ns = duty_ns;
	pdata->period_ns = period_ns;
	pdata->duty_cnt = duty_cnt;
	pdata->period_cnt = period_cnt;

	csp_pwm_set_store(pwm->hwpwm, pwmc->reg, pdata->period_cnt);
	csp_pwm_set_cmp(pwm->hwpwm, pwmc->reg, pdata->duty_cnt);

	return 0;
}

const struct pwm_ops lombo_pwm_ops = {
	.request = lombo_pwm_request,
	.free = lombo_pwm_free,
	.config = lombo_pwm_config,
	.set_polarity = lombo_pwm_set_polarity,
	.enable = lombo_pwm_enable,
	.disable = lombo_pwm_disable,
	.owner = THIS_MODULE,
};

int lombo_pwm_xxx(struct pwm_chip *chip,
			struct pwm_device *pwm)
{
	/* todo... */

	return 0;
}

const struct extra_pwm_ops lombo_extra_ops = {
	.xxx = lombo_pwm_xxx,
};

int lombo_pwm_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	struct lombo_pwm_chip *pwmc;
	struct resource res;
	int irq, ret = 0;
	struct pinctrl *ptl;
	struct pinctrl_state *pins;

	ptl = devm_pinctrl_get(&pdev->dev);

	if (IS_ERR(ptl) || ptl == NULL)
		PRT_ERR("get pinctrl fail\n");
	else {
		pins = pinctrl_lookup_state(ptl, "default");
		if (IS_ERR(pins) || pins == NULL)
			PRT_ERR("get pinctrl fail\n");
		else {
			pinctrl_select_state(ptl, pins);
			PRT_INFO("pwm get pin success\n");
		}
	}

	pwmc = kzalloc(sizeof(*pwmc), GFP_KERNEL);
	if (NULL == pwmc) {
		PRT_ERR("failed to alloc memory\n");
		return -ENOMEM;
	}

	/* get pwm system irq number */
	irq = platform_get_irq(pdev, 0);
	if (irq < 0) {
		PRT_ERR("failed to get irq\n");
		goto err_kfree;
	}
	pwmc->irq = irq;

	/* register map */
	if (of_address_to_resource(np, 0, &res)) {
		PRT_ERR("reg config invalid\n");
		goto err_kfree;
	}
	pwmc->reg = ioremap(res.start, resource_size(&res));

	/* pwm chip regitster */
	pwmc->ops = &lombo_extra_ops;
	pwmc->chip.dev = &pdev->dev;
	pwmc->chip.ops = &lombo_pwm_ops;
	pwmc->chip.of_xlate = of_pwm_xlate_with_flags;
	pwmc->chip.of_pwm_n_cells = 3;
	pwmc->chip.base = -1;		/* dynamic alloc base */
	pwmc->chip.npwm = NPWM;
	ret = pwmchip_add(&pwmc->chip);
	if (ret < 0) {
		PRT_ERR("pwmchip_add failed\n");
		goto err_iounmap;
	}

	platform_set_drvdata(pdev, pwmc);

	/* irq initialize */
	ret = pwm_irq_init(pdev, pwmc->reg);
	if (ret < 0) {
		PRT_ERR("pwm_irq_init failed\n");
		goto err_chip_remove;
	}

	PRT_INFO("success\n");
	return 0;

err_chip_remove:
	pwmchip_remove(&pwmc->chip);
err_iounmap:
	iounmap(pwmc->reg);
err_kfree:
	kfree(pwmc);
	return ret;
}

int lombo_pwm_remove(struct platform_device *pdev)
{
	struct lombo_pwm_chip *pwmc = platform_get_drvdata(pdev);
	int ret;

	pwm_irq_exit(pdev, NULL);

	ret = pwmchip_remove(&pwmc->chip);
	if (ret < 0) {
		PRT_ERR("pwmchip_remove err\n");
		return ret;
	}

	iounmap(pwmc->reg);
	kfree(pwmc);
	return 0;
}

const struct of_device_id lombo_pwm_dt_ids[] = {
	{ .compatible = "lombo,n7-pwm", },
	{},
};
MODULE_DEVICE_TABLE(of, lombo_pwm_dt_ids);

struct platform_driver lombo_pwm_driver = {
	.driver = {
		.name = "lombo-pwm",
		.of_match_table = lombo_pwm_dt_ids,
	},
	.probe = lombo_pwm_probe,
	.remove = lombo_pwm_remove,
};

static int __init lombo_pwm_driver_init(void)
{
	return platform_driver_register(&lombo_pwm_driver);
}
subsys_initcall(lombo_pwm_driver_init);

static void __exit lombo_pwm_driver_exit(void)
{
	platform_driver_unregister(&lombo_pwm_driver);
}
module_exit(lombo_pwm_driver_exit);

MODULE_AUTHOR("lomboswer@lombotech.com>");
MODULE_DESCRIPTION("Driver for LomboTech PWM controller");
MODULE_LICENSE("GPL");
