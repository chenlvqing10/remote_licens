/*
 * GPIO core driver - Driver of LomboTech UART Controller
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
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/gpio.h>
#include <linux/irqdomain.h>
#include <linux/spinlock.h>
#include <linux/clk.h>
#include <linux/clk-private.h>

#include "../core.h"
#include "pinctrl-lombo.h"

static unsigned int pin_base;

/* lombo soc pin config options supported */
static struct pin_config {
	const char *property;
	enum pincfg_type param;
} cfg_params[] = {
	{ "lombo,pin-pud", PINCFG_TYPE_PUD },
	{ "lombo,pin-drv", PINCFG_TYPE_DRV },
	{ "lombo,pin-val", PINCFG_TYPE_DAT },
	{ "lombo,pin-pud-reg", PINCFG_TYPE_PUD_REG },
};

static inline struct lombo_pin_bank *gc_to_pin_bank(struct gpio_chip *gc)
{
	return container_of(gc, struct lombo_pin_bank, gpio_chip);
}

/* get the group count number */
static int lombo_get_group_count(struct pinctrl_dev *pctldev)
{
	struct lombo_pinctrl_drv_data *pctldrv;
	pctldrv = pinctrl_dev_get_drvdata(pctldev);

	return pctldrv->ngroups;
}

/* get special group name */
static const char *lombo_get_group_name(struct pinctrl_dev *pctldev,
					unsigned selector)
{
	struct lombo_pinctrl_drv_data *pctldrv;
	pctldrv = pinctrl_dev_get_drvdata(pctldev);

	return pctldrv->groups[selector].name;
}

/* get special group pin informations */
static int lombo_get_group_pins(struct pinctrl_dev *pctldev, unsigned selector,
				const unsigned **pins, unsigned *num_pins)
{
	struct lombo_pinctrl_drv_data *pctldrv;
	pctldrv = pinctrl_dev_get_drvdata(pctldev);

	*pins = pctldrv->groups[selector].pins;
	*num_pins = pctldrv->groups[selector].npins;

	return 0;
}

/* free device pinctrl_map memory */
static void lombo_dt_free_map(struct pinctrl_dev *pctldev,
			      struct pinctrl_map *map, unsigned num_maps)
{
	int i;

	for (i = 0; i < num_maps; i++)
		if (map[i].type == PIN_MAP_TYPE_CONFIGS_GROUP)
			kfree(map[i].data.configs.configs);

	kfree(map);
}

static int add_config(struct device *dev, unsigned long **configs,
		      unsigned *num_configs, unsigned long config)
{
	unsigned old_num = *num_configs;
	unsigned new_num = old_num + 1;
	unsigned long *new_configs;

	new_configs = krealloc(*configs, sizeof(*new_configs) * new_num,
				GFP_KERNEL);
	if (!new_configs) {
		dev_err(dev, "krealloc configs failed\n");
		return -ENOMEM;
	}

	new_configs[old_num] = config;
	*configs = new_configs;
	*num_configs = new_num;

	return 0;
}

static int reserve_map(struct device *dev, struct pinctrl_map **map,
			unsigned *reserved_maps, unsigned *num_maps,
			unsigned reserve)
{
	unsigned old_num = *reserved_maps;
	unsigned new_num = *num_maps + reserve;
	struct pinctrl_map *new_map;

	if (old_num >= new_num)
		return 0;

	new_map = krealloc(*map, sizeof(*new_map) * new_num, GFP_KERNEL);
	if (!new_map) {
		dev_err(dev, "krealloc pinctrl map failed\n");
		return -ENOMEM;
	}

	memset(new_map + old_num, 0, (new_num - old_num) * sizeof(*new_map));

	*map = new_map;
	*reserved_maps = new_num;

	return 0;
}

static int add_map_mux(struct pinctrl_map **map, unsigned *reserved_maps,
			unsigned *num_maps, const char *group, const char *fun)
{
	if (WARN_ON(*num_maps == *reserved_maps))
		return -ENOSPC;

	(*map)[*num_maps].type = PIN_MAP_TYPE_MUX_GROUP;
	(*map)[*num_maps].data.mux.group = group;
	(*map)[*num_maps].data.mux.function = fun;
	(*num_maps)++;

	return 0;
}

