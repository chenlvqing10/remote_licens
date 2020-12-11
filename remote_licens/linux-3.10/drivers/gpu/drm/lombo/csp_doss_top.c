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

#ifndef __CSP_DOSS_TOP_C__
#define __CSP_DOSS_TOP_C__

#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/io.h>

#include "csp_doss_top.h"

#define WRITEREG32(reg, val)		writel(val, (void __iomem *)reg)
#define READREG32(reg)			readl((void __iomem *)reg)

static reg_doss_t *reg_doss[DOSS_MAX_INST] = { NULL };

void csp_doss_set_register_base(u32 doss_idx, void *addr, u32 size)
{
	reg_doss[doss_idx] = (reg_doss_t *)addr;
}

/* Get register base address */
void csp_doss_get_register_base(u32 doss_idx, unsigned long *addr)
{
	*addr = (unsigned long)reg_doss[doss_idx];
}

void csp_doss_init(u32 doss_idx)
{
	/* doss reset release */
	reg_doss[doss_idx]->reset.bits.rst = DOSS_RESET_RST_0;
	udelay(1);
	reg_doss[doss_idx]->reset.bits.rst = DOSS_RESET_RST_1;
}

void csp_doss_exit(u32 doss_idx)
{
	/* doss reset hold */
	reg_doss[doss_idx]->reset.bits.rst = DOSS_RESET_RST_0;
}

static u32 get_reg_clk_offset(u32 path_id, doss_clk_id_t clk_id)
{
	u32 reg_offset;
	reg_offset = clk_id + DOSS_PATH_SIZE * path_id;
	return reg_offset;
}

void csp_doss_clk_enable(u32 doss_idx, u32 path_id, doss_clk_id_t clk_id)
{
	reg_doss_path_clk_t reg_clk;
	ulong reg_addr;

	reg_addr = (ulong)(reg_doss[doss_idx]) +
		get_reg_clk_offset(path_id, clk_id);
	reg_clk.val = READREG32(reg_addr);
	reg_clk.bits.gate = DOSS_CLK_ON;
	WRITEREG32(reg_addr, reg_clk.val);
}

void csp_doss_clk_disable(u32 doss_idx, u32 path_id, doss_clk_id_t clk_id)
{
	reg_doss_path_clk_t reg_clk;
	ulong reg_addr;

	reg_addr = (ulong)(reg_doss[doss_idx]) +
		get_reg_clk_offset(path_id, clk_id);
	reg_clk.val = READREG32(reg_addr);
	reg_clk.bits.gate = DOSS_CLK_OFF;
	WRITEREG32(reg_addr, reg_clk.val);
}

void csp_doss_clk_set(u32 doss_idx, u32 path_id, doss_clk_id_t clk_id,
	doss_clk_src_t src, u8 div0, u8 div1)
{
	reg_doss_path_clk_t reg_clk;
	ulong reg_addr;

	reg_addr = (ulong)(reg_doss[doss_idx]) +
		get_reg_clk_offset(path_id, clk_id);
	reg_clk.val = READREG32(reg_addr);
	reg_clk.bits.src = src;
	reg_clk.bits.div0 = div0;
	reg_clk.bits.div1 = div1;
	WRITEREG32(reg_addr, reg_clk.val);
}

void csp_doss_clk_src_set(u32 doss_idx, u32 path_id,
	doss_clk_id_t clk_id, doss_clk_src_t src)
{
	reg_doss_path_clk_t reg_clk;
	ulong reg_addr;
	reg_addr = (ulong)(reg_doss[doss_idx]) +
		get_reg_clk_offset(path_id, clk_id);
	reg_clk.val = READREG32(reg_addr);
	reg_clk.bits.src = src;
	WRITEREG32(reg_addr, reg_clk.val);
}

static u32 get_reg_ctrl_offset(u32 path_id, doss_ctrl_id_t ctrl_id)
{
	u32 reg_offset;
	reg_offset = ctrl_id + DOSS_PATH_SIZE * path_id;
	return reg_offset;
}

void csp_doss_path_enable(u32 doss_idx, u32 path_id, doss_ctrl_id_t ctrl_id)
{
	reg_doss_path_ctrl_t reg_ctrl;
	ulong reg_addr;

	reg_addr = (ulong)(reg_doss[doss_idx]) +
		get_reg_ctrl_offset(path_id, ctrl_id);
	reg_ctrl.val = READREG32(reg_addr);
	reg_ctrl.bits.gate = DOSS_CTRL_ON;
	WRITEREG32(reg_addr, reg_ctrl.val);
}

void csp_doss_path_disable(u32 doss_idx, u32 path_id, doss_ctrl_id_t ctrl_id)
{
	reg_doss_path_ctrl_t reg_ctrl;
	ulong reg_addr;

	reg_addr = (ulong)(reg_doss[doss_idx]) +
		get_reg_ctrl_offset(path_id, ctrl_id);
	reg_ctrl.val = READREG32(reg_addr);
	reg_ctrl.bits.gate = DOSS_CTRL_OFF;
	WRITEREG32(reg_addr, reg_ctrl.val);
}

void csp_doss_slave_enable(u32 doss_idx, u32 path_id, doss_ctrl_id_t ctrl_id)
{
	reg_doss_path_ctrl_t reg_ctrl;
	ulong reg_addr;
	reg_addr = (ulong)(reg_doss[doss_idx]) +
		get_reg_ctrl_offset(path_id, ctrl_id);
	reg_ctrl.val = READREG32(reg_addr);
	reg_ctrl.bits.slave = DOSS_CTRL_ON;
	WRITEREG32(reg_addr, reg_ctrl.val);
}

