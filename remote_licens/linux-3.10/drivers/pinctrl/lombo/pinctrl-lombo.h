/*
 * Lombo's Soc pinctrl driver
 *
 * Copyright(c)2018 Lombo Tech Co.Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef PINCTRL_LOMBO_H
#define PINCTRL_LOMBO_H

#include <linux/pinctrl/pinctrl.h>
#include <linux/pinctrl/pinmux.h>
#include <linux/pinctrl/pinconf.h>
#include <linux/pinctrl/consumer.h>
#include <linux/pinctrl/machine.h>
#include <mach/common.h>
#include <linux/gpio.h>

/**
 * bank registers number define.
 * 4 function + 4 pull up/down + 4 driver + 1 data + 1 pull up/down resistor
 */
#define BANK_REGISTERS_NUM 14

/* registers address step*/
#define ADDR_STEP	0x4

/* register bit width */
#define REG_BIT_WIDTH	32

/* function number for pin as gpio output */
#define FUN_INPUT	0x1
#define FUN_OUTPUT	0x2

/**
 * enum pincfg_type - pin configuration types supported.
 * @PINCFG_TYPE_FUNC: Function configuration.
 * @PINCFG_TYPE_PUD: Pull up/down configuration.
 * @PINCFG_TYPE_DRV: Drive strength configuration.
 * @PINCFG_TYPE_DAT: Pin value configuration.
 * @PINCFG_TYPE_PUD_REG: Pull up/down resistor configuration.
 */
enum pincfg_type {
	PINCFG_TYPE_FUNC,
	PINCFG_TYPE_PUD,
	PINCFG_TYPE_DRV,
	PINCFG_TYPE_DAT,
	PINCFG_TYPE_PUD_REG,

	PINCFG_TYPE_NUM
};

/**
 * enum eint_type - bank external interrupt types
 * @EINT_TYPE_NONE: bank does not support external interrupts
 * @EINT_TYPE_GPIO: bank supportes external gpio interrupts
 * @EINT_TYPE_WKUP: bank supportes external wakeup interrupts
 */
enum eint_type {
	EINT_TYPE_NONE,
	EINT_TYPE_GPIO,
	EINT_TYPE_WKUP,
};

/**
 * enum eint_trig_type - bank external interrupt trig types
 * @EINT_TRIG_LOW_LEVEL: eint trigged by low level signals
 * @EINT_TRIG_HIGH_LEVEL: eint trigged by high level signals
 * @EINT_TRIG_RISING_EDGE: eint trigged by rising edge signals
 * @EINT_TRIG_FALLING_EDGE: eint trigged by falling edge signals
 * @EINT_TRIG_BOTH_EDGE: eint trigged by both rising and falling
 * These bits' arrangement are agree with the spec. For example,
 * EINT_TRIG_HIGH_LEVEL's value is 1, which indicate triggered by high level in
 * GPIO_External_Interrupt_Trigger_Configuration registers
 */
enum eint_trig_type {
	EINT_TRIG_LOW_LEVEL,
	EINT_TRIG_HIGH_LEVEL,
	EINT_TRIG_RISING_EDGE,
	EINT_TRIG_FALLING_EDGE,
	EINT_TRIG_BOTH_EDGE
};

/*
 * pin configuration (pull up/down and drive strength) type and its value are
 * packed together into a 16-bits. The upper 8-bits represent the configuration
 * type and the lower 8-bits hold the value of the configuration type.
 */
#define PINCFG_TYPE_MASK		0xFF
#define PINCFG_VALUE_SHIFT		8
#define PINCFG_VALUE_MASK		(0xFF << PINCFG_VALUE_SHIFT)
#define PINCFG_PACK(type, value)	(((value) << PINCFG_VALUE_SHIFT) | type)
#define PINCFG_UNPACK_TYPE(cfg)		((cfg) & PINCFG_TYPE_MASK)
#define PINCFG_UNPACK_VALUE(cfg)	(((cfg) & PINCFG_VALUE_MASK) >> \
						PINCFG_VALUE_SHIFT)


/**
 * struct lombo_pinctrl_drv_data: Lombo pin controller driver data
 * @node: global list node
 * @regs: register base address of pin controller.
 * @dev: device instance representing the controller.
 * @irq: interrpt number used by the controller to notify gpio interrupts.
 * @pctl: pin controller descriptor registered with the pinctrl subsystem.
 * @pctl_dev: cookie representing pinctrl device instance.
 * @groups: list of pin groups available to the driver.
 * @ngroups: number of such pin groups.
 * @functions: list of pin functions available to the driver.
 * @nfunction: number of such pin functions.
 * @pin_banks: list of pin banks available in the controller.
 * @nbanks: number of sunch pin banks.
 * @pin_base: starting system wide pin number.
 * @nr_pins: number of pins supported by the controller.
 */
struct lombo_pinctrl_drv_data {
	struct list_head		node;
	void __iomem			*regs_base;
	struct device			*dev;
	int				irq;

