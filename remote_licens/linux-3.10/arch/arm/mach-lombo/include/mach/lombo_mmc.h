/*
 * lombo_mmc.h - Extend definitions of Lombo mmc driver
 *
 * Copyright (C) 2019, LomboTech Co.Ltd.
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

#ifndef __LOMBO_MMC_H__
#define __LOMBO_MMC_H__

/**
 * lbsdc_scan_card - for sdio device driver to trigger card detect.
 * @on: 0, power off; 1, power on;
 *
 * return 0 if success; otherwise failed.
 */
int lbsdc_scan_card(u32 on);

#endif /* __LOMBO_MMC_H__ */
