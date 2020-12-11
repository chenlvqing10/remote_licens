/*
 * lombo.c - Common code for LomboTech Socs
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

#include <linux/clocksource.h>
#include <linux/clk-provider.h>
#include <linux/of_platform.h>
#include <linux/of_fdt.h>
#include <linux/memblock.h>
#include <linux/bug.h>
#include <asm/mach/map.h>
#include <asm/mach/arch.h>
#include <asm/setup.h>

#include <mach/rtc_power.h>
#include <mach/common.h>
#include <mach/csp.h>
#include "lombo.h"

#if 0
static void lombo_restart(char mode, const char *cmd)
{
	/* TODO--- */
}

static struct of_device_id lombo_restart_ids[] = {
	{ .compatible = "lombotech,lombo-wdt", .data = lombo_restart },
	{ /*sentinel*/ }
};
#endif

static struct map_desc lombo_io_desc[] __initdata = {
	{
		.virtual	= (unsigned long)LOMBO_IO_AHB_VBASE,
		.pfn		= __phys_to_pfn(LOMBO_IO_AHB_PBASE),
		.length		= LOMBO_IO_AHB_SIZE,
		.type		= MT_DEVICE,
	}, {
		.virtual	= (unsigned long)LOMBO_IO_APB_VBASE,
		.pfn		= __phys_to_pfn(LOMBO_IO_APB_PBASE),
		.length		= LOMBO_IO_APB_SIZE,
		.type		= MT_DEVICE,
	}, {
		.virtual	= (unsigned long)LOMBO_SRAM_VBASE,
		.pfn		= __phys_to_pfn(LOMBO_SRAM_PBASE),
		.length		= LOMBO_SRAM_SIZE,
		.type		= MT_MEMORY,
	}
};

#ifdef CONFIG_ARCH_LOMBO_N7V1
static int dma_status;

static int __init early_dma_status(char *arg)
{
	if (arg)
		dma_status = simple_strtoul(arg, NULL, 0);
	else
		dma_status = 0;

	return 0;
}
early_param("dma_status", early_dma_status);

/**
 * boot_get_dma_status - Get dma status
 *
 * return 1 if dma fail, 0 if dma okay
 */
u32 boot_get_dma_status(void)
{
	return dma_status;
}

static int boot_type;

static int __init early_boot_type(char *arg)
{
	if (arg)
		boot_type = simple_strtoul(arg, NULL, 0);
	else
		boot_type = -1;

	return 0;
}
early_param("boot", early_boot_type);

/**
 * lombo_get_boot_type - Get boot type
 *
 * return boot type.
 */
enum BOOT_TYPE lombo_get_boot_type(void)
{
	if (boot_type == -1)
		return BOOT_TYPE_INVALID;
	else
		return boot_type;
}
#endif

#ifdef CONFIG_ION_LOMBO
u32 lombo_ion_reserve;

static int __init early_ion(char *arg)
{
	lombo_ion_reserve = memparse(arg, NULL);
	return 0;
}
early_param("ion", early_ion);

u32 lombo_get_ion_carvout(u32 *base, u32 *size)
{
	u32 carvout_base, carvout_size, dram_size;

	carvout_size = lombo_ion_reserve;
	if (carvout_size <= 0) {
		PRT_ERR("error carvout size 0x%0x\n", carvout_size);
		return -1;
	}

	dram_size = lombo_get_sdram_capacity();
	if (carvout_size > (dram_size / 2)) {
		PRT_WARN("config carvout size: %uMB more than dram_size/2\n",
				carvout_size >> 20);
	}

	if (dram_size > SZ_256M)
		carvout_base = CONFIG_PHYS_OFFSET + dram_size - carvout_size;
	else
		carvout_base = CONFIG_PHYS_OFFSET +
			dram_size - SZ_8M * 2 - SZ_1M - carvout_size;

	*base = carvout_base;
	*size = carvout_size;
	PRT_INFO("carout base 0x%0x, size 0x%0x\n", *base, *size);
	return 0;
}