static int add_map_configs(struct device *dev, struct pinctrl_map **map,
			   unsigned *reserved_maps, unsigned *num_maps,
			   const char *group, unsigned long *configs,
			   unsigned num_configs)
{
	unsigned long *dup_configs;

	if (WARN_ON(*num_maps == *reserved_maps))
		return -ENOSPC;

	dup_configs = kmemdup(configs, num_configs * sizeof(*dup_configs),
			      GFP_KERNEL);

	if (!dup_configs) {
		dev_err(dev, "kmemdup configs failed\n");
		return -ENOMEM;
	}

	(*map)[*num_maps].type = PIN_MAP_TYPE_CONFIGS_GROUP;
	(*map)[*num_maps].data.configs.group_or_pin = group;
	(*map)[*num_maps].data.configs.configs = dup_configs;
	(*map)[*num_maps].data.configs.num_configs = num_configs;
	(*num_maps)++;

	return 0;
}

/* parse pinctrl dt subnode to create pinctrl_map for devices */
static int lombo_dt_subnode_to_map(struct lombo_pinctrl_drv_data *pctldrv,
				   struct device *dev,
				   struct device_node *np,
				   struct pinctrl_map **map,
				   unsigned *reserved_maps,
				   unsigned *num_maps)
{
	int ret, i;
	u32 val;
	unsigned long config;
	unsigned long *configs = NULL;
	unsigned num_configs = 0;
	unsigned reserve = 0;
	struct property *prop;
	const char *group;
	bool has_func = false;

	ret = of_property_read_u32(np, "lombo,pin-function", &val);
	if (!ret)
		has_func = true;

	for (i = 0; i < ARRAY_SIZE(cfg_params); i++) {
		ret = of_property_read_u32(np, cfg_params[i].property, &val);
		if (!ret) {
			config = PINCFG_PACK(cfg_params[i].param, val);
			ret = add_config(dev, &configs, &num_configs, config);
			if (ret < 0)
				goto exit;
		} else if (ret != -EINVAL) {
			dev_err(dev, "parse %s fail\n", cfg_params[i].property);
		}
	}

	if (has_func)
		reserve++;
	if (num_configs)
		reserve++;

	ret = of_property_count_strings(np, "lombo,pins");
	if (ret < 0) {
		dev_err(dev, "parse lombo,pins property failed\n");
		goto exit;
	}
	reserve *= ret;

	ret = reserve_map(dev, map, reserved_maps, num_maps, reserve);
	if (ret < 0) {
		dev_err(dev, "pin reserve_map failed\n");
		goto exit;
	}

	of_property_for_each_string(np, "lombo,pins", prop, group) {
		if (has_func) {
			ret = add_map_mux(map, reserved_maps, num_maps, group,
					  np->full_name);
			if (ret < 0) {
				dev_err(dev, "pin add_map_mux failed\n");
				goto exit;
			}
		}

		if (num_configs) {
			ret = add_map_configs(dev, map, reserved_maps, num_maps,
					      group, configs, num_configs);
			if (ret < 0) {
				dev_err(dev, "pin add_map_configs failed\n");
				goto exit;
			}
		}
	}

	ret = 0;

exit:
	kfree(configs);
	return ret;
}

/* parsing pinctrl device tree node to create pinctrl_map entries */
static int lombo_dt_node_to_map(struct pinctrl_dev *pctldev,
				struct device_node *np_config,
				struct pinctrl_map **map,
				unsigned *num_maps)
{
	struct lombo_pinctrl_drv_data *pctldrv;
	unsigned reserved_maps = 0;
	struct device_node *np;
	int ret;

	*map = NULL;
	*num_maps = 0;
	pctldrv = pinctrl_dev_get_drvdata(pctldev);

	if (!of_get_child_count(np_config))
		return lombo_dt_subnode_to_map(pctldrv, pctldev->dev, np_config,
						map, &reserved_maps, num_maps);

	for_each_child_of_node(np_config, np) {
		ret = lombo_dt_subnode_to_map(pctldrv, pctldev->dev, np, map,
					      &reserved_maps, num_maps);
		if (ret < 0) {
			lombo_dt_free_map(pctldev, *map, *num_maps);
			return ret;
		}
	}

	return 0;
}

/* list of pinctrl callbacks for the pinctrl core */
static const struct pinctrl_ops lombo_pctrl_ops = {
	.get_groups_count	= lombo_get_group_count,
	.get_group_name		= lombo_get_group_name,
	.get_group_pins		= lombo_get_group_pins,
	.dt_node_to_map		= lombo_dt_node_to_map,
	.dt_free_map		= lombo_dt_free_map,
};

