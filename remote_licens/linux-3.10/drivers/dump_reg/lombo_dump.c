/*
 * lombo_dump.c - Driver of LomboTech dump module
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

#include <linux/fs.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/err.h>
#include <linux/string.h>
#include <linux/kobject.h>
#include <mach/csp.h>
#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <mach/debug.h>
#include "lombo_dump.h"

#define vsize (u32)(dump.paddr[1] - dump.paddr[0])

static struct dump_para dump = {0, {0}, {0} };
static struct write_para write = {0, 0, 0, 0};

/*
 * addr_valid_judge - judge the validty of the input address
 * @param: none
 *
 * return: 0, valid; EINVAL, unvalid
 */
static int addr_valid_judge(void __iomem *addr)
{
	int ret = 0;
#ifdef CONFIG_ARCH_LOMBO_N9
	u64 j_addr = (u64)addr;
#endif

#ifdef CONFIG_ARCH_LOMBO_N7
	u32 j_addr = (u32)addr;
#endif
	if (((j_addr & 0b11) != 0) || (j_addr == 0))
		ret = -EINVAL;
	return ret;
}

/*
 *virtual_addr_judge - jude the virtual address
 *retuen: 0, not used; EINVAL, used
 */
static int virtual_addr_judge(void __iomem *addr)
{
	int ret = 0;

	if (addr != 0)
		ret = 1;
	return ret;
}

/*
 * printf_help - printf user guide
 * @param: none
 *
 */
static void printf_help(void)
{
	pr_info("---------user guide--------------\n");
	pr_info("1.read a register:\n");
	pr_info("   echo address > dump\n");
	pr_info("   cat dump\n");
	pr_info("   e.g.echo 0x0400b000 > dump;cat dump\n");
	pr_info("2.read two registers:\n");
	pr_info("   echo address1,address2 > dump\n");
	pr_info("   cat dump\n");
	pr_info("   e.g.echo 0x0400b000,0x0400b020 > dump;cat dump\n");
	pr_info("3.read a group of registers:\n");
	pr_info("   echo start_address end_address > dump\n");
	pr_info("   cat dump\n");
	pr_info("   e.g.echo 0x0400b020 0x0400b080 > dump;cat dump\n");
	pr_info("4.write value to a register:\n");
	pr_info("   echo address value > write\n");
	pr_info("   get result of write: cat write\n");
	pr_info("   e.g.echo 0x0400b020 0x0f02 > write;cat write\n");
	pr_info("NOTE:1.Address must be aligned in 4 bytes and hexadecimal.\n");
	pr_info("     2.The value of input should be hexadecimal.\n");
	pr_info("     3.address:physical address;v_address:virtual address.\n");
}

/*
 * dump_show - read register
 * @param: none
 *
 * return length of read buf
 */
static ssize_t dump_show(struct class *class, struct class_attribute *attr,
								char *buf)
{
	u32 dump_val = 0, cnt = 0, offset = 0;
	void __iomem *dump_vaddr;
	if (dump.addr[0] == 0)
		goto out;

	if (dump.mode == DUMP_MOD_GROUP) {
		dump_vaddr = dump.addr[0];
		offset += sprintf(buf + offset, "%-14s:0x%8p~0x%8p\n",
				"address", dump.paddr[0], dump.paddr[1]);
		offset += sprintf(buf + offset, "%-14s:0x%8p~0x%8p",
			"v_address", dump.addr[0], dump.addr[0] + vsize);
		do {
			if (cnt % 4 == 0) {
				offset += sprintf(buf + offset, "\n0x%8p:",
							dump_vaddr);
			}
			offset += sprintf(buf + offset, "   0x%08x\t",
						readl(dump_vaddr));
			dump_vaddr += 0x04;
			cnt++;
		} while (dump_vaddr <= (dump.addr[0] + vsize));
		strcat(buf, "\n");
	} else if (dump.mode == DUMP_MOD_MUL) {
		dump_val = readl(dump.addr[0]);
		offset += sprintf(buf + offset, "%-14s\t%-14s\t%-14s\n",
				"address", "dump_val", "v_address");
		offset += sprintf(buf + offset, "0x%8p    \t0x%08x    \t0x%p\n",
					dump.paddr[0], dump_val, dump.addr[0]);
		dump_val = readl(dump.addr[1]);
		offset += sprintf(buf + offset, "0x%8p    \t0x%08x    \t0x%p\n",
					dump.paddr[1], dump_val, dump.addr[1]);
	} else if (dump.mode == DUMP_MOD_ONE) {
		dump_val = readl(dump.addr[0]);
		offset += sprintf(buf + offset, "%-14s\t%-14s\t%-14s\n",
				"address", "dump_val", "v_address");
		offset += sprintf(buf + offset, "0x%8p    \t0x%08x    \t0x%p\n",
					dump.paddr[0], dump_val, dump.addr[0]);
	}
out:
	return strlen(buf);
}

/*
 * dump_store - store data from user
 * @param: none
 *
 * return length of buf
 */
