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

#include <linux/pinctrl/consumer.h>
#include <linux/err.h>
 
#define DRIVER_NAME "tp_coordinate"
#define SCREEN_MAX_X  10000
#define SCREEN_MAX_Y  10000

struct class *tp_coordinate_class;
struct device *tp_coordinate_cmd_dev;
static dev_t tp_coordinate_devno;
static struct cdev tp_coordinate_cdev;
static struct input_dev *virtmouse_dev;
static struct vmouse{
	int operate;
	int mx;
	int mx_last;
	int my;
	int my_last;
}vir_mouse_point = {0,0,0};

static int powm(unsigned char x)
{
	int ret = 1;
	unsigned char times = 0;
	for (times = 0; times < x; times++)
		ret = ret*10;
	return ret;
}

static char checknum(char *agv)
{
	unsigned char i, num = 0;
	while (*(agv + num) != '\0') {
		num++;
		if (num >= 100)
			pr_err("Form err: data in wrong form\n");
	}	
	if (num != 10) {
		pr_err("Form err: the number of valid data should be 9\n");
		return -1;
	}	
	for (i = 1; i <= num - 2; i++) {
		if (!(*(agv + i) <= '9' && *(agv + i) >= '0')) {
			pr_err("Form err: the data should be an interger\n");
			return -1;
		}
	}
	
	return 0;
}

static ssize_t tp_coordinate_value_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%s\n","tp_coordinate_value_show\n");
}

static ssize_t tp_coordinate_value_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t size)
{
	int err,eff_firstflag = 0, input_mx = 0, input_my = 0,input_opera = 0;
	char ret_check;
	unsigned char i;
	char coordinate_string[256];
	char *argv1, *argv2, *cur;
	unsigned long val;
	err = kstrtoul(buf, 10, &val);
	/*if (err) {
		pr_err("tp_coordinate_value_store--Invalid size\n");
		return err;
	}*/
	vir_mouse_point.mx = 0;
	vir_mouse_point.my = 0;
	memset(coordinate_string,0,sizeof(coordinate_string));
	sprintf(coordinate_string,"%s",buf);
	cur = coordinate_string;
	argv1 = strsep(&cur," ");
	argv2 = cur;
	ret_check = checknum(argv2);
	if (ret_check)
		return ret_check;

	vir_mouse_point.operate = (int)(argv2[1] - '0');
	if (vir_mouse_point.operate == 2)
		input_opera = 0;
	else 
		input_opera = vir_mouse_point.operate;

	for (i = 2; i <= 5; i++) {
		if ((argv2[i] == '0') && (!eff_firstflag))
			continue;
		else if ((argv2[i] != '0') && (!eff_firstflag)) {
			eff_firstflag = 1;
			vir_mouse_point.mx += (int)(argv2[i] - '0')*powm(5-i);
		} else {
			vir_mouse_point.mx += (int)(argv2[i] - '0')*powm(5-i);
		}
	}
	eff_firstflag = 0;

	for (i = 6; i <= 9; i++) {
		if ((argv2[i] == '0') && (!eff_firstflag))
			continue;
		else if ((argv2[i] != '0') && (!eff_firstflag)) {
			eff_firstflag = 1;
			vir_mouse_point.my += (int)(argv2[i] - '0')*powm(9-i);
		} else {
			vir_mouse_point.my += (int)(argv2[i] - '0')*powm(9-i);
		}
	}
	eff_firstflag = 0;
	if(vir_mouse_point.mx > SCREEN_MAX_X || vir_mouse_point.mx > SCREEN_MAX_Y){
		pr_err("%s:%d: form data out of range\n",__func__, __LINE__);
		return -1;
	}
	input_mx = vir_mouse_point.mx - vir_mouse_point.mx_last;
	input_my = vir_mouse_point.my - vir_mouse_point.my_last;
	vir_mouse_point.mx_last = vir_mouse_point.mx;
	vir_mouse_point.my_last = vir_mouse_point.my;
	input_report_key(virtmouse_dev,BTN_LEFT,input_opera);
	input_report_rel(virtmouse_dev, REL_X, input_mx);
	input_report_rel(virtmouse_dev, REL_Y, input_my);
	input_sync(virtmouse_dev);
	return size;
	
}

static struct file_operations tp_coordinate_fops = {  
    .owner      =   THIS_MODULE  
};  

static struct device_attribute tp_coordinate_class_attrs[] = {
	__ATTR(value, 0644, tp_coordinate_value_show ,tp_coordinate_value_store),
	__ATTR_NULL,
};

static int tp_coordinate_probe(struct platform_device *pdev)
{

 	alloc_chrdev_region(&tp_coordinate_devno, 0, 1, DRIVER_NAME);
	cdev_add(&tp_coordinate_cdev, tp_coordinate_devno, 1);
	cdev_init(&tp_coordinate_cdev, &tp_coordinate_fops);   
	tp_coordinate_class = class_create(THIS_MODULE, DRIVER_NAME);
	tp_coordinate_class->dev_attrs = tp_coordinate_class_attrs;
	device_create(tp_coordinate_class,NULL, 0, NULL,"coordinate");
	
	return 0;
}

static struct platform_device tp_coordinate_device = {
	.name	= DRIVER_NAME,
	.id	=-1,
};

static struct platform_driver tp_coordinate_driver = {
	.probe		= tp_coordinate_probe,
	.driver		= {
	        .name	= DRIVER_NAME,
	        .owner	= THIS_MODULE,
	}
};

static int __init tp_coordinate_init(void)
{	
	platform_device_register(&tp_coordinate_device);
	platform_driver_register(&tp_coordinate_driver);
	return 0;
}

static int __init virtmouse_init(void)
{
	int error;

	virtmouse_dev = input_allocate_device();
	if(!virtmouse_dev)
		return -ENODEV;

	virtmouse_dev->name ="Virtual mouse";
	virtmouse_dev->phys = "virtmouse/input0";
	virtmouse_dev->id.bustype = BUS_AMIGA;

	virtmouse_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REL);
	virtmouse_dev->relbit[0] = BIT_MASK(REL_X) | BIT_MASK(REL_Y);
	virtmouse_dev->keybit[BIT_WORD(BTN_LEFT)] = BIT_MASK(BTN_LEFT) |
		BIT_MASK(BTN_MIDDLE) | BIT_MASK(BTN_RIGHT);

	error = input_register_device(virtmouse_dev);
	if (error) {
		input_free_device(virtmouse_dev);
		return error;
	}

	return 0;
}
static void __exit tp_coordinate_exit(void)
{
	cdev_del(&tp_coordinate_cdev);
	platform_driver_unregister(&tp_coordinate_driver);
	device_destroy(tp_coordinate_class , tp_coordinate_devno);
	class_destroy(tp_coordinate_class);
}

static void __exit virtmouse_exit(void)
{
	input_unregister_device(virtmouse_dev);
}

module_init(tp_coordinate_init);
module_exit(tp_coordinate_exit);
module_init(virtmouse_init);
module_exit(virtmouse_exit);
MODULE_AUTHOR("lomboswer@lombotech.com");
MODULE_DESCRIPTION("LomboTech N7 gpio¡ªout Driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:tp_coordinate");
