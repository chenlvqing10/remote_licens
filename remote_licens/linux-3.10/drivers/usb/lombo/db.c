/*
 * lombo usb driver debug interfaces
 *
 * Copyright(c)2018 Lombo Tech Co.Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/types.h>
#include <linux/spinlock.h>
#include <linux/debugfs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/usb/ch9.h>
#include <linux/usb/gadget.h>
#include <linux/usb/phy.h>
#include <linux/usb/otg.h>

#include "lombo_nx.h"
#include "lb.h"
#include "udc.h"
#include "bits.h"
#include "db.h"
#include "otg.h"

/* prints information about device capabilities and status */
static int lb_device_show(struct seq_file *s, void *data)
{
	struct lb_hdc *lb = s->private;
	struct usb_gadget *gadget = &lb->gadget;

	seq_printf(s, "speed             = %d\n", gadget->speed);
	seq_printf(s, "max_speed         = %d\n", gadget->max_speed);
	seq_printf(s, "is_otg            = %d\n", gadget->is_otg);
	seq_printf(s, "is_a_peripheral   = %d\n", gadget->is_a_peripheral);
	seq_printf(s, "b_hnp_enable      = %d\n", gadget->b_hnp_enable);
	seq_printf(s, "a_hnp_support     = %d\n", gadget->a_hnp_support);
	seq_printf(s, "a_alt_hnp_support = %d\n", gadget->a_alt_hnp_support);
	seq_printf(s, "name              = %s\n",
		   (gadget->name ? gadget->name : ""));

	if (!lb->driver)
		return 0;

	seq_printf(s, "gadget function   = %s\n",
		       (lb->driver->function ? lb->driver->function : ""));
	seq_printf(s, "gadget max speed  = %d\n", lb->driver->max_speed);

	return 0;
}

static int lb_device_open(struct inode *inode, struct file *file)
{
	return single_open(file, lb_device_show, inode->i_private);
}


