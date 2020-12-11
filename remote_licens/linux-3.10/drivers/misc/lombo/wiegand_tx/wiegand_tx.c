/*
 * wiegand_tx.c - Common code for LomboTech Socs
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
#include <linux/delay.h>
#include <linux/hrtimer.h>

#define USE_HRTIMER		1
#define WIEGAND_MAJOR		243
#define DEVICE_NAME		"wiegand_tx"

#define GPIO_DATA0		"pin_data0"
#define GPIO_DATA1		"pin_data1"

#define TX_DATA0_STATE		0
#define TX_DATA1_STATE		1

#if USE_HRTIMER
#define MAX_TX_DATA_SIZE	62
#else
#define MAX_TX_DATA_SIZE	64
#endif

#define BIT_LONG(nr)			(1ULL << (nr))

struct wiegand_tx_para {
	int data_size;
	uint64_t cardid;
};

struct wiegand_tx_dev {
	int hid;		/* hidden id */
	int pid;		/* public id */
	int count;
	struct device *dev;
	struct cdev cdev;
	struct class *cdev_class;

#if USE_HRTIMER
	struct completion send_completion;
	struct hrtimer timer;

	int tx_cnt;
	int data_num; /* the total tx data num */
	uint64_t highest_bit;
	uint64_t tx_data;
	uint64_t cardid;
#endif

	int gpio_data0;
	int gpio_data1;
};

static struct wiegand_tx_dev *card;

static void output_io_reset(void)
{
	gpio_set_value(card->gpio_data0, 1);
	gpio_set_value(card->gpio_data1, 1);
}

#if USE_HRTIMER
/* event + data(from hogh to low bits) + odd */
static void wiegand_send_prepare(int data_num, uint64_t cardid)
{
	int i, half;
	int even = 0, odd = 0;
	uint64_t temp;

	/* printk(KERN_DEBUG "Send_Wiegand start\n"); */
	/* printk(KERN_DEBUG "data_num[%d] cardid[%ld]\n", data_num, cardid); */
	half = data_num / 2;
	temp = cardid;
	for (i = 0; i < data_num; i++) {
		if (temp & 0x01) {
			if (i < half)
				odd++;
			else
				even++;
		}
		temp = temp >> 1;
	}

	card->cardid = cardid;
	card->tx_data = cardid;
	if (even % 2)
		card->tx_data |= BIT_LONG(data_num);
	else
		card->tx_data &= ~BIT_LONG(data_num);

	card->tx_data = card->tx_data << 1;

	if (odd % 2)
		card->tx_data &= ~BIT_LONG(0);
	else
		card->tx_data |= BIT_LONG(0);

	card->data_num = data_num + 2; /* 2 is for even and odd */
	card->highest_bit = BIT_LONG(data_num + 1);

#if 0
	pr_err("wiegand_send_prepare data_num[%d]\n", card->data_num);
	pr_err("wiegand_send_prepare highest_bit[%llx]\n", card->highest_bit);
	pr_err("wiegand_send_prepare tx_data[%llx]\n", card->tx_data);
#endif
}

static void wiegand_tx_set_data(int data_sta, int value)
{
	if (data_sta == TX_DATA0_STATE)
		gpio_set_value(card->gpio_data0, value);
	else
		gpio_set_value(card->gpio_data1, value);
}

/* the kernel hrtimer event */
static enum hrtimer_restart wiegand_tx_hrtimer_event(struct hrtimer *timer)
{
	int data_sta;

	if (card->tx_data & card->highest_bit)
		data_sta = TX_DATA1_STATE;
	else
		data_sta = TX_DATA0_STATE;

	if (card->tx_cnt < card->data_num * 2) {
		if (card->tx_cnt % 2 == 0) {
			wiegand_tx_set_data(data_sta, 0);
			hrtimer_forward_now(&card->timer,
					    ktime_set(0, 100*1000));
		} else {
			wiegand_tx_set_data(data_sta, 1);
			hrtimer_forward_now(&card->timer,
					    ktime_set(0, 1500*1000));
			card->tx_data = card->tx_data << 1;
		}
		card->tx_cnt++;
		return HRTIMER_RESTART;
	} else {
		output_io_reset();
		card->tx_cnt = 0;
		complete(&card->send_completion);
		return HRTIMER_NORESTART;
	}
}

static void wiegand_hrtimer_send(void)
{
	output_io_reset();
	mdelay(10);

	card->tx_cnt = 0;

	hrtimer_start(&card->timer, ktime_set(0, 100*1000), HRTIMER_MODE_REL);
}

#else

static void output_data(unsigned int gpio)
{
	gpio_set_value(gpio, 0);
	usleep_range(100, 110);
	gpio_set_value(gpio, 1);
	usleep_range(1500, 1510);
}

