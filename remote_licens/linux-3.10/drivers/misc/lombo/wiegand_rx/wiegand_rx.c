/*
 * wiegand_rx.c - Common code for LomboTech Socs
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
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/time.h>
#include <linux/device.h>
#include <linux/semaphore.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/of_platform.h>
#include <linux/of_irq.h>
#include <linux/interrupt.h>
#include <linux/of_gpio.h>
#include <linux/slab.h>
#include <linux/workqueue.h>
#include <linux/module.h>
#include <linux/err.h>
#include <linux/input.h>
#include <linux/kthread.h>
#include <linux/poll.h>

#define WIEGAND_MAJOR	243
#define DEVICE_NAME	"wiegand"

#define IRQ_NUM1	"irq_num1"
#define IRQ_NUM2	"irq_num2"

/* delay 33ms to convert data */
#define TIMER_DELAY	(HZ / 8)

bool TIMER_KICK_FLAG;

static int wiegand_flag = 0;
static int g_cardno;
static int convert_flag = 0;
static int poll_flag = 0;
struct wiegand_rx_dev {
	char wiegand[34];
	int hid;		/* hidden id */
	int pid;		/* public id */
	int count;
	struct device *dev;
	struct cdev cdev;
	struct class *cdev_class;

	struct timer_list wiegand_timer;

	int gpio_num1;
	int gpio_num2;
	int d0_irq;
	int d1_irq;
};

static struct wiegand_rx_dev *card;
wait_queue_head_t wiegand_rx_wait;
struct task_struct *wiegand_rx_thread = NULL;
DECLARE_WAIT_QUEUE_HEAD(wiegand_rx_wait);

static int convert_data(void)
{
	int i, even, odd;
	if (card->count == 34) {
		/* parity check, bit[1~16] */
		even = 0;
		for (i = 1; i < 17; i++) {
			if (card->wiegand[i] == 1)
				even = (~even) & 0x01;
		}
		if (even != card->wiegand[0]) {
			card->count = 0;
			goto error;
		}

		/* parity check, bit[17~32] */
		odd = 1;
		for (i = 17; i < 33; i++) {
			if (card->wiegand[i] == 1)
				odd = (~odd) & 0x01;
		}
		if (odd != card->wiegand[33]) {
			card->count = 0;
			goto error;
		}
		card->hid = 0;
		for (i = 1; i <= 16; i++)
			card->hid = card->hid << 1 | card->wiegand[i];
		card->pid = 0;
		for (i = 17; i <= 32; i++)
			card->pid = card->pid << 1 | card->wiegand[i];
		g_cardno = card->hid << 16 | card->pid;
		poll_flag = 1;
		wiegand_flag = card->count;
		card->count = 0;
		return 0;

	} else if (card->count == 26) {
		/* parity check, bit[1~12] */
		even = 0;
		for (i = 1; i < 13; i++) {
			if (card->wiegand[i] == 1)
				even = (~even) & 0x01;
		}
		if (even != card->wiegand[0]) {
			card->count = 0;
			goto error;
		}

		/* bit[13~24] */
		odd = 1;
		for (i = 13; i < 25; i++) {
			if (card->wiegand[i] == 1)
				odd = (~odd) & 0x01;
		}
		if (odd != card->wiegand[25]) {
			card->count = 0;
			goto error;
		}

		card->hid = 0;
		for (i = 1; i <= 8; i++)
			card->hid = card->hid << 1 | card->wiegand[i];

		card->pid = 0;
		for (i = 9; i <= 24; i++)
			card->pid = card->pid << 1 | card->wiegand[i];

		g_cardno = card->hid << 16 | card->pid;
		poll_flag = 1;
		wiegand_flag = card->count;
		card->count = 0;
		return 0;
	} else
		goto error;
error:
	pr_err("Parity Efficacy Error!\n");
	return -1;
}
static void conevert_data_handler(void *data)
{
	struct sched_param param = { .sched_priority = 1 };
	sched_setscheduler(current, SCHED_RR, &param);
	while (!kthread_should_stop()) {
		set_current_state(TASK_INTERRUPTIBLE);
		wait_event_interruptible(wiegand_rx_wait,convert_flag != 0);
		convert_flag = 0;
		set_current_state(TASK_RUNNING);
		disable_irq(card->d0_irq);
		disable_irq(card->d1_irq);
		convert_data();
		enable_irq(card->d0_irq);
		enable_irq(card->d1_irq);
		TIMER_KICK_FLAG = false;
	}
}

static void wiegand_do_timer(unsigned long arg)
{
	convert_flag = 1;
	wake_up_interruptible(&wiegand_rx_wait);
}

static irqreturn_t wiegand_rx_d0_irq_handler(int irq, void *dev_id)
{
	disable_irq_nosync(card->d0_irq);
	/* (gpio_get_value(card->gpio_d0) == 0) */
	{
		card->wiegand[card->count] = 0;
		card->count++;
	}

	enable_irq(card->d0_irq);

	if (TIMER_KICK_FLAG == false) {
		card->wiegand_timer.expires = jiffies + TIMER_DELAY;
		add_timer(&card->wiegand_timer);
		TIMER_KICK_FLAG = true;
	}

	return IRQ_HANDLED;
}

