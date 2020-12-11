/*
 * lombo_disp_tool.c
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
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/err.h>

struct disp_usb_cmd {
	u8 cmd;
	u8 flag;
	u32 addr;
	u32 len;
	u8  res[6];
} __packed;

struct disp_drm_info {
	int json_len;
	int gamma_len;
};

#define DISP_USB_CMD_GET_DRM_INFO	(0x90)
#define DISP_USB_CMD_GET_JSON		(0x91)
#define DISP_USB_CMD_GET_GAMMA		(0x92)

#define DISP_USB_CMD_SET_JSON		(0xa0)
#define DISP_USB_CMD_SET_GAMMA		(0xa1)

#define DISP_JSON_SRC_FILE		"/data/disp_config.json"
#define DISP_GAMMA_SRC_FILE		"/data/disp_gamma.bin"

#define GAMMA_TBL_SIZE			(1024)

#define DEBUG_LEVEL			KERN_DEBUG

static int disp_usb_read_file(const char *file_path, char *buf, u32 len)
{
	struct file *fp;
	mm_segment_t fs;
	loff_t pos = 0;

	fp = filp_open(file_path, O_RDWR, 0644);
	if (IS_ERR(fp)) {
		pr_err("open %s error\n", file_path);
		return -1;
	}
	fs = get_fs();
	set_fs(KERNEL_DS);
	vfs_read(fp, buf, len, &pos);
	filp_close(fp, NULL);
	set_fs(fs);
	printk(DEBUG_LEVEL "disp_usb_read_file[%s] len[%d]\n", file_path, len);

	return 0;
}

static int disp_usb_write_file(const char *file_path, char *buf, u32 len)
{
	struct file *fp;
	mm_segment_t fs;
	loff_t pos = 0;

	fp = filp_open(file_path, O_RDWR | O_CREAT | O_TRUNC, 0644);
	if (IS_ERR(fp)) {
		pr_err("open %s error\n", file_path);
		return -1;
	}
	fs = get_fs();
	set_fs(KERNEL_DS);
	vfs_write(fp, buf, len, &pos);
	filp_close(fp, NULL);
	set_fs(fs);
	printk(DEBUG_LEVEL "disp_usb_write_file[%s] len[%d]\n",
		file_path, len);

	return 0;
}

int disp_usb_read(void *cmd, void *data)
{
	int ret = 0;
	struct disp_usb_cmd *c = (struct disp_usb_cmd *)cmd;

	printk(DEBUG_LEVEL "disp_usb_read cmd[%02x]\n", c->res[1]);
	if (c->res[1] == DISP_USB_CMD_GET_DRM_INFO) {
		struct disp_drm_info *info = data;
		struct kstat st;

		memset(&st, 0x00, sizeof(st));
		vfs_stat(DISP_JSON_SRC_FILE, &st);
		info->json_len = st.size;

		memset(&st, 0x00, sizeof(st));
		vfs_stat(DISP_GAMMA_SRC_FILE, &st);
		info->gamma_len = st.size;

		if (info->gamma_len != GAMMA_TBL_SIZE) {
			pr_err("warning: file[%s] size[%d] is not %d\n",
				DISP_GAMMA_SRC_FILE, info->gamma_len,
				GAMMA_TBL_SIZE);
			pr_err("force to clear it to 0\n");
			info->gamma_len = 0;
		}

		printk(DEBUG_LEVEL "disp_usb_read: json_len[%d] gamma_len[%d]\n",
			info->json_len, info->gamma_len);
	} else if (c->res[1] == DISP_USB_CMD_GET_JSON)
		ret = disp_usb_read_file(DISP_JSON_SRC_FILE, data, c->len);
	else if (c->res[1] == DISP_USB_CMD_GET_GAMMA)
		ret = disp_usb_read_file(DISP_GAMMA_SRC_FILE, data, c->len);
	else {
		ret = -1;
		pr_err("disp_usb_read unsupport cmd\n");
	}

	return ret;
}
EXPORT_SYMBOL(disp_usb_read);

int disp_usb_write(void *cmd, void *data)
{
	int ret = 0;
	struct disp_usb_cmd *c = (struct disp_usb_cmd *)cmd;

	printk(DEBUG_LEVEL "disp_usb_write cmd[%02x]\n", c->res[1]);
	if (c->res[1] == DISP_USB_CMD_SET_JSON)
		ret = disp_usb_write_file(DISP_JSON_SRC_FILE, data, c->len);
	else if (c->res[1] == DISP_USB_CMD_SET_GAMMA)
		ret =  disp_usb_write_file(DISP_GAMMA_SRC_FILE, data, c->len);
	else {
		ret = -1;
		pr_err("disp_usb_write unsupport cmd\n");
	}

	return ret;
}
EXPORT_SYMBOL(disp_usb_write);
