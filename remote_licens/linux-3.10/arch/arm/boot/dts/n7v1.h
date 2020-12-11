/*
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

#ifndef __DTS_LOMBO_N7V1_H
#define __DTS_LOMBO_N7V1_H

#include <dt-bindings/interrupt-controller/arm-gic.h>

/*
 * CLOCK module
 */

#define CLK_32K			32768
#define CLK_24M			24000000

/*
 * DMA module
 */

/* DMA device request line number define */
#define DMA_MEM_RT_RP		0
#define DMA_SPI0_RX_SRP		1
#define DMA_SPI0_TX_DSP		2
#define DMA_SPI1_RX_SRP		3
#define DMA_SPI1_TX_DSP		4
#define DMA_SPI2_RX_SRP		5
#define DMA_SPI2_TX_DSP		6
#define DMA_I2C0_RX_SRP		7
#define DMA_I2C0_TX_DRP		8
#define DMA_I2C1_RX_SRP		9
#define DMA_I2C1_TX_DRP		10
#define DMA_I2C2_RX_SRP		11
#define DMA_I2C2_TX_DRP		12
#define DMA_I2C3_RX_SRP		13
#define DMA_I2C3_TX_DRP		14
#define DMA_UART0_RX_SRP	15
#define DMA_UART0_TX_DRP	16
#define DMA_UART1_RX_SRP	17
#define DMA_UART1_TX_DRP	18
#define DMA_UART2_RX_SRP	19
#define DMA_UART2_TX_DRP	20
#define DMA_UART3_RX_SRP	21
#define DMA_UART3_TX_DRP	22
#define DMA_I2S_RX_SRP		23
#define DMA_I2S_TX_DRP		24
#define DMA_TON_TX_DRP		26
#if defined(CONFIG_ARCH_LOMBO_N7V1)
#define DMA_I2S1_RX_SRP		30
#define DMA_I2S1_TX_DRP		31
#endif

/* DMA channel priority define, from 0 to 7, 0 is the lowest */
#define DMA_CHAN_PRIO_0		0
#define DMA_CHAN_PRIO_1		1
#define DMA_CHAN_PRIO_2		2
#define DMA_CHAN_PRIO_3		3
#define DMA_CHAN_PRIO_4		4
#define DMA_CHAN_PRIO_5		5
#define DMA_CHAN_PRIO_6		6
#define DMA_CHAN_PRIO_7		7

/*
 * Pinctrl module
 */

/* pull up or pull down define */
#define DISABLE_PULL_UP_DOWN	0
#define ENABLE_PULL_UP		1
#define ENABLE_PULL_DOWN	2

/* driver configuration */
#define DRV_LEVEL0	0
#define DRV_LEVEL1	1
#define DRV_LEVEL2	2
#define DRV_LEVEL3	3
#define DRV_LEVEL4	4
#define DRV_LEVEL5	5
#define DRV_LEVEL6	6
#define DRV_LEVEL7	7

/* pull up/down resistor */
#define PULL_100K	0
#define PULL_20K	1

/* gpio interrupt sample time */
#define CLK_RTCCLK	0
#define CLK_HFEOSC	1

#define CLK_DIV_1	0
#define CLK_DIV_2	1
#define CLK_DIV_4	2
#define CLK_DIV_8	3
#define CLK_DIV_16	4
#define CLK_DIV_32	5
#define CLK_DIV_64	6
#define CLK_DIV_128	7
#define CLK_DIV_256	8
#define CLK_DIV_512	9
#define CLK_DIV_1024	10

/*
 * IRQ module
 */

/* irq number configurations in "interrupts" field */
#define SPI_IRQ(n)	((n) - 32)	/* n is the irq number in spec */
#define PPI_IRQ(n)	((n) - 16)

