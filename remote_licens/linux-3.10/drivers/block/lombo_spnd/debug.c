/*
 * debug.c - nand driver debug code for LomboTech
 * nand driver debug code implement
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
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/mutex.h>
#include <linux/uaccess.h>
#include <linux/debugfs.h>

#include "include/nftl.h"
#include "include/extra.h"

/* #define DBG */
#include <mach/debug.h>
#undef MOD_NAME
#define MOD_NAME	"NAND_DBG"


#define NAND_DBG_DEBUG
#ifdef NAND_DBG_DEBUG
#define DBG_INFO(fmt, ...)	PRT_INFO(fmt "\n", ##__VA_ARGS__)
#endif

#define DBG_ERR(fmt, ...)	PRT_ERR(fmt "\n", ##__VA_ARGS__)

#define NAND_ROOT_NAME			"nand"
#define NFTL_ROOT_NAME			"nftl"
#define PHY_ROOT_NAME			"phy"


static struct dentry *nand_root;

static ssize_t write_nftl_key(struct file *file, const char __user *buf,
	size_t count, loff_t *pos)
{
	char lbuf[12];
	struct inode *inode = file_inode(file);

	if (count > sizeof(lbuf))
		return -EINVAL;

	if (copy_from_user(lbuf, buf, count))
		return -EFAULT;

	mutex_lock(&inode->i_mutex);
	_nftl_set_key(lbuf, count);
	mutex_unlock(&inode->i_mutex);

	return count;
}

static ssize_t write_nftl_compressor(struct file *file, const char __user *buf,
	size_t count, loff_t *pos)
{
	char lbuf[8];
	struct inode *inode = file_inode(file);

	memset(lbuf, 0, sizeof(lbuf));

	if ((count < 1) || (count > sizeof(lbuf)))
		goto out;

	if (copy_from_user(lbuf, buf, count))
		return -EFAULT;
	lbuf[count - 1] = '\0';

#if 0
	DBG_INFO("count = %i", count);
	DBG_INFO("%02x %02x %02x %02x %02x %02x %02x %02x",
	lbuf[0], lbuf[1], lbuf[2], lbuf[3], lbuf[4], lbuf[5], lbuf[6], lbuf[7]);
	DBG_INFO("%c %c %c %c %c %c %c %c",
	lbuf[0], lbuf[1], lbuf[2], lbuf[3], lbuf[4], lbuf[5], lbuf[6], lbuf[7]);
#endif

out:
	mutex_lock(&inode->i_mutex);
	_change_compressor(lbuf);
	mutex_unlock(&inode->i_mutex);

	return count;
}

static ssize_t show_block_oob(struct file *file, const char __user *buf,
	size_t count, loff_t *pos)
{
	char lbuf[12];
	long block;
	struct inode *inode = file_inode(file);

	if (count > sizeof(lbuf))
		return -EINVAL;

	memset(lbuf, 0, sizeof(lbuf));
	if (copy_from_user(lbuf, buf, count))
		return -EFAULT;

	if (kstrtol(lbuf, 10, &block))
		return -EINVAL;

#if 0
	DBG_INFO("block = %d", block);
#endif

	mutex_lock(&inode->i_mutex);
	_show_block_oob(block);
	mutex_unlock(&inode->i_mutex);

	return count;
}

static ssize_t show_map(struct file *file, const char __user *buf,
	size_t count, loff_t *pos)
{
	char lbuf[12];
	long block;
	struct inode *inode = file_inode(file);

	if (count > sizeof(lbuf))
		return -EINVAL;

	memset(lbuf, 0, sizeof(lbuf));
	if (copy_from_user(lbuf, buf, count))
		return -EFAULT;

	if (kstrtol(lbuf, 10, &block))
		return -EINVAL;

#if 0
	DBG_INFO("block = %d", block);
#endif

	mutex_lock(&inode->i_mutex);
	_show_map(block);
	mutex_unlock(&inode->i_mutex);

	return count;
}

static ssize_t show_page(struct file *file, const char __user *buf,
	size_t count, loff_t *pos)
{
	char lbuf[12];
	long block;
	struct inode *inode = file_inode(file);

	if (count > sizeof(lbuf))
		return -EINVAL;

	memset(lbuf, 0, sizeof(lbuf));
	if (copy_from_user(lbuf, buf, count))
		return -EFAULT;

	if (kstrtol(lbuf, 10, &block))
		return -EINVAL;

#if 0
	DBG_INFO("block = %d", block);
#endif

	mutex_lock(&inode->i_mutex);
	_show_page(block);
	mutex_unlock(&inode->i_mutex);

	return count;
}