/* get the soc functions count  */
static int lombo_pinmux_get_functions_count(struct pinctrl_dev *pctldev)
{
	struct lombo_pinctrl_drv_data *pctldrv;

	pctldrv = pinctrl_dev_get_drvdata(pctldev);
	return pctldrv->nfunctions;
}

/* get special function name */
static const char *lombo_pinmux_get_function_name(struct pinctrl_dev *pctldev,
					  unsigned selector)
{
	struct lombo_pinctrl_drv_data *pctldrv;

	pctldrv = pinctrl_dev_get_drvdata(pctldev);
	return pctldrv->functions[selector].name;
}

static int lombo_pinmux_get_function_groups(struct pinctrl_dev *pctldev,
				   unsigned selector,
				   const char * const **groups,
				   unsigned * const num_groups)
{
	struct lombo_pinctrl_drv_data *pctldrv;

	pctldrv = pinctrl_dev_get_drvdata(pctldev);
	*groups = pctldrv->functions[selector].groups;
	*num_groups = pctldrv->functions[selector].ngroups;
	return 0;
}

/* get the special pin bank */
static void pin_to_reg_bank(struct lombo_pinctrl_drv_data *pctldrv,
			    unsigned pin, void __iomem **reg, u32 *offset,
			    struct lombo_pin_bank **bank)
{
	struct lombo_pin_bank *b = pctldrv->pin_banks;

	while ((pin >= b->pin_base) && ((b->pin_base + b->npins - 1) < pin))
		b++;

	*reg = pctldrv->regs_base + b->pctl_offset;
	*offset = pin - b->pin_base;
	if (bank)
		*bank = b;
}

/* pin mux function register write ops*/
static int lombo_pinmux_setup(struct pinctrl_dev *pctldev, unsigned selector,
				unsigned group, bool enable)
{
	struct lombo_pinctrl_drv_data *pctldrv;
	const struct lombo_pin_bank_type *type;
	struct lombo_pin_bank *bank;
	const struct lombo_pmx_func *func;
	const struct lombo_pin_group *grp;
	void __iomem *reg;
	unsigned long flags;
	u32 mask, shift, data, pin_offset, nreg;

	pctldrv = pinctrl_dev_get_drvdata(pctldev);
	func = &pctldrv->functions[selector];
	grp = &pctldrv->groups[group];

	pin_to_reg_bank(pctldrv, grp->pins[0] - pctldrv->pin_base, &reg,
			&pin_offset, &bank);

	type = bank->type;
	mask = (1 << type->fld_width[PINCFG_TYPE_FUNC]) - 1;
	shift = pin_offset * type->fld_width[PINCFG_TYPE_FUNC];

	/* register is 32-bits width, select config register */
	nreg = shift / REG_BIT_WIDTH;
	shift = shift - nreg * REG_BIT_WIDTH;
	reg = reg + nreg * ADDR_STEP;

	spin_lock_irqsave(&bank->slock, flags);
	data = readl(reg + type->reg_offset[PINCFG_TYPE_FUNC]);
	data &= ~(mask << shift);
	if (enable)
		data |= func->val << shift;
	writel(data, reg + type->reg_offset[PINCFG_TYPE_FUNC]);
	spin_unlock_irqrestore(&bank->slock, flags);

	return 0;
}

/* enable special pin mux function */
static int lombo_pinmux_enable(struct pinctrl_dev *pctldev, unsigned selector,
				unsigned group)
{
	return lombo_pinmux_setup(pctldev, selector, group, true);
}

/* disable special pin mux function */
static void lombo_pinmux_disable(struct pinctrl_dev *pctldev, unsigned selector,
				unsigned group)
{
	lombo_pinmux_setup(pctldev, selector, group, false);
}

/* list of pinmux callbacks for the pinmux vertical in pinctrl core */
static const struct pinmux_ops lombo_pinmux_ops = {
	.get_functions_count	= lombo_pinmux_get_functions_count,
	.get_function_name	= lombo_pinmux_get_function_name,
	.get_function_groups	= lombo_pinmux_get_function_groups,
	.enable			= lombo_pinmux_enable,
	.disable		= lombo_pinmux_disable,
};