static irqreturn_t wiegand_rx_d1_irq_handler(int irq, void *dev_id)
{
	disable_irq_nosync(card->d1_irq);
	/* if(gpio_get_value(card->gpio_d1) == 0 ) */
	{
		card->wiegand[card->count] = 1;
		card->count++;
	}

	enable_irq(card->d1_irq);

	if (TIMER_KICK_FLAG == false) {
		card->wiegand_timer.expires = jiffies + TIMER_DELAY;
		add_timer(&card->wiegand_timer);
		TIMER_KICK_FLAG = true;
	}
	return IRQ_HANDLED;
}

static void free_irqs(void)
{
	free_irq(card->d0_irq, card);
	free_irq(card->d1_irq, card);
}

static ssize_t Wiegand_rx_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%x\n", g_cardno);
}

static ssize_t Wiegand_rx_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	int err;
	unsigned long val;

	err = kstrtoul(buf, 10, &val);
	if (err) {
		pr_err("Wiegand_rx_store--Invalid size\n");
		return err;
	}

	g_cardno = val;
	return count;
}

static DEVICE_ATTR(wiegand_rx, S_IWUSR | S_IWGRP | S_IWOTH | S_IRUGO,
	Wiegand_rx_show, Wiegand_rx_store);

static struct attribute *wiegand_rx_attributes[] = {
	&dev_attr_wiegand_rx.attr,
	NULL
};

static struct attribute_group wiegand_rx_attribute_group = {
	.attrs = wiegand_rx_attributes
};

static int wiegand_rx_probe(struct platform_device *pdev)
{
	int ret;
	struct wiegand_rx_dev *tgpio_data;
	struct device_node *np = pdev->dev.of_node;

	dev_info(&pdev->dev, "wiegand_rx_probe enter\n");
	tgpio_data = devm_kzalloc(&pdev->dev, sizeof(*tgpio_data), GFP_KERNEL);
	if (!tgpio_data) {
		dev_err(&pdev->dev, "alloc wiegand_rx_dev failed\n");
		return -ENOMEM;
	}

	tgpio_data->gpio_num1 = of_get_named_gpio(np, IRQ_NUM1, 0);
	if (tgpio_data->gpio_num1 < 0) {
		dev_info(&pdev->dev, "%s property not found %d\n",
					IRQ_NUM1, __LINE__);
		tgpio_data->gpio_num1 = 0;
	}

	tgpio_data->gpio_num2 = of_get_named_gpio(np, IRQ_NUM2, 0);
	if (tgpio_data->gpio_num2 < 0) {
		dev_info(&pdev->dev, "%s property not found %d\n",
					IRQ_NUM2, __LINE__);
		tgpio_data->gpio_num2 = 0;
	}

	dev_info(&pdev->dev, "d0_irq gpio: %d, d1_irq gpio: %d\n",
	     tgpio_data->gpio_num1, tgpio_data->gpio_num2);


	ret = devm_gpio_request_one(&pdev->dev, tgpio_data->gpio_num1,
				    GPIOF_IN, "wiegand_rx-d0_irq");
	if (ret < 0) {
		dev_err(&pdev->dev, "request %d fail %d\n",
				tgpio_data->gpio_num1, __LINE__);
		return ret;
	}

	ret = devm_gpio_request_one(&pdev->dev, tgpio_data->gpio_num2,
				    GPIOF_IN, "wiegand_rx-d1_irq");
	if (ret < 0) {
		dev_err(&pdev->dev, "request %d fail %d\n",
				tgpio_data->gpio_num2, __LINE__);
		return ret;
	}

	tgpio_data->d0_irq = gpio_to_irq(tgpio_data->gpio_num1);
	if (tgpio_data->d0_irq < 0) {
		dev_err(&pdev->dev, "new get irq %d fail %d\n",
				tgpio_data->d0_irq, __LINE__);
		return -EINVAL;
	}

	tgpio_data->d1_irq = gpio_to_irq(tgpio_data->gpio_num2);
	if (tgpio_data->d1_irq < 0) {
		dev_err(&pdev->dev, "get irq %d fail %d\n",
				tgpio_data->d1_irq, __LINE__);
		return -EINVAL;
	}

	ret = request_irq(tgpio_data->d0_irq, wiegand_rx_d0_irq_handler,
			  IRQ_TYPE_EDGE_FALLING, "wiegand_rx_d0_irq_handler",
						card);
	if (unlikely(ret)) {
		dev_err(&pdev->dev, "request d0_irq %d fail %d\n",
				tgpio_data->d0_irq, __LINE__);
		return ret;
	}

	ret = request_irq(tgpio_data->d1_irq, wiegand_rx_d1_irq_handler,
			  IRQ_TYPE_EDGE_FALLING, "wiegand_rx_d1_irq_handler",
						card);
	if (unlikely(ret)) {
		dev_err(&pdev->dev, "request d1_irq %d fail\n",
						tgpio_data->d1_irq);
		return ret;
	}
	
	wiegand_rx_thread = kthread_run(conevert_data_handler, 0, "wiegand_rx");
	if (IS_ERR(wiegand_rx_thread))
		return (int)PTR_ERR(wiegand_rx_thread);

	platform_set_drvdata(pdev, tgpio_data);

	ret = sysfs_create_group(&pdev->dev.kobj, &wiegand_rx_attribute_group);
	if (ret) {
		dev_err(&pdev->dev, "sysfs_create_group fail %d\n",
						tgpio_data->d1_irq);
		return ret;
	}
	if (card != NULL) {
		card->d0_irq = tgpio_data->d0_irq;
		card->d1_irq = tgpio_data->d1_irq;
	}
	return 0;
}