static ssize_t read_nftl_version(struct file *file, char __user *buf,
				size_t count, loff_t *ppos)
{
	struct inode *inode = file_inode(file);

	mutex_lock(&inode->i_mutex);
	_show_nftl_version();
	mutex_unlock(&inode->i_mutex);

	return 0;
}

static ssize_t read_nftl_info(struct file *file, char __user *buf,
				size_t count, loff_t *ppos)
{
	struct inode *inode = file_inode(file);

	mutex_lock(&inode->i_mutex);
	_show_nftl_info();
	mutex_unlock(&inode->i_mutex);

	return 0;
}

static ssize_t read_block_info(struct file *file, char __user *buf,
				size_t count, loff_t *ppos)
{
	struct inode *inode = file_inode(file);

	mutex_lock(&inode->i_mutex);
	_show_block_info();
	mutex_unlock(&inode->i_mutex);

	return 0;
}

static ssize_t read_gc_info(struct file *file, char __user *buf,
				size_t count, loff_t *ppos)
{
	struct inode *inode = file_inode(file);

	mutex_lock(&inode->i_mutex);
	_show_gc_info();
	mutex_unlock(&inode->i_mutex);

	return 0;
}

static const struct file_operations nftl_key_fops = {
	.write		= write_nftl_key,
};

static const struct file_operations nftl_compressor_fops = {
	.write		= write_nftl_compressor,
};

static const struct file_operations block_oob_fops = {
	.write		= show_block_oob,
};

static const struct file_operations map_fops = {
	.write		= show_map,
};

static const struct file_operations page_fops = {
	.write		= show_page,
};

static const struct file_operations nftl_version_fops = {
	.read		= read_nftl_version,
};

static const struct file_operations nftl_info_fops = {
	.read		= read_nftl_info,
};

static const struct file_operations block_info_fops = {
	.read		= read_block_info,
};

static const struct file_operations gc_info_fops = {
	.read		= read_gc_info,
};

static int add_nftl_debug_info(struct dentry *parent)
{
	struct dentry *nftl_root;

	BUG_ON(!parent);

	nftl_root = debugfs_create_dir(NFTL_ROOT_NAME, parent);
	if (IS_ERR(nftl_root))
		/* Don't complain -- debugfs just isn't enabled */
		return -1;
	if (!nftl_root)
		/* Complain -- debugfs is enabled, but it failed to
		 * create the directory. */
		goto err;

	if (!debugfs_create_file("key", S_IWUSR, nftl_root, NULL,
					&nftl_key_fops)) {
		DBG_ERR("add key fail");
		goto err;
	}

	if (!debugfs_create_file("compressor", S_IWUSR, nftl_root, NULL,
					&nftl_compressor_fops)) {
		DBG_ERR("add compressor fail");
		goto err;
	}

	if (!debugfs_create_file("block_oob", S_IWUSR, nftl_root, NULL,
					&block_oob_fops)) {
		DBG_ERR("add block_oob fail");
		goto err;
	}

	if (!debugfs_create_file("map", S_IWUSR, nftl_root, NULL,
					&map_fops)) {
		DBG_ERR("add map fail");
		goto err;
	}

	if (!debugfs_create_file("page", S_IWUSR, nftl_root, NULL,
					&page_fops)) {
		DBG_ERR("add page fail");
		goto err;
	}

	if (!debugfs_create_file("version", S_IRUSR, nftl_root, NULL,
					&nftl_version_fops)) {
		DBG_ERR("add nftl_version fail");
		goto err;
	}

	if (!debugfs_create_file("nftl_info", S_IRUSR, nftl_root, NULL,
					&nftl_info_fops)) {
		DBG_ERR("add nftl_info fail");
		goto err;
	}

	if (!debugfs_create_file("block_info", S_IRUSR, nftl_root, NULL,
					&block_info_fops)) {
		DBG_ERR("add block_info fail");
		goto err;
	}

	if (!debugfs_create_file("gc_info", S_IRUSR, nftl_root, NULL,
					&gc_info_fops)) {
		DBG_ERR("add gc_info fail");
		goto err;
	}

	return 0;

err:
	DBG_ERR("fail");
	debugfs_remove_recursive(nftl_root);
	return -1;
}