/* irq number which defined in spec */
/* #define INT_RSVD		SPI_IRQ(32) */
#define INT_WDOG		SPI_IRQ(33)
#define INT_RTC			SPI_IRQ(34)
#define INT_GTIMER0		SPI_IRQ(35)
#define INT_GTIMER1		SPI_IRQ(36)
#define INT_GTIMER2		SPI_IRQ(37)
#define INT_GTIMER3		SPI_IRQ(38)
/* #define INT_RSVD		SPI_IRQ(39) */
/* #define INT_RSVD		SPI_IRQ(40) */
#define INT_PWM			SPI_IRQ(41)
#define INT_DMA			SPI_IRQ(42)
#define INT_PTIMER0		SPI_IRQ(43)
#define INT_PTIMER1		SPI_IRQ(44)
/* #define INT_RSVD		SPI_IRQ(45) */
/* #define INT_RSVD		SPI_IRQ(46) */
#define INT_I2C0		SPI_IRQ(47)
#define INT_I2C1		SPI_IRQ(48)
#define INT_I2C2		SPI_IRQ(49)
#define INT_I2C3		SPI_IRQ(50)
/* #define INT_RSVD		SPI_IRQ(51) */
/* #define INT_RSVD		SPI_IRQ(52) */
#define INT_SPI0		SPI_IRQ(53)
#define INT_SPI1		SPI_IRQ(54)
#define INT_SPI2		SPI_IRQ(55)
/* #define INT_RSVD		SPI_IRQ(56) */
/* #define INT_RSVD		SPI_IRQ(57) */
/* #define INT_RSVD		SPI_IRQ(58) */
#define INT_UART0		SPI_IRQ(59)
#define INT_UART1		SPI_IRQ(60)
#define INT_UART2		SPI_IRQ(61)
#define INT_UART3		SPI_IRQ(62)
/* #define INT_RSVD		SPI_IRQ(63) */
/* #define INT_RSVD		SPI_IRQ(64) */
#define INT_GPADC		SPI_IRQ(65)
/* #define INT_RSVD		SPI_IRQ(66) */
#define INT_USB20		SPI_IRQ(67)
/* #define INT_RSVD		SPI_IRQ(68) */
/* #define INT_RSVD		SPI_IRQ(69) */
/* #define INT_RSVD		SPI_IRQ(70) */
/* #define INT_RSVD		SPI_IRQ(71) */
/* #define INT_RSVD		SPI_IRQ(72) */
#define INT_SIO			SPI_IRQ(73)
#define INT_GPIO0		SPI_IRQ(74)
#define INT_GPIO1		SPI_IRQ(75)
#define INT_GPIO2		SPI_IRQ(76)
/* #define INT_RSVD		SPI_IRQ(77) */
/* #define INT_RSVD		SPI_IRQ(78) */
/* #define INT_RSVD		SPI_IRQ(79) */
#define INT_SDC0		SPI_IRQ(80)
#define INT_SDC1		SPI_IRQ(81)
#define INT_SDC2		SPI_IRQ(82)
/* #define INT_RSVD		SPI_IRQ(83) */
#define INT_SDRAM		SPI_IRQ(84)
#define INT_I2S			SPI_IRQ(85)
#define INT_I2S1		SPI_IRQ(86)
/* #define INT_RSVD		SPI_IRQ(87) */
#define INT_AC			SPI_IRQ(88)
/* #define INT_RSVD		SPI_IRQ(89) */
#define INT_VC			SPI_IRQ(90)
#define INT_VDC			SPI_IRQ(91)
#define INT_TCON		SPI_IRQ(92)
/* #define INT_RSVD		SPI_IRQ(93) */
/* #define INT_RSVD		SPI_IRQ(94) */
/* #define INT_RSVD		SPI_IRQ(95) */
/* #define INT_RSVD		SPI_IRQ(96) */
#define INT_MIPI_DSI		SPI_IRQ(97)
/* #define INT_RSVD		SPI_IRQ(98) */
#define INT_TVE			SPI_IRQ(99)
/* #define INT_RSVD		SPI_IRQ(100) */
/* #define INT_RSVD		SPI_IRQ(101) */
#define INT_VIC			SPI_IRQ(102)
#define INT_MIPI_CSI		SPI_IRQ(103)
#define INT_AX_ENU		SPI_IRQ(104)
#define INT_AX_CVU		SPI_IRQ(105)
#define INT_ISP0		SPI_IRQ(106)
#define INT_ISP_FE0		SPI_IRQ(107)
#define INT_ISP1		SPI_IRQ(108)
#define INT_ISP_FE1		SPI_IRQ(109)
/* #define INT_RSVD		SPI_IRQ(110) */
/* #define INT_RSVD		SPI_IRQ(111) */
#define INT_TVD			SPI_IRQ(112)
/* #define INT_RSVD		SPI_IRQ(113) */
/* #define INT_RSVD		SPI_IRQ(114) */
/* #define INT_RSVD		SPI_IRQ(115) */
#define INT_DC0			SPI_IRQ(116)
#define INT_DC1			SPI_IRQ(117)
#define INT_SE0			SPI_IRQ(118)
#define INT_SE1			SPI_IRQ(119)
#define INT_DIT			SPI_IRQ(120)
/* #define INT_RSVD		SPI_IRQ(121) */
#define INT_ROT			SPI_IRQ(122)
/* #define INT_RSVD		SPI_IRQ(123) */
#define INT_AXIERR		SPI_IRQ(124)
#define INT_nPMUIRQ0		SPI_IRQ(125)
#define INT_COMMRX0		SPI_IRQ(126)
#define INT_COMMTX0		SPI_IRQ(127)
#define INT_nPMUIRQ1		SPI_IRQ(128)
#define INT_COMMRX1		SPI_IRQ(129)
#define INT_COMMTX1		SPI_IRQ(130)
/* #define INT_RSVD		SPI_IRQ(131) */
/* #define INT_RSVD		SPI_IRQ(132) */
/* #define INT_RSVD		SPI_IRQ(133) */
/* #define INT_RSVD		SPI_IRQ(134) */
/* #define INT_RSVD		SPI_IRQ(135) */
/* #define INT_RSVD		SPI_IRQ(136) */
/* #define INT_RSVD		SPI_IRQ(137) */
/* #define INT_RSVD		SPI_IRQ(138) */
/* #define INT_RSVD		SPI_IRQ(139) */
/* #define INT_RSVD		SPI_IRQ(140) */
/* #define INT_RSVD		SPI_IRQ(141) */
/* #define INT_RSVD		SPI_IRQ(142) */
#define INT_IR_RX		SPI_IRQ(143)
/* #define INT_RSVD		SPI_IRQ(144) */
#define INT_GMAC		SPI_IRQ(145)


#endif /* __DTS_LOMBO_N7V1_H */
