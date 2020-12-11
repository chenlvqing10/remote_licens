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

#ifndef __NV_DRV__
#define __NV_DRV__

/* IO CTRL CMD */
#define CV_RUN				0
#define NN_RUN				1
#define NV_ENABLE_CLK			4
#define NV_DISABLE_CLK			5
#define NV_WRITE_REGISTER		6
#define NV_READ_REGISTER		7
#define GET_CV_STATUS			10
#define GET_NN_STATUS			11

/* NV STATUS */
enum {
	NN_IDLE = 0x10,
	NN_BUSY,
	NN_FINISH,
	NN_ERR,
	NN_CHECK_RESULT,
	CV_IDLE = 0x100,
	CV_BUSY,
	CV_FINISH,
	CV_ERR,
	CV_CHECK_RESULT
};

/* NV ERR Code */
enum {
	ERR_CMD = -2,
	NN_ERR_UNKOWN = -1,
	NN_TIMEOUT = -13,
	CV_ERR_UNKOWN = -100,
	CV_TIMEOUT = -103
};

typedef struct {
	unsigned int  addr;
	unsigned int  value;
} nv_register;

#endif

