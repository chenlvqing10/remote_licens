/*
 * A simple sysfs interface for the generic PWM framework
 *
 * Copyright (C) 2013 H Hartley Sweeten <hsweeten@visionengravers.com>
 *
 * Based on previous work by Lars Poeschel <poeschel@lemonage.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/device.h>
#include <linux/mutex.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/kdev_t.h>
#include <linux/pwm.h>
#include <linux/sysfs.h>
#include <mach/debug.h>
#include "lombo/pwm.h"

static struct class pwm_class;

struct pwm_export {
	struct device child;
	struct pwm_device *pwm;
	struct mutex lock;
};

static struct pwm_export *child_to_pwm_export(struct device *child)
{
	return container_of(child, struct pwm_export, child);
}

static struct pwm_device *child_to_pwm_device(struct device *child)
{
	struct pwm_export *export = child_to_pwm_export(child);

	return export->pwm;
}

static ssize_t period_show(struct device *child,
			   struct device_attribute *attr,
			   char *buf)
{
	struct pwm_device *pwm = child_to_pwm_device(child);
	struct pwm_data *pdata = pwm_get_chip_data(pwm);

	return sprintf(buf, "%u\n", pdata->period_ns);
}

static ssize_t period_store(struct device *child,
			    struct device_attribute *attr,
			    const char *buf, size_t size)
{
	struct pwm_export *export = child_to_pwm_export(child);
	struct pwm_device *pwm = export->pwm;
	struct pwm_data *pdata = pwm_get_chip_data(pwm);
	unsigned int val;
	int ret;

	ret = kstrtouint(buf, 0, &val);
	if (ret)
		return ret;

	mutex_lock(&export->lock);
	ret = pwm_config(pwm, pdata->duty_ns, val);
	mutex_unlock(&export->lock);

	return ret ? : size;
}

static ssize_t duty_cycle_show(struct device *child,
				struct device_attribute *attr, char *buf)
{
	struct pwm_device *pwm = child_to_pwm_device(child);
	struct pwm_data *pdata = pwm_get_chip_data(pwm);

	return sprintf(buf, "%u\n", pdata->duty_ns);
}

static ssize_t duty_cycle_store(struct device *child,
				struct device_attribute *attr,
				const char *buf, size_t size)
{
	struct pwm_export *export = child_to_pwm_export(child);
	struct pwm_device *pwm = export->pwm;
	struct pwm_data *pdata = pwm_get_chip_data(pwm);
	unsigned int val;
	int ret;

	ret = kstrtouint(buf, 0, &val);
	if (ret)
		return ret;

	mutex_lock(&export->lock);
	ret = pwm_config(pwm, val, pdata->period_ns);
	mutex_unlock(&export->lock);

	return size;
}

static ssize_t enable_show(struct device *child,
			   struct device_attribute *attr,
			   char *buf)
{
	struct pwm_device *pwm = child_to_pwm_device(child);
	char *en;

	if (pwm && test_and_set_bit(PWMF_ENABLED, &pwm->flags))
		en = "enabled";
	else
		en = "disabled";
	return sprintf(buf, "%s\n", en);
}

static ssize_t enable_store(struct device *child,
			    struct device_attribute *attr,
			    const char *buf, size_t size)
{
	struct pwm_export *export = child_to_pwm_export(child);
	struct pwm_device *pwm = export->pwm;
	int val;
	int ret = 0;

	ret = kstrtoint(buf, 0, &val);
	if (ret)
		return ret;

	mutex_lock(&export->lock);

	switch (val) {
	case 0:
		pwm_disable(pwm);
		break;
	case 1:
		ret = pwm_enable(pwm);
		break;
	default:
		ret = -EINVAL;
		goto unlock;
	}

unlock:
	mutex_unlock(&export->lock);
	return ret ? : size;
}

static ssize_t polarity_show(struct device *child,
			     struct device_attribute *attr,
			     char *buf)
{
	struct pwm_device *pwm = child_to_pwm_device(child);
	const char *polarity = "unknown";
	struct pwm_data *pdata = pwm_get_chip_data(pwm);

	switch (pdata->polarity) {
	case PWM_POLARITY_NORMAL:
		polarity = "normal";
		break;

	case PWM_POLARITY_INVERSED:
		polarity = "inversed";
		break;
	}

	return sprintf(buf, "%s\n", polarity);
}

static ssize_t polarity_store(struct device *child,
			      struct device_attribute *attr,
			      const char *buf, size_t size)
{
	struct pwm_export *export = child_to_pwm_export(child);
	struct pwm_device *pwm = export->pwm;
	struct pwm_data *pdata = pwm_get_chip_data(pwm);
	enum pwm_polarity polarity;
	int ret;

	if (sysfs_streq(buf, "normal"))
		polarity = PWM_POLARITY_NORMAL;
	else if (sysfs_streq(buf, "inversed"))
		polarity = PWM_POLARITY_INVERSED;
	else
		return -EINVAL;

	mutex_lock(&export->lock);
	if (pdata->polarity == polarity) {
		PRT_ERR("polarity had been set to %s\n", buf);
		return size;
	} else
		ret = pwm_set_polarity(pwm, polarity);
	mutex_unlock(&export->lock);

	return ret ? : size;
}

static DEVICE_ATTR_RW(period);
static DEVICE_ATTR_RW(duty_cycle);
static DEVICE_ATTR_RW(enable);
static DEVICE_ATTR_RW(polarity);

static struct attribute *pwm_attrs[] = {
	&dev_attr_period.attr,
	&dev_attr_duty_cycle.attr,
	&dev_attr_enable.attr,
	&dev_attr_polarity.attr,
	NULL
};

static const struct attribute_group pwm_attr_group = {
	.attrs	= pwm_attrs,
};

static const struct attribute_group *pwm_attr_groups[] = {
	&pwm_attr_group,
	NULL,
};

static void pwm_export_release(struct device *child)
{
	struct pwm_export *export = child_to_pwm_export(child);

	kfree(export);
}

static int pwm_export_child(struct device *parent, struct pwm_device *pwm)
{
	struct pwm_export *export;
	int ret;

	if (test_and_set_bit(PWMF_EXPORTED, &pwm->flags))
		return -EBUSY;

	export = kzalloc(sizeof(*export), GFP_KERNEL);
	if (!export) {
		clear_bit(PWMF_EXPORTED, &pwm->flags);
		return -ENOMEM;
	}

	export->pwm = pwm;
	mutex_init(&export->lock);

	export->child.release = pwm_export_release;
	export->child.parent = parent;
	export->child.devt = MKDEV(0, 0);
	export->child.groups = pwm_attr_groups;
	dev_set_name(&export->child, "pwm%u", pwm->hwpwm);

	ret = device_register(&export->child);
	if (ret) {
		clear_bit(PWMF_EXPORTED, &pwm->flags);
		put_device(&export->child);
		export = NULL;
		return ret;
	}

	return 0;
}

static int pwm_unexport_match(struct device *child, void *data)
{
	return child_to_pwm_device(child) == data;
}

static int pwm_unexport_child(struct device *parent, struct pwm_device *pwm)
{
	struct device *child;

	if (!test_and_clear_bit(PWMF_EXPORTED, &pwm->flags))
		return -ENODEV;

	child = device_find_child(parent, pwm, pwm_unexport_match);
	if (!child)
		return -ENODEV;

	/* for device_find_child() */
	put_device(child);
	device_unregister(child);
	pwm_put(pwm);

	return 0;
}

