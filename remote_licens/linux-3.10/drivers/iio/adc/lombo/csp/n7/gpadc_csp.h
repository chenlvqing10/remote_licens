/*
 * gpadc_csp.h - gpadc csp operation header
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
 */

#ifndef __GPADC_CSP_H___
#define __GPADC_CSP_H___

#include <mach/csp.h>

/* GPADC register offset */
#define GPADC_GPAEN_OFF			(0x00000010)
#define GPADC_GPAC_OFF			(0x00000014)
#define GPADC_GPAIE_OFF			(0x00000018)
#define GPADC_GPAIS_OFF			(0x0000001C)
#define GPADC_GPAIC_OFF			(0x00000020)
#define GPADC_CDAT_OFF			(0x00000024)
#define GPADC_AIN0DAT_OFF		(0x00000030)
#define GPADC_AUX0HDAT_OFF		(0x00000034)
#define GPADC_AUX0TDAT_OFF		(0x00000038)
#define GPADC_AIN1DAT_OFF		(0x00000040)
#define GPADC_AUX1HDAT_OFF		(0x00000044)
#define GPADC_AUX1TDAT_OFF		(0x00000048)
#define GPADC_GSC_OFF			(0x00000070)
#define GPADC_S0CDAT_OFF		(0x00000080)
#define GPADC_S1CDAT_OFF		(0x00000084)
#define GPADC_S2CDAT_OFF		(0x00000088)
#define GPADC_S0DAT_OFF			(0x00000090)
#define GPADC_S0UTDAT_OFF		(0x00000094)
#define GPADC_S0DTDAT_OFF		(0x00000098)
#define GPADC_S1DAT_OFF			(0x000000A0)
#define GPADC_S1UTDAT_OFF		(0x000000A4)
#define GPADC_S1DTDAT_OFF		(0x000000A8)
#define GPADC_S2DAT_OFF			(0x000000B0)
#define GPADC_S2UTDAT_OFF		(0x000000B4)
#define GPADC_S2DTDAT_OFF		(0x000000B8)
#define GPADC_DEBUG0			(0x000000F0)
#define GPADC_DEBUG1			(0x000000F4)

#define GPADC_FIOC_OFF			(0x00000100)
#define GPADC_FPUD_OFF			(0x00000104)
#define GPADC_FDRV_OFF			(0x00000108)
#define GPADC_FDAT_OFF			(0x0000010C)

/* AUXIN threshold*/
#define AUXIN_T_VALUE	0xff0

/* GPADC bit */

/* Sensor data  */
#define SENSOR2_DATA		BIT(22)
#define SENSOR1_DATA		BIT(21)
#define SENSOR0_DATA		BIT(20)

/* AUXIN data */
#define AUX1_DATA		BIT(17)
#define AUX0_DATA		BIT(16)

/* Sensor up threshold and down threshold */
#define SENSOR2_UP_THD		BIT(13)
#define SENSOR2_DOWN_THD	BIT(12)
#define SENSOR1_UP_THD		BIT(11)
#define SENSOR1_DOWN_THD	BIT(10)
#define SENSOR0_UP_THD		BIT(9)
#define SENSOR0_DOWN_THD	BIT(8)

/* AUXIN threshold1 and threshold0 */
#define AUX1_THD1		BIT(3)
#define AUX1_THD0		BIT(2)
#define AUX0_THD1		BIT(1)
#define AUX0_THD0		BIT(0)


#define GPADC_ALL_BIT			(SENSOR2_DATA |\
					SENSOR1_DATA |\
					SENSOR0_DATA |\
					AUX1_DATA |\
					AUX0_DATA |\
					SENSOR2_UP_THD |\
					SENSOR2_DOWN_THD |\
					SENSOR1_UP_THD |\
					SENSOR1_DOWN_THD |\
					SENSOR0_UP_THD |\
					SENSOR0_DOWN_THD |\
					AUX1_THD1 |\
					AUX1_THD0 |\
					AUX0_THD1 |\
					AUX0_THD0)


typedef enum {
	GPADC_SRC_1KHZ = 0x0,
	GPADC_SRC_512HZ = 0x1,
	GPADC_SRC_256HZ = 0x2,
	GPADC_SRC_128HZ = 0x3,
	GPADC_SRC_1MHZ = 0x5,
} GPADC_SRC_TYPE;

