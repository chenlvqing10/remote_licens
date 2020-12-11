/*
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Authors:
 *	lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#ifndef _LOMBO_VO_MANAGE_H_
#define _LOMBO_VO_MANAGE_H_

struct lombo_vo_host;

/**
 * struct lombo_vo_msg - read/write lombo disp-if buffer
 * @type: payload data type
 * @flags: flags controlling this message transmission
 * @tx_len: length of @tx_buf
 * @tx_buf: data to be written
 * @rx_len: length of @rx_buf
 * @rx_buf: data to be read, or NULL
 */
struct lombo_vo_msg {
	u8 type;
	u16 flags;

	size_t tx_len;
	void *tx_buf;

	size_t rx_len;
	void *rx_buf;
};

struct lombo_vo_host_ops {
	ssize_t (*transfer)(struct lombo_vo_host *host,
			    const struct lombo_vo_msg *msg);
};

typedef enum tag_lombo_vo_msg_type {
	LOMBO_CPU_IF_MSG_WR_IDX		= 0x0,
	LOMBO_CPU_IF_MSG_WR_PARA	= 0x1,
	LOMBO_CPU_IF_MSG_WR_REG		= 0x2,
	LOMBO_CPU_IF_MSG_RD_PARA	= 0x3,
	LOMBO_CPU_IF_MSG_RD_REG		= 0x4,
	LOMBO_CPU_IF_MSG_WR_GRAM	= 0x5,
	LOMBO_CPU_IF_MSG_RD_GRAM	= 0x6,
} lombo_vo_msg_type_t;

struct lombo_vo_host {
	struct device *dev;
	uint32_t host_id;
	const struct lombo_vo_host_ops *ops;
};

struct lombo_vo_device {
	struct lombo_vo_host *host;
};

#endif /* #ifndef _LOMBO_VO_MANAGE_H_ */