	struct pinctrl_desc		pctl_desc;
	struct pinctrl_dev		*pctl_dev;

	const struct lombo_pin_group	*groups;
	unsigned int			ngroups;
	const struct lombo_pmx_func	*functions;
	unsigned int			nfunctions;

	struct lombo_pin_bank		*pin_banks;
	u32				nbanks;
	unsigned int			pin_base;
	unsigned int			npins;
};

/**
 * struct lombo_pin_group: group of pins of a pinmux function
 * @name: name of the pin group, used to lookup the group.
 * @pins: the pins included in this group.
 * @npins: number of pins included in this group.
 * @func: the function number to be programmed when selected.
 */
struct lombo_pin_group {
	const char		*name;
	const unsigned int	*pins;
	u8			npins;
	u8			func;
};

/**
 * struct samsung_pmx_func: a pin function.
 * @name: name of the pin function, used to lookup the function.
 * @groups: one or more names of pin groups that provide this function.
 * @ngroups: number of groups included in @groups.
 * @val: the function value to be set.
 */
struct lombo_pmx_func {
	const char		*name;
	const char		**groups;
	u8			ngroups;
	u32			val;
};

/**
 * struct lombo_pin_bank: represent a controller pin-bank.
 * @type: type of the bank (register offsets and bitfield widths)
 * @pctl_offset: starting offset of the pin-bank registers.
 * @nr_pins: number of pins included in this bank.
 * @eint_func: function to set in CON register to configure pin as EINT.
 * @eint_type: type of the external interrupt supported by the bank.
 * @eint_mask: bit mask of pins which support EINT function.
 * @eint_offset: SoC-specific EINT register or interrupt offset of bank.
 * @name: name to be prefixed for each pin in this pin bank.
 * @pin_base: starting pin number of the bank.
 * @soc_priv: per-bank private data for SoC-specific code.
 * @of_node: OF node of the bank.
 * @drvdata: link to controller driver data
 * @irq_domain: IRQ domain of the bank.
 * @gpio_chip: GPIO chip of the bank.
 * @grange: linux gpio pin range supported by this bank.
 * @irq_chip: link to irq chip for external gpio and wakeup interrupts.
 * @slock: spinlock protecting bank registers
 * @pm_save: saved register values during suspend
 */
struct lombo_pin_bank {
	const struct lombo_pin_bank_type	*type;
	u32				pctl_offset;
	u8				npins;
	u8				eint_func;
	enum eint_type			eint_type;
	u32				eint_mask;
	u32				eint_offset;
	const char			*name;

	u32				pin_base;
	void				*soc_priv;
	struct device_node		*of_node;
	struct lombo_pinctrl_drv_data	*pctldrv;
	int				irq;
	struct irq_domain		*irq_domain;
	struct gpio_chip		gpio_chip;
	struct pinctrl_gpio_range	grange;
	spinlock_t			slock;

	u32				pm_save[BANK_REGISTERS_NUM];
};

/* maximum length of a pin in pin descriptor (example: "gpa0") */
#define PIN_NAME_LENGTH	8

/**
 * struct lombo_pin_bank_type: pin bank type description
 * @fld_width: widths of config bitfields
 * @reg_offset: offsets of registers
 */
struct lombo_pin_bank_type {
	u8 fld_width[PINCFG_TYPE_NUM];
	u8 reg_offset[PINCFG_TYPE_NUM];
};

/**
 * struct lombo_pin_bank_data: represent a controller pin-bank (init data).
 * @type: type of the bank (register offsets and bitfield widths)
 * @pctl_offset: starting offset of the pin-bank registers.
 * @nr_pins: number of pins included in this bank.
 * @eint_func: function to set in CON register to configure pin as EINT.
 * @eint_type: type of the external interrupt supported by the bank.
 * @eint_mask: bit mask of pins which support EINT function.
 * @eint_offset: SoC-specific EINT register or interrupt offset of bank.
 * @name: name to be prefixed for each pin in this pin bank.
 */
struct lombo_pin_bank_data {
	const struct lombo_pin_bank_type *type;
	u32		pctl_offset;
	u8		npins;
	u8		eint_func;
	enum eint_type	eint_type;
	u32		eint_mask;
	u32		eint_offset;
	const char	*name;
};

/**
 * struct lombo_pin_ctrl: represent a pin controller.
 * @pin_banks: list of pin banks included in this controller.
 * @nbanks: number of pin banks.
 * @eint_gpio_init: callback to setup the external gpio interrupts controller.
 * @eint_wkup_init: callback to setup the external wakeup interrupts controller.
 */
struct lombo_pin_ctrl {
	const struct lombo_pin_bank_data *pin_banks;
	u32		nbanks;

	int		(*eint_gpio_init)(struct lombo_pinctrl_drv_data *);
	int		(*eint_wkup_init)(struct lombo_pinctrl_drv_data *);
};

/* function */
int lombo_pinctrl_probe(struct platform_device *pdev,
			struct lombo_pin_ctrl *data);

#endif /* PINCTRL_LOMBO_H */