static int wiegand_rx_remove(struct platform_device *pdev)
{
	struct wiegand_rx_dev *tgpio_data = platform_get_drvdata(pdev);

	devm_gpio_free(&pdev->dev, tgpio_data->gpio_num1);
	devm_gpio_free(&pdev->dev, tgpio_data->gpio_num2);

	return 0;
}

static ssize_t rfcd_read(struct file *filp, char __user *buf,
					size_t size, loff_t *ppos)
{
	char cardno[5];
	cardno[4] = wiegand_flag;
	cardno[3] = g_cardno & 0xff;
	g_cardno >>= 8;
	cardno[2] = g_cardno & 0xff;
	g_cardno >>= 8;
	cardno[1] = g_cardno & 0xff;
	g_cardno >>= 8;
	cardno[0] = g_cardno & 0xff;
	g_cardno = -1;
	wiegand_flag = 0;
	if (copy_to_user(buf, cardno, size))
		return -EFAULT;
	poll_flag = 0;
	return size;
}
static unsigned int rfcd_poll(struct file *filp, struct poll_table_struct *wait)
{
	unsigned int mask = 0;
	poll_wait(filp,&wiegand_rx_wait,wait);
	if(poll_flag)
		mask |= POLLIN | POLLRDBAND;
	return mask;
}
static ssize_t rfcd_write(struct file *filp, const char __user *buf,
					size_t size, loff_t *ppos)
{
	return 0;
}

static int rfcd_open(struct inode *inode, struct file *filp)
{
	return 0;
}

int rfcd_release(struct inode *inode, struct file *filp)
{
	return 0;
}

static const struct file_operations rfcd_fops = {
	.owner = THIS_MODULE,
	.read = rfcd_read,
	.poll = rfcd_poll,
	.write = rfcd_write,
	.open = rfcd_open,
	.release = rfcd_release,
};

static const struct of_device_id of_wiegand_rx_match[] = {
	{ .compatible = "wiegand_rx", },
	{},
};

static struct platform_driver wiegand_rx_driver = {
	.probe		= wiegand_rx_probe,
	.remove		= wiegand_rx_remove,
	.driver		= {
		.name	= "wiegand_rx",
		.owner	= THIS_MODULE,
		.of_match_table = of_match_ptr(of_wiegand_rx_match),
	},
};

static int __init wiegand_rx_init(void)
{
	dev_t devno = MKDEV(WIEGAND_MAJOR, 1);
	int ret = -1, result;

	if (WIEGAND_MAJOR)
		result = register_chrdev_region(devno, 1, DEVICE_NAME);
	else
		result = alloc_chrdev_region(&devno, 0, 1, DEVICE_NAME);
	if (result < 0) {
		pr_err("%s:register_chrdev_region error\n", __func__);
		return result;
	}

	card = kmalloc(sizeof(struct wiegand_rx_dev), GFP_KERNEL);
	if (!card) {
		result = -ENOMEM;
		pr_err("wiegand_rx_dev platform kmalloc error!\n");
		goto err0;
	}
	memset(card, 0, sizeof(struct wiegand_rx_dev));
	card->count = 0;

	cdev_init(&(card->cdev), &rfcd_fops);
	card->cdev.owner = THIS_MODULE;
	ret = cdev_add(&card->cdev, devno, 1);
	if (ret) {
		pr_err("cdev add wiegand err\r\n");
		goto err1;
	}

	card->cdev_class = class_create(THIS_MODULE, DEVICE_NAME);
	if (IS_ERR(card->cdev_class)) {
		pr_err("ERR:cannot create a cdev_class\n");
		goto err1;
	}
	device_create(card->cdev_class, NULL, devno, 0, DEVICE_NAME);

	ret = platform_driver_register(&wiegand_rx_driver);
	if (ret) {
		pr_err("register wiegand_rx_driver platform driver  error!\n");
		goto err2;
	}

	setup_timer(&card->wiegand_timer, wiegand_do_timer, 0);

	pr_info("wiegand_rx_init ok!\n");
	return 0;

err2:
	free_irqs();
err1:
	kfree(card);
err0:
	unregister_chrdev_region(devno, 1);

	return ret;
}

static void __exit wiegand_rx_exit(void)
{
	platform_driver_unregister(&wiegand_rx_driver);
}

module_init(wiegand_rx_init);
module_exit(wiegand_rx_exit);

MODULE_AUTHOR("lombo@com");
MODULE_LICENSE("GPL");

