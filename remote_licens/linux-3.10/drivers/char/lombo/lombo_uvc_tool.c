/*
 * lombo_uvc_tool.c
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

#define UVC_INFO_READ_JSON_FILE		"/data/uvc_info_write.json"
#define UVC_INFO_WRITE_JSON_FILE		"/data/uvc_info_read.json"
#define UVC_INFO_RESPONSE_STATUS    "/data/uvc_info_response_status.json"


#define GAMMA_TBL_SIZE			(1024)

#define DEBUG_LEVEL			KERN_DEBUG

struct uvc_usb_cmd {
	u8 cmd;
	u8 flag;
	u32 addr;
	u32 len;
	u8  res[6];
} __packed;

static int uvc_usb_read_file(const char *file_path, char *buf, u32 len)
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

	return 0;
}

static int uvc_usb_write_file(const char *file_path, char *buf, u32 len)
{
	struct file *fp;
	mm_segment_t fs;
	loff_t pos = 0;
	fp = filp_open(file_path, O_RDWR | O_CREAT, 0644);
	if (IS_ERR(fp)) {
		pr_err("open %s error\n", file_path);
		return -1;
	}
	fs = get_fs();
	set_fs(KERNEL_DS);
	vfs_write(fp, buf, len, &pos);
	filp_close(fp, NULL);
	set_fs(fs);

	return 0;
}

int uvc_usb_read(void *cmd, void *data)
{
	int ret = 0;
	struct uvc_usb_cmd *c = (struct uvc_usb_cmd *)cmd;
	if (c->res[0]) {
		ret = uvc_usb_read_file(UVC_INFO_RESPONSE_STATUS, data, c->len);
	} else {
		ret = uvc_usb_read_file(UVC_INFO_READ_JSON_FILE, data, c->len);
	}
	return ret;
}
EXPORT_SYMBOL(uvc_usb_read);

int uvc_usb_write(void *cmd, void *data)
{
	int ret = 0;
	struct uvc_usb_cmd *c = (struct uvc_usb_cmd *)cmd;
	ret = uvc_usb_write_file(UVC_INFO_WRITE_JSON_FILE, data, c->len);
	return ret;
}
EXPORT_SYMBOL(uvc_usb_write);