#ifdef SPND_TEST_WITH_BURN_LIB
static ssize_t phy_erase_nftl(struct file *file, char __user *buf,
				size_t count, loff_t *ppos)
{
	struct inode *inode = file_inode(file);

	mutex_lock(&inode->i_mutex);
	DBG_INFO("nandphy_erase_nftl_area start ...");
	nandphy_erase_nftl_area();
	DBG_INFO("nandphy_erase_nftl_area finish");
	mutex_unlock(&inode->i_mutex);

	return 0;
}

static const struct file_operations phy_erase_nftl_fops = {
	.read		= phy_erase_nftl,
};

static ssize_t phy_format_all(struct file *file, char __user *buf,
				size_t count, loff_t *ppos)
{
	struct inode *inode = file_inode(file);

	mutex_lock(&inode->i_mutex);
	DBG_INFO("spnd_format start ...");
	spnd_format(0);
	DBG_INFO("spnd_format finish");
	mutex_unlock(&inode->i_mutex);

	return 0;
}

static const struct file_operations phy_format_fops = {
	.read		= phy_format_all,
};
#endif

#ifdef SPND_HAS_DUMP_LIB
static ssize_t phy_dump_mapping(struct file *file, char __user *buf,
				size_t count, loff_t *ppos)
{
	struct inode *inode = file_inode(file);

	mutex_lock(&inode->i_mutex);
	DBG_INFO("phy_block_dump_mapping start ...");
	phy_block_dump_mapping();
	DBG_INFO("phy_block_dump_mapping finish");
	mutex_unlock(&inode->i_mutex);

	return 0;
}

static ssize_t phy_block_map(struct file *file, const char __user *buf,
				size_t count, loff_t *ppos)
{
	struct inode *inode = file_inode(file);
	u32 vblock = 0;
	u32 pblock = 0;

	mutex_lock(&inode->i_mutex);
	sscanf(buf, "%d", &vblock);
	pblock = phy_block_get_v2p_map(vblock);
	pr_info("%d\n", pblock);
	mutex_unlock(&inode->i_mutex);

	return count;
}

static const struct file_operations phy_dump_mapping_fops = {
	.read		= phy_dump_mapping,
	.write		= phy_block_map,
};

static ssize_t phy_dump_pparam(struct file *file, char __user *buf,
				size_t count, loff_t *ppos)
{
	struct inode *inode = file_inode(file);

	mutex_lock(&inode->i_mutex);
	DBG_INFO("phy_dump_phyparam start ...");
	phy_dump_phyparam();
	DBG_INFO("phy_dump_phyparam finish");
	mutex_unlock(&inode->i_mutex);

	return 0;
}

static const struct file_operations phy_dump_pparam_fops = {
	.read		= phy_dump_pparam,
};

static ssize_t phy_dump_nparam(struct file *file, char __user *buf,
				size_t count, loff_t *ppos)
{
	struct inode *inode = file_inode(file);

	mutex_lock(&inode->i_mutex);
	DBG_INFO("phy_dump_nandparam start ...");
	phy_dump_nandparam();
	DBG_INFO("phy_dump_nandparam finish");
	mutex_unlock(&inode->i_mutex);

	return 0;
}

static const struct file_operations phy_dump_nparam_fops = {
	.read		= phy_dump_nparam,
};

static ssize_t phy_dump_cparam(struct file *file, char __user *buf,
				size_t count, loff_t *ppos)
{
	struct inode *inode = file_inode(file);

	mutex_lock(&inode->i_mutex);
	DBG_INFO("phy_dump_snc_param start ...");
	phy_dump_snc_param();
	DBG_INFO("phy_dump_snc_param finish");
	mutex_unlock(&inode->i_mutex);

	return 0;
}

static const struct file_operations phy_dump_cparam_fops = {
	.read		= phy_dump_cparam,
};

static ssize_t phy_all_block_stats(struct file *file, char __user *buf,
				size_t count, loff_t *ppos)
{
	struct inode *inode = file_inode(file);

	mutex_lock(&inode->i_mutex);
	DBG_INFO("phy_show_all_block_stats start ...");
	phy_show_all_block_stats();
	DBG_INFO("phy_show_all_block_stats finish");
	mutex_unlock(&inode->i_mutex);

	return 0;
}

static ssize_t phy_block_stats(struct file *file, const char __user *buf,
				size_t count, loff_t *ppos)
{
	struct inode *inode = file_inode(file);
	u32 pblock = 0;

	mutex_lock(&inode->i_mutex);
	DBG_INFO("phy_show_block_stats %d start ...", pblock);
	sscanf(buf, "%d", &pblock);
	phy_show_block_stats(pblock);
	DBG_INFO("phy_show_block_stats %d finish", pblock);
	mutex_unlock(&inode->i_mutex);

	return count;
}