/* pin configuration register read or write ops*/
static int lombo_pinconf_rw(struct pinctrl_dev *pctldev, unsigned int pin,
			    unsigned long *config, bool set)
{
	struct lombo_pinctrl_drv_data *pctldrv;
	const struct lombo_pin_bank_type *type;
	struct lombo_pin_bank *bank;
	enum pincfg_type cfg_type;
	void __iomem *reg;
	unsigned long flags;
	u32 data, width, pin_offset, mask, shift, nreg;
	u32 cfg_value, cfg_reg;

	pctldrv = pinctrl_dev_get_drvdata(pctldev);
	cfg_type = PINCFG_UNPACK_TYPE(*config);
	pin_to_reg_bank(pctldrv, pin - pctldrv->pin_base, &reg, &pin_offset,
			&bank);

	type = bank->type;
	if (cfg_type >= PINCFG_TYPE_NUM || !type->fld_width[cfg_type]) {
		dev_err(pctldev->dev, "pin config invalid\n");
		return -EINVAL;
	}

	width = type->fld_width[cfg_type];
	cfg_reg = type->reg_offset[cfg_type];

	spin_lock_irqsave(&bank->slock, flags);

	/* pull up/down and drive strength*/
	if (cfg_type == PINCFG_TYPE_PUD || cfg_type == PINCFG_TYPE_DRV) {
		shift = pin_offset * 4;
		nreg = shift / REG_BIT_WIDTH;
		shift = shift - nreg * REG_BIT_WIDTH;
		reg = reg + nreg * ADDR_STEP;
	} else
		shift = pin_offset * width;

	mask = (1 << width) - 1;
	data = readl(reg + cfg_reg);

	/* set config */
	if (set) {
		cfg_value = PINCFG_UNPACK_VALUE(*config);
		data &= ~(mask << shift);
		data |= (cfg_value << shift);
		writel(data, reg + cfg_reg);
	} else {
		data >>= shift;
		data &= mask;
		*config = PINCFG_PACK(cfg_type, data);
	}

	spin_unlock_irqrestore(&bank->slock, flags);
	return 0;
}

/* get a  pin configuration */
static int lombo_pinconf_get(struct pinctrl_dev *pctldev, unsigned pin,
			     unsigned long *config)
{
	return lombo_pinconf_rw(pctldev, pin, config, false);
}

/* set a  pin configuration */
static int lombo_pinconf_set(struct pinctrl_dev *pctldev, unsigned pin,
			     unsigned long config)
{
	return lombo_pinconf_rw(pctldev, pin, &config, true);
}

/* set a group pins configuration */
static int lombo_pinconf_group_get(struct pinctrl_dev *pctldev,
				   unsigned group, unsigned long *config)
{
	struct lombo_pinctrl_drv_data *pctldrv;
	const unsigned int *pins;

	pctldrv = pinctrl_dev_get_drvdata(pctldev);
	pins = pctldrv->groups[group].pins;
	lombo_pinconf_get(pctldev, pins[0], config);

	return 0;
}

/* get a group pins configuration */
static int lombo_pinconf_group_set(struct pinctrl_dev *pctldev, unsigned group,
				   unsigned long config)
{
	struct lombo_pinctrl_drv_data *pctldrv;
	const unsigned int *pins;
	unsigned int i;

	pctldrv = pinctrl_dev_get_drvdata(pctldev);
	pins = pctldrv->groups[group].pins;
	for (i = 0; i < pctldrv->groups[group].npins; i++)
		lombo_pinconf_set(pctldev, pins[i], config);

	return 0;
}

/* list of pinconfig callbacks for pinconfig vertical in the pinctrl code */
static const struct pinconf_ops lombo_pinconf_ops = {
	.pin_config_get		= lombo_pinconf_get,
	.pin_config_set		= lombo_pinconf_set,
	.pin_config_group_get	= lombo_pinconf_group_get,
	.pin_config_group_set	= lombo_pinconf_group_set,
};

/* create pin groups information struct */
static struct lombo_pin_group *lombo_pinctrl_create_groups(
					struct device *dev,
					struct lombo_pinctrl_drv_data *pctldrv,
					unsigned int *cnt)
{
	struct pinctrl_desc *pctldesc = &pctldrv->pctl_desc;
	const struct pinctrl_pin_desc *pdesc;
	struct lombo_pin_group *groups, *grp;
	int i;

	/* each pin for a group */
	groups = devm_kzalloc(dev, pctldesc->npins * sizeof(*groups),
			      GFP_KERNEL);
	if (!groups) {
		dev_err(dev, "alloc pin group mem failed\n");
		return ERR_PTR(-ENOMEM);
	}
	grp = groups;

	pdesc = pctldesc->pins;
	for (i = 0; i < pctldesc->npins; ++i, ++pdesc, ++grp) {
		grp->name = pdesc->name;
		grp->pins = &pdesc->number;
		grp->npins = 1;
	}

	*cnt = pctldesc->npins;
	return groups;
}