typedef enum {
	SAMPLE_RATE_8HZ = 0x0,
	SAMPLE_RATE_4HZ = 0x1,
	SAMPLE_RATE_2HZ = 0x2,
	SAMPLE_RATE_1HZ = 0x3,
} SENSOR_SAMPLE_RATE;

typedef enum {
	GPADC_CLK_24M = 0x0,
	GPADC_CLK_16M = 0x1,
} GPADC_CLK_SRC;

typedef enum {
	AUXIN_CHAN_0 = 0,
	AUXIN_CHAN_1 = 1,
} AUXIN_CHAN;

typedef enum {
	AUXIN_SH_DATA_0 = 0,
	AUXIN_SH_DATA_1 = 1,
} AUXIN_SH_DATA;

typedef enum {
	AUXIN_TH_DATA_0 = 0,
	AUXIN_TH_DATA_1 = 1,
} AUXIN_TH_DATA;

typedef enum {
	SENSOR_CHAN_0 = 0,
	SENSOR_CHAN_1 = 1,
	SENSOR_CHAN_2 = 2,
} SENSOR_CHAN;

typedef enum {
	IO_AIN_0 = 0,
	IO_AIN_1 = 1,
} IO_AIN;

typedef enum {
	IO_FUNC_AIN = 0x0,
	IO_FUNC_INPUT = 0x1,
	IO_FUNC_OUTPUT = 0x2,
} IO_AIN_FUNC;

typedef enum {
	IO_PUD_DIS_PU_PD = 0x0,
	IO_PUD_EN_PU = 0x1,
	IO_PUD_EN_PD = 0x2,
} IO_AIN_PUD;

typedef enum {
	IO_DRV_LEVEL_0 = 0x0,
	IO_DRV_LEVEL_1 = 0x1,
	IO_DRV_LEVEL_2 = 0x2,
	IO_DRV_LEVEL_3 = 0x3,
	IO_DRV_LEVEL_4 = 0x4,
	IO_DRV_LEVEL_5 = 0x5,
	IO_DRV_LEVEL_6 = 0x6,
	IO_DRV_LEVEL_7 = 0x7,
} IO_AIN_DRV;

void csp_gpadc_set_en(void *base, bool en);
void csp_gpadc_set_cali_en(void *base, bool en);
void csp_gpadc_set_avg_en(void *base, bool en);
void csp_gpadc_set_clk(void *base, GPADC_CLK_SRC clk);
void csp_gpadc_set_sample_rate(void *base, GPADC_SRC_TYPE t);
void csp_gpadc_set_int_en(void *base, u32 source, bool en);
u32 csp_gpadc_get_int_pend(void *base, u32 source);
void csp_gpadc_clr_int_pend(void *base, u32 source);

/* AUXIN */
void csp_auxin_set_en(void *base, AUXIN_CHAN chan, bool en);
u32 csp_auxin_get_data(void *base, AUXIN_CHAN chan);
u32 csp_aux_get_sh_data(void *base, AUXIN_CHAN chan);
void csp_aux_set_th_data(void *base, AUXIN_CHAN chan, u32 val);

/* Sensor */
void csp_sensor_set_en(void *base, bool en);
void csp_sensor_set_avg_en(void *base, bool en);
void csp_sensor_set_sample_rate(void *base, SENSOR_SAMPLE_RATE sr);
u32 csp_sensor_get_cali_data(void *base, SENSOR_CHAN chan);
u32 csp_sensor_get_data(void *base, SENSOR_CHAN chan);
void csp_sensor_set_dth_data(void *base, SENSOR_CHAN chan, u32 val);
void csp_sensor_set_uth_data(void *base, SENSOR_CHAN chan, u32 val);

/* IO */
void csp_io_set_func(void *base, IO_AIN ain, IO_AIN_FUNC func);
void csp_io_set_pud(void *base, IO_AIN ain, IO_AIN_PUD pud);
void csp_io_set_drv(void *base, IO_AIN ain, IO_AIN_DRV drv);
u32 csp_io_get_data(void *base, IO_AIN ain);

#endif /* __GPADC_CSP_H___ */