void __init lombo_reserve(void)
{
	u32 carvout_base, carvout_size;
	early_init_fdt_scan_reserved_mem();

	/* caculate the base & size of the carvout heap */
	if (lombo_get_ion_carvout(&carvout_base, &carvout_size)) {
		PRT_ERR("get ion carvout params error!\n");
		return;
	}
	/*
	 * if carveout memory conflict with exsiting reserved memorys,
	 * the system maybe crash, so we should re-config it (both in
	 * in dts and the LOMBO_ION_CARVOUT_BASE/SIZE)
	 */
	if (memblock_is_region_reserved(carvout_base, carvout_size))
		PRT_ERR("carveout memory overlapped, please re-config it!\n");

	memblock_reserve(carvout_base, carvout_size);
}
#endif

void __init lombo_map_io(void)
{
	iotable_init(lombo_io_desc, ARRAY_SIZE(lombo_io_desc));
}

static void __init lombo_timer_init(void)
{
	of_clk_init(NULL);
	clocksource_of_init();
}

u32 lombo_get_sdram_capacity(void)
{
	u32 capacity = 0;
	int i;

	for (i = 0; i < meminfo.nr_banks; i++)
		capacity += meminfo.bank[i].size;

	return capacity;
}

#define PM_KEY_FIELD		(0xEE18)
static void lombo_poweroff(void)
{
	u32 reg;
	void *addr;

#ifdef CONFIG_ARCH_LOMBO_N7
	addr = (void *)VA_RTC_PM_PE;
#elif CONFIG_ARCH_LOMBO_N9
	addr = (void *)VA_RTC_PM_PE0;
#endif

	reg = readl(addr);
	reg |= (PM_KEY_FIELD << 16);
	reg &= ~BIT(1); /* disable pwr_ddr_en */
	reg &= ~BIT(0); /* disable pwr_en */
	writel(reg, addr);
}

#ifdef CONFIG_ARCH_LOMBO_N7V0_DEMO_BOARD
#include <mach/csp.h>
#endif

static void __init lombo_machine_init(void)
{
	of_platform_populate(NULL, of_default_bus_match_table, NULL, NULL);
#ifdef CONFIG_PM
	lombo_pm_init();
#endif
	pm_power_off = lombo_poweroff;

#ifdef CONFIG_ARCH_LOMBO_N7V0_DEMO_BOARD
	/* enable 5v power for peripherals */
	{
	reg_rtc_sio_func_r0_t sio_func;
	u32 reg_tmp;

	sio_func.val = READREG32(VA_RTC_SIO_FUNC_R0);
	sio_func.bits.sio3 = 2;
	WRITEREG32(VA_RTC_SIO_FUNC_R0, sio_func.val);

	reg_tmp = READREG32(VA_RTC_SIO_DATA_R0);
	reg_tmp = reg_tmp | (1 << 3);
	WRITEREG32(VA_RTC_SIO_DATA_R0, reg_tmp);
	}
#endif
}

#ifndef CONFIG_USE_OF
static void __init gic_init_irq(void)
{
	/* gic_init(0, 29, gic_dist_base_addr, gic_cpu_base_addr); */
}
#endif

#ifdef CONFIG_USE_OF
static const char *const lombo_dt_compat[] __initconst = {
	"lombotech,n7",
	NULL
};

DT_MACHINE_START(LOMBO, "LomboTech-N7 (Flattened Device Tree)")
	.dt_compat    = lombo_dt_compat,
	.map_io       = lombo_map_io,
	.init_time    = lombo_timer_init,
#ifdef CONFIG_ION_LOMBO
	.reserve      = lombo_reserve,
#endif
	.init_machine = lombo_machine_init,
	.smp          = smp_ops(lombo_smp_ops),
MACHINE_END
#else
MACHINE_START(LOMBO, "LomboTech-N7")
	.atag_offset    = 0x100,
	.init_machine   = lombo_machine_init,
	.init_time      = lombo_timer_init,
#ifdef CONFIG_ION_LOMBO
	.reserve        = lombo_reserve,
#endif
	.init_irq       = gic_init_irq,
	.map_io         = lombo_map_io,
	.restart        = lombo_restart,
	.smp		= smp_ops(lombo_smp_ops),
MACHINE_END
#endif