static ssize_t dump_store(struct class *class, struct class_attribute *attr,
				const char *buf, size_t size)
{
	char *p_addr = NULL;
	int ret = 0;

	ret = virtual_addr_judge(dump.addr[0]);
	if (ret) {
		iounmap(dump.addr[0]);
		dump.addr[0] = 0;
		if (dump.mode == DUMP_MOD_MUL) {
			iounmap(dump.addr[1]);
			dump.addr[1] = 0;
		}
	}

	sscanf(buf, "%x", (unsigned int *)&dump.addr[0]);
	if (size <= 11) {
		ret = addr_valid_judge(dump.addr[0]);
		if (!ret) {
			dump.paddr[0] = dump.addr[0];
			dump.addr[0] = (void __iomem *)ioremap
					((phys_addr_t)dump.addr[0], 0x04);
		} else {
			pr_err("error address:0x%8p\n", dump.addr[0]);
			memset(&dump, 0, sizeof(struct dump_para));
			printf_help();
			goto out;
		}
		dump.mode = DUMP_MOD_ONE;
		dump.addr[1] = 0;
	} else {
		if (NULL != strchr(buf, ',')) {
			p_addr = strchr(buf, ',');
			dump.mode = DUMP_MOD_MUL;
		} else if (NULL != strchr(buf, ' ')) {
			p_addr = strchr(buf, ' ');
			dump.mode = DUMP_MOD_GROUP;
		}
		if (NULL == p_addr) {
			pr_err("The arguments of input was error!\n");
			printf_help();
			goto out;
		}
		sscanf(p_addr + 1, "%x", (unsigned int *)&dump.addr[1]);
		ret = addr_valid_judge(dump.addr[1]);
		if (!ret) {
			dump.paddr[0] = dump.addr[0];
			dump.paddr[1] = dump.addr[1];
			if (dump.mode == DUMP_MOD_MUL) {
				dump.addr[0] = (void __iomem *)
				ioremap((phys_addr_t)dump.addr[0], 0x04);
				dump.addr[1] = (void __iomem *)
				ioremap((phys_addr_t)dump.addr[1], 0x04);
			} else if (dump.mode == DUMP_MOD_GROUP)
				dump.addr[0] = (void __iomem *)
					ioremap((phys_addr_t)dump.addr[0],
						dump.addr[1] - dump.addr[0]);
		} else {
			pr_err("error address:0x%8p\n", dump.addr[1]);
			memset(&dump, 0, sizeof(struct dump_para));
			printf_help();
			goto out;
		}
	}
out:
	return size;
}

/*
 *write_show - show the result of write
 * @param: none
 *
 * return length of write buf
 */
static ssize_t write_show(struct class *class, struct class_attribute *attr,
								char *buf)
{
	u32 dump_val = 0, offset = 0;

	if (write.addr == 0)
		goto out;

	dump_val = readl(write.addr);
	offset += sprintf(buf + offset, "%-14s\t%-14s\t%-14s\t%-14s\n",
			"address", "before_write", "after_write", "v_address");
	offset += sprintf(buf + offset,
				"0x%p    \t0x%08x    \t0x%08x    \t0x%p\n",
			write.paddr, write.bef_val, dump_val, write.addr);
out:
	return strlen(buf);
}

/*
 * write_store - store data from user and weite register
 * @param: none
 *
 * return length of write buf
 */
static ssize_t write_store(struct class *class, struct class_attribute *attr,
						const char *buf, size_t size)
{
	int ret = 0;
	char *p_val = NULL;

	ret = virtual_addr_judge(write.addr);
	if (ret) {
		write.addr = 0;
		iounmap(write.addr);
	}

	sscanf(buf, "%x", (unsigned int *)&write.addr);
	ret = addr_valid_judge(write.addr);
	if (!ret) {
		write.paddr = write.addr;
		write.addr = (void __iomem *)ioremap
					((phys_addr_t)write.addr, 0x04);
	} else {
		pr_err("error address:0x%8p\n", write.addr);
		memset(&write, 0, sizeof(struct write_para));
		printf_help();
		goto out;
	}
	p_val = strchr(buf, ' ');
	sscanf(p_val + 1, "%x", &write.chg_val);
	write.bef_val = readl(write.addr);
	writel(write.chg_val, write.addr);
out:
	return size;
}

static struct class_attribute dump_attrs[] = {
	__ATTR(dump, S_IWUSR | S_IRUGO, dump_show, dump_store),
	__ATTR(write, S_IWUSR | S_IRUGO, write_show, write_store),
	__ATTR_NULL
};

static struct class dump_class = {
	.name		= "lombo_dump",
	.owner		= THIS_MODULE,
	.class_attrs	= dump_attrs,
};

__init int dump_class_init(void)
{
	int ret = 0;

	ret = class_register(&dump_class);
	if (ret)
		PRT_ERR("dump_class register failed!\n");

	return ret;
}

void __exit dump_class_exit(void)
{
	class_unregister(&dump_class);
}

module_init(dump_class_init);
module_exit(dump_class_exit);

MODULE_AUTHOR("lomboswer@lombotech.com");
MODULE_DESCRIPTION("Driver for LomboTech dump reg module");
MODULE_LICENSE("GPL");
