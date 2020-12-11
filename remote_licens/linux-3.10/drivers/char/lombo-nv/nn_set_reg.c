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

#include "nn_reg.h"

void nn_set_reg(void *regs_base, nn_param_t cmd_value)
{
	writel(cmd_value.cmd_ddr, regs_base + NN_CMD_ADDR);
	writel(cmd_value.nv_ctrl, regs_base + NN_CTL);
}

