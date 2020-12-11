/* SPDX-License-Identifier: GPL-2.0 */
/*
 * drivers/char/lombo/lombo_util.h
 *
 * Copyright (C) 2019 LB, Inc.
 */

#ifndef _LOMBO_UTIL_H
#define _LOMBO_UTIL_H

#include <linux/ioctl.h>
#include <linux/types.h>


struct util_fd2phy {
	__u32 fd;
	long phy_addr;
};

struct util_sync {
	__u32 fd;
};

#define LB_UTIL_MAGIC		'L'

#define LB_UTIL_FD2PHY		_IOWR(LB_UTIL_MAGIC, 0, \
					  struct util_fd2phy)

#define LB_UTIL_SYNC		_IOWR(LB_UTIL_MAGIC, 0, \
					  struct util_sync)

#endif /* _LOMBO_UTIL_H */