/* create pin functions information struct */
static struct lombo_pmx_func *lombo_pinctrl_create_functions(
					struct device *dev,
					struct lombo_pinctrl_drv_data *pctldrv,
					unsigned int *cnt)
{
	struct lombo_pmx_func *functions, *func;
	struct device_node *dev_np = dev->of_node;
	struct device_node *cfg_np;
	unsigned int func_cnt = 0;
	int ret, npins, i;

	for_each_child_of_node(dev_np, cfg_np) {
		if (!of_find_property(cfg_np, "lombo,pin-function", NULL))
			continue;
		++func_cnt;
	}

	functions = devm_kzalloc(dev, func_cnt * sizeof(*func), GFP_KERNEL);
	if (!functions) {
		dev_err(dev, "alloc function mem failed\n");
		return ERR_PTR(-EINVAL);
	}
	func = functions;

	func_cnt = 0;
	for_each_child_of_node(dev_np, cfg_np) {
		if (of_property_read_u32(cfg_np, "lombo,pin-function",
					 &func->val))
			continue;

		npins = of_property_count_strings(cfg_np, "lombo,pins");
		if (npins < 1) {
			dev_err(dev, "%s pin list invalid\n", cfg_np->name);
			return ERR_PTR(-EINVAL);
		}

		func->name = cfg_np->full_name;
		func->groups = devm_kzalloc(dev, npins * sizeof(char *),
					    GFP_KERNEL);
		if (!func->groups) {
			dev_err(dev, "alloc pin groups mem failed\n");
			return ERR_PTR(-ENOMEM);
		}

		for (i = 0; i < npins; ++i) {
			const char *gname;
			ret = of_property_read_string_index(cfg_np,
							    "lombo,pins", i,
							    &gname);
			if (ret) {
				dev_err(dev, "read %s %d pin name failed\n",
					cfg_np->name, i);
				continue;
			}
			func->groups[i] = gname;
		}

		func->ngroups = npins;
		++func;
		++func_cnt;
	}

	*cnt = func_cnt;
	return functions;
}

/* parse dt nodo to special struct */
static int lombo_pinctrl_parse_dt(struct platform_device *pdev,
				   struct lombo_pinctrl_drv_data *pctldrv)
{
	struct lombo_pin_group *groups;
	struct lombo_pmx_func *functions;
	struct device *dev = &pdev->dev;
	unsigned int grp_cnt = 0, func_cnt = 0;

	groups = lombo_pinctrl_create_groups(dev, pctldrv, &grp_cnt);
	if (IS_ERR(groups))
		return PTR_ERR(groups);

	functions = lombo_pinctrl_create_functions(dev, pctldrv, &func_cnt);
	if (IS_ERR(functions))
		return PTR_ERR(functions);

	pctldrv->groups = groups;
	pctldrv->ngroups = grp_cnt;
	pctldrv->functions = functions;
	pctldrv->nfunctions = func_cnt;

	return 0;
}

/* register pinctrl driver interface to pinctrl subsystem */
static int lombo_pinctrl_register(struct platform_device *pdev,
				  struct lombo_pinctrl_drv_data *pctldrv)
{
	struct pinctrl_desc *pctldesc = &pctldrv->pctl_desc;
	struct pinctrl_pin_desc *pindesc, *pdesc;
	struct lombo_pin_bank *pin_bank;
	char *pin_names;
	int pin, bank, ret;

	pctldesc->name = "lombo-pinctrl";
	pctldesc->owner = THIS_MODULE;
	pctldesc->pctlops = &lombo_pctrl_ops;
	pctldesc->pmxops = &lombo_pinmux_ops;
	pctldesc->confops = &lombo_pinconf_ops;

	pindesc = devm_kzalloc(&pdev->dev, sizeof(*pindesc) * pctldrv->npins,
				GFP_KERNEL);
	if (!pindesc) {
		dev_err(&pdev->dev, "alloc pin desc mem failed\n");
		return -ENOMEM;
	}
	pctldesc->pins = pindesc;
	pctldesc->npins = pctldrv->npins;

	/* pin number */
	for (pin = 0, pdesc = pindesc; pin < pctldesc->npins; pin++, pdesc++)
		pdesc->number = pin + pctldrv->pin_base;