static void output_data0(void)
{
	output_data(card->gpio_data0);
}

static void output_data1(void)
{
	output_data(card->gpio_data1);
}

/* event + data(from hogh to low bits) + odd */
static void send_wiegand(int data_num, uint64_t cardid)
{
	int i, half;
	int even = 0, odd = 0;
	uint64_t temp;

	/* printk(KERN_DEBUG "Send_Wiegand start\n"); */
	/* printk(KERN_DEBUG "data_num[%d] cardid[%ld]\n", data_num, cardid); */

	output_io_reset();
	mdelay(10);

	half = data_num / 2;
	temp = cardid;
	for (i = 0; i < data_num; i++) {
		if (temp & 0x01) {
			if (i < half)
				odd++;
			else
				even++;
		}
		temp = temp >> 1;
	}

	if (even % 2)
		output_data1();
	else
		output_data0();

	temp = BIT_LONG(data_num - 1);
	for (i = 0; i < data_num; i++) {
		if (cardid & temp)
			output_data1();
		else
			output_data0();
		cardid = cardid << 1;
	}

	if (odd % 2)
		output_data0();
	else
		output_data1();

	output_io_reset();

	/* printk(KERN_DEBUG "\n Send_Weigand end\n"); */
}
#endif

static ssize_t wiegand_tx_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "test:wiegand%u cardid:%llu\n",
			card->data_num, card->cardid);
}

static ssize_t wiegand_tx_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	int err;
	unsigned long val;
	int data_num;
	uint64_t cardid = 654868;

	err = kstrtoul(buf, 10, &val);
	if (err) {
		pr_err("Wiegand26_tx_store--Invalid size\n");
		return err;
	}

	if (val != 0)
		data_num = 32;
	else
		data_num = 24;

#if USE_HRTIMER
	INIT_COMPLETION(card->send_completion);
	wiegand_send_prepare(data_num, cardid);
	wiegand_hrtimer_send();
	wait_for_completion(&card->send_completion);
#else
	send_wiegand(data_num, cardid);
#endif
	pr_err("test:wiegand%u cardid:%llu\n", data_num + 2, cardid);

	return count;
}

static DEVICE_ATTR(wiegand_tx, S_IWUSR | S_IWGRP | S_IWOTH | S_IRUGO,
				wiegand_tx_show, wiegand_tx_store);

static struct attribute *wiegand_tx_attributes[] = {
	&dev_attr_wiegand_tx.attr,
	NULL
};

static struct attribute_group wiegand_tx_attribute_group = {
	.attrs = wiegand_tx_attributes
};

static int wiegand_tx_probe(struct platform_device *pdev)
{
	int ret;
	struct wiegand_tx_dev *tgpio_data;
	struct device_node *np = pdev->dev.of_node;

	dev_info(&pdev->dev, "wiegand_tx_probe enter\n");
	tgpio_data = devm_kzalloc(&pdev->dev, sizeof(*tgpio_data), GFP_KERNEL);
	if (!tgpio_data) {
		dev_err(&pdev->dev, "alloc wiegand_tx_dev failed\n");
		return -ENOMEM;
	}

	tgpio_data->gpio_data0 = of_get_named_gpio(np, GPIO_DATA0, 0);
	if (tgpio_data->gpio_data0 < 0) {
		dev_info(&pdev->dev, "%s property not found %d\n",
					GPIO_DATA0, __LINE__);
		tgpio_data->gpio_data0 = 0;
	}

	tgpio_data->gpio_data1 = of_get_named_gpio(np, GPIO_DATA1, 0);
	if (tgpio_data->gpio_data1 < 0) {
		dev_info(&pdev->dev, "%s property not found %d\n",
					GPIO_DATA1, __LINE__);
		tgpio_data->gpio_data1 = 0;
	}

	dev_info(&pdev->dev, "data0 gpio: %d, data1 gpio: %d\n",
	     tgpio_data->gpio_data0, tgpio_data->gpio_data1);


	ret = devm_gpio_request_one(&pdev->dev, tgpio_data->gpio_data0,
				    GPIOF_OUT_INIT_HIGH, "wiegand_tx-d0");
	if (ret < 0) {
		dev_err(&pdev->dev, "request %d fail %d\n",
				tgpio_data->gpio_data0, __LINE__);
		return ret;
	}

	ret = devm_gpio_request_one(&pdev->dev, tgpio_data->gpio_data1,
				    GPIOF_OUT_INIT_HIGH, "wiegand_tx-d1");
	if (ret < 0) {
		dev_err(&pdev->dev, "request %d fail %d\n",
				tgpio_data->gpio_data1, __LINE__);
		return ret;
	}

	platform_set_drvdata(pdev, tgpio_data);

	ret = sysfs_create_group(&pdev->dev.kobj, &wiegand_tx_attribute_group);
	if (ret) {
		dev_err(&pdev->dev, "sysfs_create_group fail\n");
		return ret;
	}

	if (card != NULL) {
		card->gpio_data0 = tgpio_data->gpio_data0;
		card->gpio_data1 = tgpio_data->gpio_data1;
	}

	return 0;
}