static const struct file_operations lb_device_fops = {
	.open		= lb_device_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

/* reads port test mode */
static int lb_port_test_show(struct seq_file *s, void *data)
{
	struct lb_hdc *lb = s->private;
	unsigned long flags;
	unsigned mode;

	spin_lock_irqsave(&lb->lock, flags);
	mode = hw_port_test_get(lb);
	spin_unlock_irqrestore(&lb->lock, flags);

	seq_printf(s, "mode = %u\n", mode);

	return 0;
}

/* writes port test mode */
static ssize_t lb_port_test_write(struct file *file, const char __user *ubuf,
				  size_t count, loff_t *ppos)
{
	struct seq_file *s = file->private_data;
	struct lb_hdc *lb = s->private;
	unsigned long flags;
	unsigned mode;
	char buf[32];
	int ret;

	if (copy_from_user(buf, ubuf, min_t(size_t, sizeof(buf) - 1, count)))
		return -EFAULT;

	if (sscanf(buf, "%u", &mode) != 1)
		return -EINVAL;

	spin_lock_irqsave(&lb->lock, flags);
	ret = hw_port_test_set(lb, mode);
	spin_unlock_irqrestore(&lb->lock, flags);

	return ret ? ret : count;
}

static int lb_port_test_open(struct inode *inode, struct file *file)
{
	return single_open(file, lb_port_test_show, inode->i_private);
}

static const struct file_operations lb_port_test_fops = {
	.open		= lb_port_test_open,
	.write		= lb_port_test_write,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

/* DMA contents of all queue heads */
static int lb_qheads_show(struct seq_file *s, void *data)
{
	struct lb_hdc *lb = s->private;
	unsigned long flags;
	unsigned i, j;

	if (lb->role != LB_ROLE_GADGET) {
		seq_puts(s, "not in gadget mode\n");
		return 0;
	}

	spin_lock_irqsave(&lb->lock, flags);
	for (i = 0; i < lb->hw_ep_max/2; i++) {
		struct lb_hw_ep *hweprx = &lb->lb_hw_ep[i];
		struct lb_hw_ep *hweptx = &lb->lb_hw_ep[i + lb->hw_ep_max/2];
		seq_printf(s, "EP=%02i: RX=%08X TX=%08X\n",
			   i, (u32)hweprx->qh.dma, (u32)hweptx->qh.dma);
		for (j = 0; j < (sizeof(struct lb_hw_qh)/sizeof(u32)); j++)
			seq_printf(s, " %04X:    %08X    %08X\n", j,
				   *((u32 *)hweprx->qh.ptr + j),
				   *((u32 *)hweptx->qh.ptr + j));
	}
	spin_unlock_irqrestore(&lb->lock, flags);

	return 0;
}

static int lb_qheads_open(struct inode *inode, struct file *file)
{
	return single_open(file, lb_qheads_show, inode->i_private);
}

static const struct file_operations lb_qheads_fops = {
	.open		= lb_qheads_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

/* DMA contents of all requests currently queued (all endpts) */
static int lb_requests_show(struct seq_file *s, void *data)
{
	struct lb_hdc *lb = s->private;
	unsigned long flags;
	struct list_head   *ptr = NULL;
	struct lb_hw_req *req = NULL;
	struct td_node *node, *tmpnode;
	unsigned i, j, qsize = sizeof(struct lb_hw_td)/sizeof(u32);

	if (lb->role != LB_ROLE_GADGET) {
		seq_puts(s, "not in gadget mode\n");
		return 0;
	}

	spin_lock_irqsave(&lb->lock, flags);
	for (i = 0; i < lb->hw_ep_max; i++)
		list_for_each(ptr, &lb->lb_hw_ep[i].qh.queue) {
			req = list_entry(ptr, struct lb_hw_req, queue);

			list_for_each_entry_safe(node, tmpnode, &req->tds, td) {
				seq_printf(s, "EP=%02i: TD=%08X %s\n",
					   i % (lb->hw_ep_max / 2),
					   (u32)node->dma,
					   ((i < lb->hw_ep_max/2) ?
					   "RX" : "TX"));

				for (j = 0; j < qsize; j++)
					seq_printf(s, " %04X:    %08X\n", j,
						   *((u32 *)node->ptr + j));
			}
		}
	spin_unlock_irqrestore(&lb->lock, flags);

	return 0;
}

static int lb_requests_open(struct inode *inode, struct file *file)
{
	return single_open(file, lb_requests_show, inode->i_private);
}

static const struct file_operations lb_requests_fops = {
	.open		= lb_requests_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int lb_role_show(struct seq_file *s, void *data)
{
	struct lb_hdc *lb = s->private;

	seq_printf(s, "%s\n", lb_role(lb)->name);

	return 0;
}

static ssize_t lb_role_write(struct file *file, const char __user *ubuf,
			     size_t count, loff_t *ppos)
{
	struct seq_file *s = file->private_data;
	struct lb_hdc *lb = s->private;
	enum lb_role role;
	char buf[8];
	int ret;

	if (copy_from_user(buf, ubuf, min_t(size_t, sizeof(buf) - 1, count)))
		return -EFAULT;

	for (role = LB_ROLE_HOST; role < LB_ROLE_END; role++)
		if (lb->roles[role] &&
		    !strncmp(buf, lb->roles[role]->name,
			     strlen(lb->roles[role]->name)))
			break;

	if (role == LB_ROLE_END || role == lb->role)
		return -EINVAL;

	lb_role_stop(lb);
	ret = lb_role_start(lb, role);

	return ret ? ret : count;
}

static int lb_role_open(struct inode *inode, struct file *file)
{
	return single_open(file, lb_role_show, inode->i_private);
}

static const struct file_operations lb_role_fops = {
	.open		= lb_role_open,
	.write		= lb_role_write,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int lb_registers_show(struct seq_file *s, void *unused)
{
	struct lb_hdc *lb = s->private;
	u32 tmp_reg;

	if (!lb || lb->in_lpm)
		return -EPERM;

	/* ------ Registers ----- */
	tmp_reg = hw_read_intr_enable(lb);
	seq_printf(s, "USBINTR reg: %08x\n", tmp_reg);

	tmp_reg = hw_read_intr_status(lb);
	seq_printf(s, "USBSTS reg: %08x\n", tmp_reg);

	tmp_reg = hw_read(lb, OP_USBMODE, ~0);
	seq_printf(s, "USBMODE reg: %08x\n", tmp_reg);

	tmp_reg = hw_read(lb, OP_USBCMD, ~0);
	seq_printf(s, "USBCMD reg: %08x\n", tmp_reg);

	tmp_reg = hw_read(lb, OP_PORTSC, ~0);
	seq_printf(s, "PORTSC reg: %08x\n", tmp_reg);

	if (lb->is_otg) {
		tmp_reg = hw_read_otgsc(lb, ~0);
		seq_printf(s, "OTGSC reg: %08x\n", tmp_reg);
	}

	return 0;
}

static int lb_registers_open(struct inode *inode, struct file *file)
{
	return single_open(file, lb_registers_show, inode->i_private);
}

static const struct file_operations lb_registers_fops = {
	.open			= lb_registers_open,
	.read			= seq_read,
	.llseek			= seq_lseek,
	.release		= single_release,
};

/* dbg_create_files: initializes the attribute interface */
int dbg_create_files(struct lb_hdc *lb)
{
	struct dentry *dent;

	lb->debugfs = debugfs_create_dir(dev_name(lb->dev), NULL);
	if (!lb->debugfs)
		return -ENOMEM;

	dent = debugfs_create_file("device", S_IRUGO, lb->debugfs, lb,
				   &lb_device_fops);
	if (!dent)
		goto err;

	dent = debugfs_create_file("port_test", S_IRUGO | S_IWUSR, lb->debugfs,
				   lb, &lb_port_test_fops);
	if (!dent)
		goto err;

	dent = debugfs_create_file("qheads", S_IRUGO, lb->debugfs, lb,
				   &lb_qheads_fops);
	if (!dent)
		goto err;

	dent = debugfs_create_file("requests", S_IRUGO, lb->debugfs, lb,
				   &lb_requests_fops);
	if (!dent)
		goto err;

	dent = debugfs_create_file("role", S_IRUGO | S_IWUSR, lb->debugfs, lb,
				   &lb_role_fops);
	if (!dent)
		goto err;

	dent = debugfs_create_file("registers", S_IRUGO, lb->debugfs, lb,
				&lb_registers_fops);

	if (dent)
		return 0;
err:
	debugfs_remove_recursive(lb->debugfs);
	return -ENOMEM;
}

/* dbg_remove_files: destroys the attribute interface */
void dbg_remove_files(struct lb_hdc *lb)
{
	debugfs_remove_recursive(lb->debugfs);
}