	/* pin name */
	pin_names = devm_kzalloc(&pdev->dev,
				sizeof(char) * PIN_NAME_LENGTH * pctldrv->npins,
				GFP_KERNEL);
	if (!pin_names) {
		dev_err(&pdev->dev, "alloc pin name mem failed\n");
		return -ENOMEM;
	}

	for (bank = 0; bank < pctldrv->nbanks; bank++) {
		pin_bank = &pctldrv->pin_banks[bank];
		for (pin = 0; pin < pin_bank->npins; pin++) {
			sprintf(pin_names, "%s-%d", pin_bank->name, pin);
			pdesc = pindesc + pin_bank->pin_base + pin;
			pdesc->name = pin_names;
			pin_names += PIN_NAME_LENGTH;
		}
	}

	ret = lombo_pinctrl_parse_dt(pdev, pctldrv);
	if (ret)
		return ret;

	pctldrv->pctl_dev = pinctrl_register(pctldesc, &pdev->dev, pctldrv);
	if (IS_ERR(pctldrv->pctl_dev)) {
		dev_err(&pdev->dev, "register pinctrl driver failed\n");
		return PTR_ERR(pctldrv->pctl_dev);
	}

	for (bank = 0; bank < pctldrv->nbanks; ++bank) {
		pin_bank = &pctldrv->pin_banks[bank];
		pin_bank->grange.name = pin_bank->name;
		pin_bank->grange.id = bank;
		pin_bank->grange.pin_base = pctldrv->pin_base
						+ pin_bank->pin_base;
		pin_bank->grange.base = pin_bank->gpio_chip.base;
		pin_bank->grange.npins = pin_bank->gpio_chip.ngpio;
		pin_bank->grange.gc = &pin_bank->gpio_chip;
		pinctrl_add_gpio_range(pctldrv->pctl_dev, &pin_bank->grange);
	}

	return 0;
}

int lombo_gpio_request(struct gpio_chip *gc, unsigned offset)
{
	struct lombo_pin_bank *bank = gc_to_pin_bank(gc);
	int gpio = gc->base + offset;

	dev_info(gc->dev, "%s: %s request gpio-%d offset-%d (gpio base:%d, bank base: %d)\n",
		__func__, gc->label, gpio, offset, gc->base, bank->pin_base);

	return pinctrl_request_gpio(gpio);
}

void lombo_gpio_free(struct gpio_chip *gc, unsigned offset)
{
	int gpio = gc->base + offset;

	return pinctrl_free_gpio(gpio);
}

/* set a output gpio pin data value  */
static void lombo_gpio_set(struct gpio_chip *gc, unsigned offset, int value)
{
	struct lombo_pin_bank *bank = gc_to_pin_bank(gc);
	const struct lombo_pin_bank_type *type = bank->type;
	unsigned long flags;
	void __iomem *reg;
	u32 data;

	reg = bank->pctldrv->regs_base + bank->pctl_offset;

	spin_lock_irqsave(&bank->slock, flags);

	data = readl(reg + type->reg_offset[PINCFG_TYPE_DAT]);
	data &= ~(1 << offset);
	if (value)
		data |= 1 << offset;
	writel(data, reg + type->reg_offset[PINCFG_TYPE_DAT]);
	spin_unlock_irqrestore(&bank->slock, flags);
}

/* get a input gpio pin data value */
static int lombo_gpio_get(struct gpio_chip *gc, unsigned offset)
{
	struct lombo_pin_bank *bank = gc_to_pin_bank(gc);
	const struct lombo_pin_bank_type *type = bank->type;
	void __iomem *reg;
	u32 data;

	reg = bank->pctldrv->regs_base + bank->pctl_offset;

	data = readl(reg + type->reg_offset[PINCFG_TYPE_DAT]);
	data >>= offset;
	data &= 1;

	return data;
}

/* gpio direction setting function */
static int lombo_gpio_set_direction(struct gpio_chip *gc, unsigned offset,
				    bool io)
{
	struct lombo_pin_bank *bank = gc_to_pin_bank(gc);
	const struct lombo_pin_bank_type *type = bank->type;
	struct lombo_pinctrl_drv_data *pctldrv = bank->pctldrv;
	void __iomem *reg;
	unsigned long flags;
	u32 data, mask, shift, nreg;

	reg = pctldrv->regs_base + bank->pctl_offset;
	reg += type->reg_offset[PINCFG_TYPE_FUNC];

	mask = (1 << type->fld_width[PINCFG_TYPE_FUNC]) - 1;
	shift = offset * type->fld_width[PINCFG_TYPE_FUNC];

	/* register is 32-bits width, select config register */
	nreg = shift / REG_BIT_WIDTH;
	shift = shift - nreg * REG_BIT_WIDTH;
	reg = reg + nreg * ADDR_STEP;

	spin_lock_irqsave(&bank->slock, flags);
	data = readl(reg);
	data &= ~(mask << shift);

	/* output */
	if (!io)
		data |= FUN_OUTPUT << shift;
	else
		data |= FUN_INPUT << shift;
	writel(data, reg);
	spin_unlock_irqrestore(&bank->slock, flags);

	return 0;
}