static int wiegand_tx_remove(struct platform_device *pdev)
{
	struct wiegand_tx_dev *tgpio_data = platform_get_drvdata(pdev);

	devm_gpio_free(&pdev->dev, tgpio_data->gpio_data0);
	devm_gpio_free(&pdev->dev, tgpio_data->gpio_data1);

	return 0;
}

static ssize_t rfcd_read(struct file *filp, char __user *buf,
					size_t size, loff_t *ppos)
{
	return 0;
}

static ssize_t rfcd_write(struct file *filp, const char __user *buf,
					size_t size, loff_t *ppos)
{
	struct wiegand_tx_para tx_para;

	/* printk(KERN_DEBUG "rfcd_write++\n"); */
	if (size != sizeof(struct wiegand_tx_para)) {
		pr_err("para size err\n");
		return -EFAULT;
	}

	if (copy_from_user(&tx_para, buf, size)) {
		pr_err("copy_from_user err\n");
		return -EFAULT;
	}

	if (tx_para.data_size > MAX_TX_DATA_SIZE) {
		pr_err("data size[%d] is over %d\n",
			tx_para.data_size, MAX_TX_DATA_SIZE);
		return -EFAULT;
	}

	if (tx_para.data_size % 2 != 0) {
		pr_err("data size[%d] should be even\n",
			tx_para.data_size);
		return -EFAULT;
	}

#if USE_HRTIMER
	INIT_COMPLETION(card->send_completion);
	wiegand_send_prepare(tx_para.data_size, tx_para.cardid);
	wiegand_hrtimer_send();
	wait_for_completion(&card->send_completion);
#else
	send_wiegand(tx_para.data_size, tx_para.cardid);
#endif
	/* printk(KERN_DEBUG "rfcd_write--\n"); */

	return size;
}

static int rfcd_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static int rfcd_release(struct inode *inode, struct file *filp)
{
	return 0;
}

static const struct file_operations rfcd_fops = {
	.owner = THIS_MODULE,
	.read = rfcd_read,
	.write = rfcd_write,
	.open = rfcd_open,
	.release = rfcd_release,
};

static const struct of_device_id of_wiegand_tx_match[] = {
	{ .compatible = "wiegand_tx", },
	{},
};

static struct platform_driver wiegand_tx_driver = {
	.probe		= wiegand_tx_probe,
	.remove		= wiegand_tx_remove,
	.driver		= {
		.name	= "wiegand_tx",
		.owner	= THIS_MODULE,
		.of_match_table = of_match_ptr(of_wiegand_tx_match),
	},
};

static int __init wiegand_tx_init(void)
{
	dev_t devno = MKDEV(WIEGAND_MAJOR, 2);
	int ret = -1, result;

	if (WIEGAND_MAJOR)
		result = register_chrdev_region(devno, 1, DEVICE_NAME);
	else
		result = alloc_chrdev_region(&devno, 0, 1, DEVICE_NAME);
	if (result < 0) {
		pr_err("%s:register_chrdev_region error\n", __func__);
		return result;
	}

	card = kmalloc(sizeof(struct wiegand_tx_dev), GFP_KERNEL);
	if (!card) {
		result = -ENOMEM;
		pr_err("wiegand_tx_dev platform kmalloc error!\n");
		goto err0;
	}
	memset(card, 0, sizeof(struct wiegand_tx_dev));
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

	ret = platform_driver_register(&wiegand_tx_driver);
	if (ret) {
		pr_err("register wiegand_tx_driver platform driver  error!\n");
		goto err1;
	}

#if USE_HRTIMER
	init_completion(&(card->send_completion));
	hrtimer_init(&card->timer, CLOCK_REALTIME, HRTIMER_MODE_REL);
	card->timer.function = wiegand_tx_hrtimer_event;
#endif

	pr_info("wiegand_tx_init ok!\n");
	return 0;

err1:
	kfree(card);
err0:
	unregister_chrdev_region(devno, 1);

	return ret;
}

static void __exit wiegand_tx_exit(void)
{
	class_destroy(card->cdev_class);
	kfree(card);
	platform_driver_unregister(&wiegand_tx_driver);
}

module_init(wiegand_tx_init);
module_exit(wiegand_tx_exit);

MODULE_AUTHOR("lombo@com");
MODULE_LICENSE("GPL");

