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

#ifndef _LOMBO_DPU_H_
#define _LOMBO_DPU_H_

#include "csp_dpu_top.h"

#define LOMBO_DPU_EXT_SCLK_NUM 3
#define LOMBO_DPU_SCLK_NUM 3

enum {
	LOMBO_DPU_CLK_REQ_RATE = 1,
	LOMBO_DPU_CLK_REAL_RATE,
	LOMBO_DPU_CLK_PARENT_RATE,
};

struct lombo_dpu_clk;

struct lombo_dpu_clk *lombo_dpu_get_clk(struct device *dev,
	struct device *dpu_top, u32 mod_id);
int lombo_dpu_put_clk(struct lombo_dpu_clk **clk);
uint32_t lombo_dpu_clk_get_parent(struct lombo_dpu_clk *child);
int lombo_dpu_clk_set_parent(
	struct lombo_dpu_clk *child, uint32_t sclk_id);
unsigned long lombo_dpu_clk_get_rate(struct lombo_dpu_clk *clk, int which);
int lombo_dpu_clk_set_rate_g(struct lombo_dpu_clk *clk,
	unsigned long rate, int sync);
int lombo_dpu_clk_prepare_enable(struct lombo_dpu_clk *clk, uint8_t sw);
int lombo_dpu_clk_disable_unprepare(struct lombo_dpu_clk *clk);

/* get a enabled timer clock */
struct lombo_dpu_clk *lombo_dpu_get_enable_tck(struct device *dpu_top,
	uint32_t req_rate, uint32_t *real_rate);

#define lombo_dpu_clk_get_parent_rate(clk)	\
	lombo_dpu_clk_get_rate(clk, LOMBO_DPU_CLK_PARENT_RATE)
#define lombo_dpu_clk_get_req_rate(clk)	\
	lombo_dpu_clk_get_rate(clk, LOMBO_DPU_CLK_REQ_RATE)
#define lombo_dpu_clk_get_real_rate(clk)	\
	lombo_dpu_clk_get_rate(clk, LOMBO_DPU_CLK_REAL_RATE)

#define lombo_dpu_clk_set_rate(clk, rate)	\
	lombo_dpu_clk_set_rate_g(clk, rate, 0)
#define lombo_dpu_clk_set_rate_sync(clk, rate)	\
	lombo_dpu_clk_set_rate_g(clk, rate, 1)

#endif /* #ifndef _LOMBO_DPU_H_ */
