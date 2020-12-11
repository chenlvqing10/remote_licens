#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/of_platform.h>
#include <linux/of_gpio.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/input.h>
#include <linux/interrupt.h>

#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/timer.h>
#include <linux/delay.h>

#include <linux/pinctrl/consumer.h>
#include <linux/err.h>
#include <linux/input/mt.h>

#define DRIVER_NAME "gpio-alarm"

struct timer_list alarm_timer;
struct input_dev *key_alarm;
static int timer_flag = 0;

struct class *gpio_in_class;
struct device *gpio_in_cmd_dev;
static dev_t gpio_in_devno;
static struct cdev gpio_in_cdev;
static unsigned mygpio;
static const struct of_device_id of_gpio_in_match[] = {
	{
		.compatible = DRIVER_NAME,
	},
	{ /* sentinel */},
};

static ssize_t gpio_in_value_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%u\n", gpio_get_value(mygpio));

	return 0;
}

static ssize_t gpio_in_value_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t size)
{
	unsigned long val;
	int err;
	err = kstrtoul(buf, 10, &val);
	if (err) {
		pr_err("gpio_out_value_store--Invalid size\n");
		return err;
	}
	if (val != 0)
		gpio_direction_output(mygpio,1);
	else
		gpio_direction_output(mygpio,0);

	return size;
}

static struct file_operations gpio_in_fops = {
    .owner      =   THIS_MODULE
};

static struct device_attribute gpio_in_class_attrs[] = {
	__ATTR(value, 0644, gpio_in_value_show ,gpio_in_value_store),
	__ATTR_NULL,
};

static void alarm_do_timer(unsigned long arg)
{
	unsigned int val;
	val = gpio_get_value(mygpio);
	if (val != timer_flag) {
		input_report_key(key_alarm, BTN_1, val);
		input_sync(key_alarm);
		timer_flag = val;
	}
	mod_timer(&alarm_timer, jiffies +(HZ *1));
}

static int gpio_in_create_of(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node, *child;
	int *gpio_num;
	int count = 0;
	int ret = 0;
	count = of_get_child_count(np);
	if (!count)
		return -1;
	gpio_num = devm_kzalloc(&pdev->dev, sizeof(gpio_num)*count,GFP_KERNEL);
	for_each_child_of_node(np, child)
		if (of_get_gpio(child, 0) == -EPROBE_DEFER)
			return -1;

	for_each_child_of_node(np, child) {
		const char *gpio_in_name;
		enum of_gpio_flags flags;
		*gpio_num = of_get_gpio_flags(child, 0, &flags);
		if (!gpio_is_valid(*gpio_num)) {
			dev_warn(&pdev->dev,"Skipping unavailable gpio %d \n",*gpio_num);
			return 0;
		}

		gpio_in_name = of_get_property(child, "label", NULL) ? : child->name;
		ret = gpio_request(*gpio_num, gpio_in_name);
		if (ret < 0)
			return ret;

		ret = gpio_direction_input(*gpio_num);
		if (ret < 0)
			return ret;
		mygpio = *gpio_num;
		device_create(gpio_in_class, NULL, 0, NULL, gpio_in_name);
		timer_flag = gpio_get_value(mygpio);
		if(timer_flag) {
				input_report_key(key_alarm, BTN_1, 1);
				input_sync(key_alarm);
		}
		gpio_num++;
	}
	return 0;

}

static int gpio_in_probe(struct platform_device *pdev)
{
	struct pinctrl *pinctrl;
	int ret = 0;
	alloc_chrdev_region(&gpio_in_devno, 0, 1, DRIVER_NAME);
	cdev_add(&gpio_in_cdev, gpio_in_devno, 1);
	cdev_init(&gpio_in_cdev, &gpio_in_fops);
	gpio_in_class = class_create(THIS_MODULE, DRIVER_NAME);
	gpio_in_class->dev_attrs = gpio_in_class_attrs;
	pinctrl = devm_pinctrl_get_select_default(&pdev->dev);
	if (IS_ERR(pinctrl))
		dev_warn(&pdev->dev,"pins are not configured from the driver\n");

	key_alarm = input_allocate_device();
	if (!key_alarm)
		return -ENOMEM;
	key_alarm->evbit[0] = BIT_MASK(EV_SYN) | BIT_MASK(EV_KEY);
	key_alarm->keybit[BIT_WORD(BTN_1)] |= BIT_MASK(BTN_1);
	key_alarm->name = "key_alarm";
	key_alarm->phys = "gpio-keys/input0";
	ret = input_register_device(key_alarm);
	if (ret) {
		pr_err("%s:%d: input_register faild\n",__func__, __LINE__);
		input_free_device(key_alarm);
		return ret;
	}

	setup_timer(&alarm_timer, alarm_do_timer, 0);
	init_timer(&alarm_timer);
	alarm_timer.function = &alarm_do_timer;
	alarm_timer.expires = jiffies +(HZ *1);
	add_timer(&alarm_timer);

	/* Analyzing Lebel form  DTS*/
	ret = gpio_in_create_of(pdev);
	if (!ret) {
		dev_warn(&pdev->dev,"Node registered failed!!! the ret is %d \n",ret);
		return -1;
	}

	return 0;
}

static struct platform_driver gpio_in_driver = {
	.probe		= gpio_in_probe,
	.driver		= {
	        .name	= DRIVER_NAME,
	        .owner	= THIS_MODULE,
	        .of_match_table = of_match_ptr(of_gpio_in_match),
	}
};

static int __init gpio_in_init(void)
{
	int ret = 0;
	ret = platform_driver_register(&gpio_in_driver);
	return ret;
}

static void gpio_in_exit(void)
{
	cdev_del(&gpio_in_cdev);
	platform_driver_unregister(&gpio_in_driver);
	device_destroy(gpio_in_class , gpio_in_devno);
	class_destroy(gpio_in_class);
	del_timer(&alarm_timer);
	input_unregister_device(key_alarm);
}

module_init(gpio_in_init);
module_exit(gpio_in_exit);
MODULE_AUTHOR("lomboswer@lombotech.com");
MODULE_DESCRIPTION("LomboTech N7 gpio in Driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:gpio-in");

