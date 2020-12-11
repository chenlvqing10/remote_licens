#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/input.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/delay.h>

#include <asm/uaccess.h>
#include <asm/io.h>

//#include <mach/platform.h>

#include "pwm-gpio.h"

#define PWM_IO (60)

#define PWM_DEVICE_NAME   "pwm-gpio"     
#define PWM_CLASS_NAME   "pwm-gpio"
#define PWM_DEVICE_NUM     "pwm-gpio"


typedef enum {
    PWM_DISABLE = 0,
    PWM_ENABLE,
}PWM_STATUS_t;

struct pwm_chip{
    dev_t devno;
    struct cdev *cdev;
    struct class *cls;
    struct device *dev;
    unsigned long period;
    unsigned long duty;
    struct hrtimer mytimer;
    ktime_t kt;
    PWM_STATUS_t status;
};

struct pwm_chip *pwm_dev;

static void pwm_gpio_start(void);
static enum hrtimer_restart    hrtimer_handler(struct hrtimer *timer);


int pwm_drv_open (struct inode * inode, struct file *filp)
{
    int minor;
    int major;

    minor = iminor(inode);
    major = imajor(inode);

    printk("major = %d minor = %d\n",major,minor);
    filp->private_data = (void *)minor;
    
    return 0;
}
ssize_t pwm_drv_read (struct file *filp, char __user *userbuf, size_t count, loff_t *fpos)
{
    int ret;
    if(filp->f_flags & O_NONBLOCK){
        return -EAGAIN;
    }

    ret = copy_to_user(userbuf,&pwm_dev->period,count);
    if(ret > 0){
        printk("error copy_to_user");
        return -EFAULT;
    }

    return count;
}
ssize_t pwm_drv_write (struct file *filp, const char __user *userbuf, size_t count, loff_t *fpos)
{
    return 0;
}

int pwm_drv_ioctl(struct file *filep, int cmd, unsigned long arg)
{
    int ret = 0;
    printk("cmd %d\n",cmd);
    switch(cmd)
    {
        case PWM_PERIOD_SET :
            pwm_dev->period = arg;
            printk("period %d\n",pwm_dev->period);
            break;

        case PWM_DUTY_SET :
            pwm_dev->duty = arg;
            printk("duty %d\n",pwm_dev->duty);
            break;

        case PWM_START :
            printk("pwm start\n");
            if(pwm_dev->status == PWM_DISABLE){
                printk("-----------start hrtimer timer-------------\n");
                pwm_dev->status = PWM_ENABLE;
                pwm_gpio_start();
            }else{
                printk("pwm_aready work\n");
            }
            break;
        default :
            ret = -1;
            break;
    }  
    return 0;     
}

int pwm_drv_close (struct inode *inode, struct file *filp)
{

    return 0;

}

const struct file_operations pwm_fops = {
    .open = pwm_drv_open,
    .write = pwm_drv_write,
    .read = pwm_drv_read,
    .unlocked_ioctl = pwm_drv_ioctl,
    .release = pwm_drv_close,
};

static int pwm_gpio_init(void)
{
    int ret = -1;
    pwm_dev->period = 1000000;
    pwm_dev->duty = 500000;

    if(gpio_request(PWM_IO, "PWM_GPIO")){
        printk("error pwm_gpio_init");
        return ret;

    }else{
        gpio_direction_output(PWM_IO, 1);
        gpio_set_value(PWM_IO, 1);
        pwm_dev->status = PWM_DISABLE;
    }

    return 0;

}

static void pwm_gpio_exit(void)
{
    gpio_set_value(PWM_IO, 1);
    gpio_free(PWM_IO);
}

static void pwm_gpio_start(void)
{
    hrtimer_init(&pwm_dev->mytimer,CLOCK_MONOTONIC,HRTIMER_MODE_REL);
    pwm_dev->mytimer.function = hrtimer_handler;
    pwm_dev->kt = ktime_set(0, pwm_dev->period-pwm_dev->duty);
    hrtimer_start(&pwm_dev->mytimer,pwm_dev->kt,HRTIMER_MODE_REL);
    
}

static enum hrtimer_restart    hrtimer_handler(struct hrtimer *timer)
{    
    if (gpio_get_value(PWM_IO) == 1) {
        if (pwm_dev->duty != pwm_dev->period) {
            gpio_set_value(PWM_IO, 0);
            pwm_dev->kt = ktime_set(0, pwm_dev->period-pwm_dev->duty);
        }
        hrtimer_forward_now(&pwm_dev->mytimer, pwm_dev->kt);
    } else {
        if (pwm_dev->duty != 0) {
            gpio_set_value(PWM_IO, 1);
            pwm_dev->kt = ktime_set(0, pwm_dev->duty);
        }
        hrtimer_forward_now(&pwm_dev->mytimer, pwm_dev->kt);
    }
    return HRTIMER_RESTART;    
}


static int __init pwm_drv_init(void)
{
    int ret;

    pwm_dev = kzalloc(sizeof(struct pwm_chip),GFP_KERNEL);
    if(pwm_dev == NULL){
        printk("kzalloc error");
        return -ENOMEM;
    }

    ret = alloc_chrdev_region(&pwm_dev->devno,0,1,PWM_DEVICE_NAME);
    if(ret != 0){
        printk("error alloc_chrdev_region");
        goto err_free;
    }

    pwm_dev->cdev = cdev_alloc();
    cdev_init(pwm_dev->cdev,&pwm_fops);
    cdev_add(pwm_dev->cdev,pwm_dev->devno,1);

    pwm_dev->cls = class_create(THIS_MODULE,PWM_CLASS_NAME);
    if(IS_ERR(pwm_dev->cls)){
        printk("error class_create");
        ret = PTR_ERR(pwm_dev->cls);
        goto err_unregister;
    }

    pwm_dev->dev = device_create(pwm_dev->cls,NULL,pwm_dev->devno,NULL,"%s",PWM_DEVICE_NUM);
    if(IS_ERR(pwm_dev->dev)){
        printk("error device_create");
        ret = PTR_ERR(pwm_dev);
        goto err_class_error;
    }
    
    ret = pwm_gpio_init();
    if(ret !=0){
        printk("error pwm_gpio_init");
        goto err_device_create;
    }
    
    return 0;

err_device_create:
    device_destroy(pwm_dev->cls,pwm_dev->devno);

err_class_error:
    class_destroy(pwm_dev->cls);

err_unregister:    
    cdev_del(pwm_dev->cdev);
    unregister_chrdev_region(pwm_dev->devno,1);

err_free:
    kfree(pwm_dev);
    return ret;
    

}

static void __exit pwm_drv_exit(void)
{
    pwm_gpio_exit();
    device_destroy(pwm_dev->cls,pwm_dev->devno);
    class_destroy(pwm_dev->cls);
    cdev_del(pwm_dev->cdev);
    unregister_chrdev_region(pwm_dev->devno,1);
    kfree(pwm_dev);

}

module_init(pwm_drv_init);
module_exit(pwm_drv_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("kevin");