void csp_doss_slave_disable(u32 doss_idx, u32 path_id, doss_ctrl_id_t ctrl_id)
{
	reg_doss_path_ctrl_t reg_ctrl;
	ulong reg_addr;
	reg_addr = (ulong)(reg_doss[doss_idx]) +
		get_reg_ctrl_offset(path_id, ctrl_id);
	reg_ctrl.val = READREG32(reg_addr);
	reg_ctrl.bits.slave = DOSS_CTRL_OFF;
	WRITEREG32(reg_addr, reg_ctrl.val);
}

void csp_doss_path_init(u32 doss_idx, u32 path_id, doss_ctrl_id_t ctrl_id)
{
	reg_doss_path_ctrl_t reg_ctrl;
	ulong reg_addr;

	reg_addr = (ulong)(reg_doss[doss_idx]) +
		get_reg_ctrl_offset(path_id, ctrl_id);
	reg_ctrl.val = READREG32(reg_addr);
	reg_ctrl.bits.rst = DOSS_RESET_RST_0;
	WRITEREG32(reg_addr, reg_ctrl.val);
	udelay(1);
	reg_ctrl.val = READREG32(reg_addr);
	reg_ctrl.bits.rst = DOSS_RESET_RST_1;
	WRITEREG32(reg_addr, reg_ctrl.val);
}

void csp_doss_path_rst(u32 doss_idx, u32 path_id, doss_ctrl_id_t ctrl_id)
{
	reg_doss_path_ctrl_t reg_ctrl;
	ulong reg_addr;
	reg_addr = (ulong)(reg_doss[doss_idx]) +
		get_reg_ctrl_offset(path_id, ctrl_id);
	reg_ctrl.val = READREG32(reg_addr);
	reg_ctrl.bits.rst = DOSS_RESET_RST_1;
	WRITEREG32(reg_addr, reg_ctrl.val);
}

void csp_doss_path_exit(u32 doss_idx, u32 path_id, doss_ctrl_id_t ctrl_id)
{
	reg_doss_path_ctrl_t reg_ctrl;
	ulong reg_addr;
	reg_addr = (ulong)(reg_doss[doss_idx]) +
		get_reg_ctrl_offset(path_id, ctrl_id);
	reg_ctrl.val = READREG32(reg_addr);
	reg_ctrl.bits.rst = DOSS_RESET_RST_0;
	WRITEREG32(reg_addr, reg_ctrl.val);
}

void csp_doss_set_input(u32 doss_idx, u32 path_id, doss_input_t input)
{
	switch (path_id) {
	case 0:
		reg_doss[doss_idx]->route.bits.src_do0 = input;
	default:
		break;
	}
}

void csp_doss_io_set_output(u32 doss_idx, u32 func_id)
{
#ifdef CONFIG_ARCH_LOMBO_N7_FPGA
	u32 rdval;
	u32 reg_addr;

	reg_addr = (u32)(reg_doss[doss_idx]) + 0x0C;
	rdval = READREG32(reg_addr);

	switch (func_id) {
	case 0:
	case 1:
	case 2:
	case 3:
		break;
	case 4:
		WRITEREG32(reg_addr, rdval & (~0x08));
		break;
	case 5:
		break;
	default:
		break;
	}
#else

#endif
}

void csp_doss_io_set_val(u32 doss_idx, u32 func_id, u32 val)
{
#ifdef CONFIG_ARCH_LOMBO_N7_FPGA
	u32 rdval;
	u32 reg_addr;

	reg_addr = (u32)(reg_doss[doss_idx]) + 0x08;
	rdval = READREG32(reg_addr);

	val = val & 0x1;
	switch (func_id) {
	case 0:
		if (0 == val)
			WRITEREG32(reg_addr, rdval & (~0x20));
		else
			WRITEREG32(reg_addr, rdval | 0x20);
		break;
	case 1:
		if (0 == val)
			WRITEREG32(reg_addr, rdval & (~0x10));
		else
			WRITEREG32(reg_addr, rdval | 0x10);
		break;
	case 2:
		if (0 == val)
			WRITEREG32(reg_addr, rdval & (~0x04));
		else
			WRITEREG32(reg_addr, rdval | 0x04);
		break;
	case 3:
		if (0 == val)
			WRITEREG32(reg_addr, rdval & (~0x02));
		else
			WRITEREG32(reg_addr, rdval | 0x02);
		break;
	case 4:
		if (0 == val)
			WRITEREG32(reg_addr, rdval & (~0x01));
		else
			WRITEREG32(reg_addr, rdval | 0x01);
		break;
	case 5:
		if (0 == val)
			WRITEREG32(reg_addr, rdval & (~0x40));
		else
			WRITEREG32(reg_addr, rdval | 0x40);
		break;
	default:
		break;
	}
#else

#endif
}

void csp_doss_intf_vref_enable(u32 doss_idx, u32 intf_id)
{
	if (intf_id == 0)
		reg_doss[doss_idx]->intf0_ldo_ctrl.bits.en_vref =
					DOSS_INTF0_LDO_CTRL_EN_VREF_1;
	else
		reg_doss[doss_idx]->intf1_ldo_ctrl.bits.en_vref =
					DOSS_INTF1_LDO_CTRL_EN_VREF_1;

}

void csp_doss_intf_vref_disable(u32 doss_idx, u32 intf_id)
{
	if (intf_id == 0)
		reg_doss[doss_idx]->intf0_ldo_ctrl.bits.en_vref =
					DOSS_INTF0_LDO_CTRL_EN_VREF_0;
	else
		reg_doss[doss_idx]->intf1_ldo_ctrl.bits.en_vref =
					DOSS_INTF1_LDO_CTRL_EN_VREF_0;

}

#endif /* __CSP_DOSS_TOP_C__ */