/* set pin function as gpio input */
static int lombo_gpio_direction_input(struct gpio_chip *gc, unsigned offset)
{
	return lombo_gpio_set_direction(gc, offset, true);
}

/* set pin function as gpio output */
static int lombo_gpio_direction_output(struct gpio_chip *gc, unsigned offset,
					int value)
{
	lombo_gpio_set(gc, offset, value);
	return lombo_gpio_set_direction(gc, offset, false);
}

/* IRQ */
static int lombo_gpio_to_irq(struct gpio_chip *gc, unsigned offset)
{
	struct lombo_pin_bank *bank = gc_to_pin_bank(gc);
	int virq;

	if (!bank->irq_domain) {
		dev_info(gc->dev, "no irq domain %d\n", __LINE__);
		return -ENXIO;
	}

	virq = irq_find_mapping(bank->irq_domain, offset);
	if (virq == 0)
		virq = irq_create_mapping(bank->irq_domain, offset);

	return virq;
}

static const struct gpio_chip lombo_gpiolib_chip = {
	.request = lombo_gpio_request,
	.free = lombo_gpio_free,
	.set = lombo_gpio_set,
	.get = lombo_gpio_get,
	.direction_input = lombo_gpio_direction_input,
	.direction_output = lombo_gpio_direction_output,
	.to_irq = lombo_gpio_to_irq,
	.owner = THIS_MODULE,
};

/* register the gpiolib interface with the gpiolib subsystem */
static int lombo_gpiolib_register(struct platform_device *pdev,
				  struct lombo_pinctrl_drv_data *pctldrv)
{
	struct lombo_pin_bank *bank = pctldrv->pin_banks;
	struct gpio_chip *gc;
	int ret, i;

	for (i = 0; i < pctldrv->nbanks; ++i, ++bank) {
		bank->gpio_chip = lombo_gpiolib_chip;
		gc = &bank->gpio_chip;
		gc->base = pctldrv->pin_base + bank->pin_base;
		gc->ngpio = bank->npins;
		gc->dev = &pdev->dev;
		gc->of_node = bank->of_node;
		gc->label = bank->name;

		ret = gpiochip_add(gc);
		if (ret) {
			dev_err(&pdev->dev, "register %s bank failed %d\n",
				gc->label, ret);
			goto fail;
		}
	}

	return 0;

fail:
	for (--i, --bank; i >= 0; --i, --bank) {
		ret = gpiochip_remove(&bank->gpio_chip);
		if (ret)
			dev_err(&pdev->dev, "gpiolib remove %s failed\n",
						bank->gpio_chip.label);
	}

	return ret;
}

/* unregister the gpiolib interface with the gpiolib subsystem */
static int lombo_gpiolib_unregister(struct platform_device *pdev,
				    struct lombo_pinctrl_drv_data *pctldrv)
{
	struct lombo_pin_bank *bank = pctldrv->pin_banks;
	int i, ret = 0;

	for (i = 0; i < pctldrv->nbanks; ++i, ++bank) {
		ret = gpiochip_remove(&bank->gpio_chip);
		if (ret)
			dev_err(&pdev->dev, "gpiolib remove %s failed\n",
						bank->gpio_chip.label);
	}

	return 0;
}

static const struct of_device_id lombo_pinctrl_dt_match[];

/* get driver static define lombo_pin_ctrl struct */
static const struct lombo_pin_ctrl *lombo_pinctrl_get_soc_data(
					struct lombo_pinctrl_drv_data *pctldrv,
					struct platform_device *pdev,
					struct lombo_pin_ctrl *data)
{
	const struct lombo_pin_bank_data *bank_data;
	const struct lombo_pin_ctrl *ctrl;
	struct lombo_pin_bank *bank;
	struct device_node *node = pdev->dev.of_node;
	struct device_node *np;
	int i;