static const struct file_operations phy_block_stats_fops = {
	.read		= phy_all_block_stats,
	.write		= phy_block_stats,
};

static ssize_t phy_print_info(struct file *file, char __user *buf,
				size_t count, loff_t *ppos)
{
	struct inode *inode = file_inode(file);

	mutex_lock(&inode->i_mutex);
	DBG_INFO("phy_print_last_info start ...");
	phy_print_last_info();
	DBG_INFO("phy_print_last_info finish");
	mutex_unlock(&inode->i_mutex);

	return 0;
}

static const struct file_operations phy_print_info_fops = {
	.read		= phy_print_info,
};

static ssize_t phy_rpl_stats(struct file *file, char __user *buf,
				size_t count, loff_t *ppos)
{
	struct inode *inode = file_inode(file);

	mutex_lock(&inode->i_mutex);
	DBG_INFO("phy_block_show_rpl_stats start ...");
	phy_block_show_rpl_stats();
	DBG_INFO("phy_block_show_rpl_stats finish");
	mutex_unlock(&inode->i_mutex);

	return 0;
}

static const struct file_operations phy_rpl_stats_fops = {
	.read		= phy_rpl_stats,
};
#endif

static int add_phy_debug_info(struct dentry *parent)
{
	struct dentry *phy_root;

	BUG_ON(!parent);

	phy_root = debugfs_create_dir(PHY_ROOT_NAME, parent);
	if (IS_ERR(phy_root))
		/* Don't complain -- debugfs just isn't enabled */
		return -1;
	if (!phy_root)
		/* Complain -- debugfs is enabled, but it failed to
		 * create the directory. */
		goto err;

#ifdef SPND_TEST_WITH_BURN_LIB
	if (!debugfs_create_file("erase_nftl", S_IRUSR, phy_root, NULL,
					&phy_erase_nftl_fops)) {
		DBG_ERR("add erase_nftl fail");
		goto err;
	}

	if (!debugfs_create_file("phy_format", S_IRUSR, phy_root, NULL,
					&phy_format_fops)) {
		DBG_ERR("add phy_format fail");
		goto err;
	}
#endif

#ifdef SPND_HAS_DUMP_LIB
	if (!debugfs_create_file("dump_mapping", S_IRUSR, phy_root, NULL,
					&phy_dump_mapping_fops)) {
		DBG_ERR("add dump_mapping fail");
		goto err;
	}

	if (!debugfs_create_file("dump_pparam", S_IRUSR, phy_root, NULL,
					&phy_dump_pparam_fops)) {
		DBG_ERR("add dump_pparam fail");
		goto err;
	}

	if (!debugfs_create_file("dump_nparam", S_IRUSR, phy_root, NULL,
					&phy_dump_nparam_fops)) {
		DBG_ERR("add dump_nparam fail");
		goto err;
	}

	if (!debugfs_create_file("dump_cparam", S_IRUSR, phy_root, NULL,
					&phy_dump_cparam_fops)) {
		DBG_ERR("add dump_cparam fail");
		goto err;
	}

	if (!debugfs_create_file("block_stats", S_IRUSR, phy_root, NULL,
					&phy_block_stats_fops)) {
		DBG_ERR("add block_stats fail");
		goto err;
	}

	if (!debugfs_create_file("print_info", S_IRUSR, phy_root, NULL,
					&phy_print_info_fops)) {
		DBG_ERR("add print_info fail");
		goto err;
	}

	if (!debugfs_create_file("rpl_stats", S_IRUSR, phy_root, NULL,
					&phy_rpl_stats_fops)) {
		DBG_ERR("add rpl_stats fail");
		goto err;
	}
#endif

	return 0;

err:
	DBG_ERR("fail");
	debugfs_remove_recursive(phy_root);
	return -1;
}


void nand_debug_exit(void)
{
	debugfs_remove_recursive(nand_root);
	nand_root = NULL;
}

void nand_debug_init(int init_nftl)
{
	if (nand_root)
		return;

	nand_root = debugfs_create_dir(NAND_ROOT_NAME, NULL);
	if (IS_ERR(nand_root))
		/* Don't complain -- debugfs just isn't enabled */
		return;
	if (!nand_root)
		/* Complain -- debugfs is enabled, but it failed to
		 * create the directory. */
		goto err;

	if (add_phy_debug_info(nand_root))
		goto err;

	if (init_nftl)
		add_nftl_debug_info(nand_root);

	return;

err:
	DBG_ERR("fail");
	nand_debug_exit();
}