static ssize_t export_store(struct device *parent,
			    struct device_attribute *attr,
			    const char *buf, size_t len)
{
	struct pwm_chip *chip = dev_get_drvdata(parent);
	struct pwm_device *pwm;
	unsigned int hwpwm;
	int ret;

	ret = kstrtouint(buf, 0, &hwpwm);
	if (ret < 0)
		return ret;

	if (hwpwm >= chip->npwm)
		return -ENODEV;

	pwm = pwm_request_from_chip(chip, hwpwm, "sysfs");
	if (IS_ERR(pwm))
		return PTR_ERR(pwm);

	ret = pwm_export_child(parent, pwm);
	if (ret < 0)
		pwm_put(pwm);

	return ret ? : len;
}

static ssize_t unexport_store(struct device *parent,
			      struct device_attribute *attr,
			      const char *buf, size_t len)
{
	struct pwm_chip *chip = dev_get_drvdata(parent);
	unsigned int hwpwm;
	int ret;

	ret = kstrtouint(buf, 0, &hwpwm);
	if (ret < 0)
		return ret;

	if (hwpwm >= chip->npwm)
		return -ENODEV;

	ret = pwm_unexport_child(parent, &chip->pwms[hwpwm]);

	return ret ? : len;
}

static ssize_t npwm_show(struct device *parent, struct device_attribute *attr,
			 char *buf)
{
	const struct pwm_chip *chip = dev_get_drvdata(parent);

	return sprintf(buf, "%u\n", chip->npwm);
}

static struct device_attribute pwm_chip_attrs[] = {
	__ATTR(export, S_IWUSR, NULL, export_store),
	__ATTR(unexport, S_IWUSR, NULL, unexport_store),
	__ATTR(npwm, S_IRUGO, npwm_show, NULL),
	__ATTR_NULL,
};

static struct class pwm_class = {
	.name = "pwm",
	.owner = THIS_MODULE,
	.dev_attrs = pwm_chip_attrs,
};

static int pwmchip_sysfs_match(struct device *parent, const void *data)
{
	return dev_get_drvdata(parent) == data;
}

void pwmchip_sysfs_export(struct pwm_chip *chip)
{
	struct device *parent;

	/*
	 * If device_create() fails the pwm_chip is still usable by
	 * the kernel its just not exported.
	 */
	parent = device_create(&pwm_class, chip->dev, MKDEV(0, 0), chip,
			       "pwmchip%d", chip->base);
	if (IS_ERR(parent)) {
		dev_warn(chip->dev,
			 "device_create failed for pwm_chip sysfs export\n");
	}
}

void pwmchip_sysfs_unexport(struct pwm_chip *chip)
{
	struct device *parent;
	unsigned int i;

	parent = class_find_device(&pwm_class, NULL, chip,
				   pwmchip_sysfs_match);
	if (!parent)
		return;

	for (i = 0; i < chip->npwm; i++) {
		struct pwm_device *pwm = &chip->pwms[i];

		if (test_bit(PWMF_EXPORTED, &pwm->flags))
			pwm_unexport_child(parent, pwm);
	}

	put_device(parent);
	device_unregister(parent);
}

static int __init pwm_sysfs_init(void)
{
	return class_register(&pwm_class);
}
subsys_initcall(pwm_sysfs_init);
