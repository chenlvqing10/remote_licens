/*
 * csp-lombo-n7v1.c - n7v1 module registers operations
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

#include <mach/csp.h>

/*
 * add the module source files if it was defined
 */

#ifdef DEF_N7V1_AC
#include "n7v1/ac/source.c"
#endif

#ifdef DEF_N7V1_AES
#include "n7v1/aes/source.c"
#endif

#ifdef DEF_N7V1_CPU_CTRL
#include "n7v1/cpu_ctrl/source.c"
#endif

#ifdef DEF_N7V1_DMAC
#include "n7v1/dmac/source.c"
#endif

#ifdef DEF_N7V1_DOSS_MIPI_DSI
#include "n7v1/doss/mipi_dsi/source.c"
#endif

#ifdef DEF_N7V1_DOSS_TCON0
#include "n7v1/doss/tcon0/source.c"
#endif

#ifdef DEF_N7V1_DOSS_TOP
#include "n7v1/doss/top/source.c"
#endif

#ifdef DEF_N7V1_GIC
#include "n7v1/gic/gicc/source.c"
#include "n7v1/gic/gicd/source.c"
#endif

#ifdef DEF_N7V1_GMAC
#include "n7v1/gmac/source.c"
#endif

#ifdef DEF_N7V1_GPADC
#include "n7v1/gpadc/source.c"
#endif

#ifdef DEF_N7V1_GPIO
#include "n7v1/gpio/source.c"
#endif

#ifdef DEF_N7V1_GTIMER
#include "n7v1/gtimer/source.c"
#endif

#ifdef DEF_N7V1_I2C
#include "n7v1/i2c/source.c"
#endif

#ifdef DEF_N7V1_I2S0
#include "n7v1/i2s/source.c"
#endif

#ifdef DEF_N7V1_I2S1
#include "n7v1/i2s1/source.c"
#endif

#ifdef DEF_N7V1_IR
#include "n7v1/ir/source.c"
#endif

#ifdef DEF_N7V1_MEM_CTRL
#include "n7v1/mem_ctrl/source.c"
#endif

#ifdef DEF_N7V1_PRCM
#include "n7v1/prcm/source.c"
#endif

#ifdef DEF_N7V1_PTIMER
#include "n7v1/ptimer/source.c"
#endif

#ifdef DEF_N7V1_PWM
#include "n7v1/pwm/source.c"
#endif

#ifdef DEF_N7V1_RTC
#include "n7v1/rtc/source.c"
#endif

#ifdef DEF_N7V1_SAU
#include "n7v1/sau/source.c"
#endif

#ifdef DEF_N7V1_SDC
#include "n7v1/sdc/source.c"
#endif

#ifdef DEF_N7V1_SPI
#include "n7v1/spi/source.c"
#endif

#ifdef DEF_N7V1_UART
#include "n7v1/uart/source.c"
#endif

#ifdef DEF_N7V1_VISS_TOP
#include "n7v1/viss/top/source.c"
#endif

#ifdef DEF_N7V1_VISS_TVD
#include "n7v1/viss/tvd/source.c"
#endif

#ifdef DEF_N7V1_VISS_VIC
#include "n7v1/viss/vic/source.c"
#endif

#ifdef DEF_N7V1_VISS_MIPI_CSI
#include "n7v1/viss/mipi_csi/source.c"
#endif

#ifdef DEF_N7V1_WDOG
#include "n7v1/wdog/source.c"
#endif
