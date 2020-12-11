/*
 *
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Authors:
 *	lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#ifndef __NN_REG_H__
#define __NN_REG_H__

#include <linux/init.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/io.h>

#define NN_CTL				(0x110)
#define NN_CMD_ADDR			(0x130)
#define NN_IRQ_EN			(0x120)
#define NN_IRQ_PEND			(0x124)
#define NN_IRQ_CLEAR			(0x128)

typedef struct {
	int  *cmd_param;
	int  cmd_ddr;
	int  nv_ctrl;
	int  fst_cmd_len;
} nn_param_t;

void nn_set_reg(void *regs_base, nn_param_t cmd_value);

#endif