	if (!data) {
		dev_err(&pdev->dev, "lombo pin ctrl data is null\n");
		return ERR_PTR(-ENOMEM);
	}
	ctrl = data;

	pctldrv->nbanks = ctrl->nbanks;
	pctldrv->pin_banks = devm_kcalloc(&pdev->dev, pctldrv->nbanks,
					  sizeof(*pctldrv->pin_banks),
					  GFP_KERNEL);
	if (!pctldrv->pin_banks) {
		dev_err(&pdev->dev, "alloc pin banks mem falied\n");
		return ERR_PTR(-ENOMEM);
	}

	bank = pctldrv->pin_banks;
	bank_data = ctrl->pin_banks;
	for (i = 0; i < ctrl->nbanks; ++i, ++bank_data, ++bank) {
		bank->type = bank_data->type;
		bank->pctl_offset = bank_data->pctl_offset;
		bank->npins = bank_data->npins;
		bank->eint_func = bank_data->eint_func;
		bank->eint_type = bank_data->eint_type;
		bank->eint_mask = bank_data->eint_mask;
		bank->eint_offset = bank_data->eint_offset;
		bank->name = bank_data->name;
		spin_lock_init(&bank->slock);
		bank->pctldrv = pctldrv;
		bank->pin_base = pctldrv->npins;
		pctldrv->npins += bank->npins;
	}

	for_each_child_of_node(node, np) {
		if (!of_find_property(np, "gpio-controller", NULL))
			continue;
		bank = pctldrv->pin_banks;
		for (i = 0; i < pctldrv->nbanks; ++i, ++bank) {
			if (!strcmp(bank->name, np->name)) {
				bank->of_node = np;
				break;
			}
		}
	}

	pctldrv->pin_base = pin_base;
	pin_base += pctldrv->npins;

	return ctrl;
}

int lombo_pinctrl_probe(struct platform_device *pdev,
			struct lombo_pin_ctrl *data)
{
	struct lombo_pinctrl_drv_data *pctldrv;
	const struct lombo_pin_ctrl *ctrl;
	struct device *dev = &pdev->dev;
	struct resource *res;
	struct clk *gate, *reset;
	int len;
	int ret;

	if (!dev->of_node) {
		dev_err(dev, "no device tree info\n");
		return -ENODEV;
	}

	pctldrv = devm_kzalloc(dev, sizeof(*pctldrv), GFP_KERNEL);
	if (!pctldrv) {
		dev_err(dev, "allocate driver data fialed\n");
		return -ENOMEM;
	}

	ctrl = lombo_pinctrl_get_soc_data(pctldrv, pdev, data);
	if (IS_ERR(ctrl)) {
		dev_err(dev, "get driver data failed\n");
		return PTR_ERR(ctrl);
	}
	pctldrv->dev = dev;

	if (!of_find_property(dev->of_node, "no-clk", &len)) {
		/* clk resource */
		gate = devm_clk_get(dev, "apb_gpio_gate");
		reset = devm_clk_get(dev, "apb_gpio_reset");
		if (IS_ERR(gate) || IS_ERR(reset)) {
			dev_err(dev, "get pinctrl gate/reset clk failed\n");
			return -EINVAL;
		}

		ret = clk_prepare_enable(reset);
		if (ret) {
			dev_err(dev, "enable pinctrl reset assert failed\n");
			return ret;
		}

		ret = clk_prepare_enable(gate);
		if (ret) {
			dev_err(dev, "enable pinctrl clk failed\n");
			return ret;
		}
	}

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	pctldrv->regs_base = devm_ioremap_resource(dev, res);
	if (IS_ERR(pctldrv->regs_base)) {
		dev_err(dev, "get io mem failed\n");
		return PTR_ERR(pctldrv->regs_base);
	}
	/* dev_info(dev, "base: 0x%llx\n",
	 (unsigned long long)pctldrv->regs_base); */

	res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (res)
		pctldrv->irq = res->start;

	ret = lombo_gpiolib_register(pdev, pctldrv);
	if (ret)
		return ret;

	ret = lombo_pinctrl_register(pdev, pctldrv);
	if (ret) {
		lombo_gpiolib_unregister(pdev, pctldrv);
		return ret;
	}

	/*gpio interrupt init*/
	if (ctrl->eint_gpio_init)
		ctrl->eint_gpio_init(pctldrv);

	platform_set_drvdata(pdev, pctldrv);

	return 0;
}
EXPORT_SYMBOL_GPL(lombo_pinctrl_probe);
