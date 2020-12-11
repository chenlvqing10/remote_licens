/*
 * csp-n7v1.h - register operation head file for n7v1
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

#ifndef __CSP_N7V1_H
#define __CSP_N7V1_H

#include "soc_define.h"

/*
 * macros for module selection.
 * open the macro if you need the module for compiling
 */

//#define DEF_N7V1_AC
//#define DEF_N7V1_AES
#define DEF_N7V1_CPU_CTRL
//#define DEF_N7V1_DMAC
//#define DEF_N7V1_DOSS_MIPI_DSI
//#define DEF_N7V1_DOSS_TCON0
#define DEF_N7V1_DOSS_TOP
#define DEF_N7V1_GIC
//#define DEF_N7V1_GMAC
#define DEF_N7V1_GPADC
#define DEF_N7V1_GPIO
#define DEF_N7V1_GTIMER
//#define DEF_N7V1_I2C
//#define DEF_N7V1_I2S0
////#define DEF_N7V1_I2S1
#define DEF_N7V1_IR
//#define DEF_N7V1_MEM_CTRL
#define DEF_N7V1_PRCM
#define DEF_N7V1_PTIMER
#define DEF_N7V1_PWM
#define DEF_N7V1_RTC
//#define DEF_N7V1_SAU
//#define DEF_N7V1_SDC
//#define DEF_N7V1_SPI
//#define DEF_N7V1_UART
#define DEF_N7V1_VISS_TOP
//#define DEF_N7V1_VISS_TVD
#define DEF_N7V1_VISS_VIC
//#define DEF_N7V1_VISS_MIPI_CSI
#define DEF_N7V1_WDOG

/*
 * other common defination for all modules can be put here..
 */

/*
 * add the module head files if it was defined
 */

#ifdef DEF_N7V1_AC
#include "../../../n7v1/ac/include.h"
#endif

#ifdef DEF_N7V1_AES
#include "../../../n7v1/aes/include.h"
#endif

#ifdef DEF_N7V1_CPU_CTRL
#include "../../../n7v1/cpu_ctrl/include.h"
#endif

#ifdef DEF_N7V1_DMAC
#include "../../../n7v1/dmac/include.h"
#endif

#ifdef DEF_N7V1_DOSS_MIPI_DSI
#include "../../../n7v1/doss/mipi_dsi/include.h"
#endif

#ifdef DEF_N7V1_DOSS_TCON0
#include "../../../n7v1/doss/tcon0/include.h"
#endif

#ifdef DEF_N7V1_DOSS_TOP
#include "../../../n7v1/doss/top/include.h"
#endif

#ifdef DEF_N7V1_GIC
#include "../../../n7v1/gic/gicc/include.h"
#include "../../../n7v1/gic/gicd/include.h"
#endif

#ifdef DEF_N7V1_GMAC
#include "../../../n7v1/gmac/include.h"
#endif

#ifdef DEF_N7V1_GPADC
#include "../../../n7v1/gpadc/include.h"
#endif

#ifdef DEF_N7V1_GPIO
#include "../../../n7v1/gpio/include.h"
#endif

#ifdef DEF_N7V1_GTIMER
#include "../../../n7v1/gtimer/include.h"
#endif

#ifdef DEF_N7V1_I2C
#include "../../../n7v1/i2c/include.h"
#endif

#ifdef DEF_N7V1_I2S0
#include "../../../n7v1/i2s/include.h"
#endif

#ifdef DEF_N7V1_I2S1
#include "../../../n7v1/i2s1/include.h"
#endif

#ifdef DEF_N7V1_IR
#include "../../../n7v1/ir/include.h"
#endif

#ifdef DEF_N7V1_MEM_CTRL
#include "../../../n7v1/mem_ctrl/include.h"
#endif

#ifdef DEF_N7V1_PRCM
#include "../../../n7v1/prcm/include.h"
#endif

#ifdef DEF_N7V1_PTIMER
#include "../../../n7v1/ptimer/include.h"
#endif

#ifdef DEF_N7V1_PWM
#include "../../../n7v1/pwm/include.h"
#endif

#ifdef DEF_N7V1_RTC
#include "../../../n7v1/rtc/include.h"
#endif

#ifdef DEF_N7V1_SAU
#include "../../../n7v1/sau/include.h"
#endif

#ifdef DEF_N7V1_SDC
#include "../../../n7v1/sdc/include.h"
#endif

#ifdef DEF_N7V1_SPI
#include "../../../n7v1/spi/include.h"
#endif

#ifdef DEF_N7V1_UART
#include "../../../n7v1/uart/include.h"
#endif

#ifdef DEF_N7V1_VISS_TOP
#include "../../../n7v1/viss/top/include.h"
#endif

#ifdef DEF_N7V1_VISS_TVD
#include "../../../n7v1/viss/tvd/include.h"
#endif

#ifdef DEF_N7V1_VISS_VIC
#include "../../../n7v1/viss/vic/include.h"
#endif

#ifdef DEF_N7V1_VISS_MIPI_CSI
#include "../../../n7v1/viss/mipi_csi/include.h"
#endif

#ifdef DEF_N7V1_WDOG
#include "../../../n7v1/wdog/include.h"
#endif

#endif /* __CSP_N7V1_H */
