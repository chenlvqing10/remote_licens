/*
 * lombo_ion.c - Common code for LomboTech SoCs
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
#include <linux/kernel.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/mm.h>
#include <linux/uaccess.h>
#include <linux/dma-mapping.h>
#include <linux/dma-contiguous.h>
#include <linux/bug.h>
#include <asm/cacheflush.h>
#include <mach/debug.h>
#include <mach/memory.h>
#include <mach/common.h>
#include <linux/memblock.h>
#include "../../uapi/lombo_ion.h"
#include "../ion_priv.h"

#undef  MOD_NAME
#define MOD_NAME "LOMBO_ION"

static struct ion_device *ion_device;
#ifdef CONFIG_64BIT
phys_addr_t lombo_ion_base;
u32 lombo_ion_reserve;

static int __init early_ion(char *arg)
{
	lombo_ion_reserve = memparse(arg, NULL);
	return 0;
}
early_param("ion", early_ion);

u32 lombo_get_ion_carvout(u32 *base, u32 *size)
{
	*base = lombo_ion_base;
	*size = lombo_ion_reserve;
	PRT_INFO("carout base 0x%0x, size 0x%0x\n", *base, *size);
	return 0;
}

void __init carvout_reserve(void)
{
	phys_addr_t paddr;

	if (0 == lombo_ion_reserve) {
		pr_err("%s: Warning lombo_ion_reserve = %d\n", __func__,
			lombo_ion_reserve);
		return;
	}
	paddr = memblock_alloc_base(lombo_ion_reserve,
			0,
			MEMBLOCK_ALLOC_ANYWHERE);
	if (!paddr) {
		pr_err("%s: err allocate mem for heap\n", __func__);
		return;
	}
	lombo_ion_base = paddr;
	pr_info("%s: carveout reserved base %lx size %zu\n", __func__,
		lombo_ion_base,
		lombo_ion_reserve);
}
#endif

struct ion_handle *ion_handle_get_by_id(struct ion_client *client, int id);
long lombo_ion_ioctl(struct ion_client *client,
			unsigned int cmd, unsigned long arg)
{
	long ret = 0;

	switch (cmd) {
	case ION_IOC_LB_PHYS_ADDR:
	{
		struct lombo_phys_data data;
		struct ion_handle *handle;
		if (copy_from_user(&data, (void __user *)arg,
			sizeof(struct lombo_phys_data)))
			return -EFAULT;

		handle = ion_handle_get_by_id(client, data.handle);
		if (IS_ERR(handle))
			return PTR_ERR(handle);

		ret = ion_phys(client, handle,
		(ion_phys_addr_t *)&data.phys_addr, (size_t *)&data.size);
		ion_handle_put(handle);
		if (ret)
			return -EINVAL;
		if (copy_to_user((void __user *)arg, &data, sizeof(data)))
			return -EFAULT;
		break;
	}
	default:
		PRT_ERR("cmd not support!!\n");
		return -ENOTTY;
	}

	return ret;
}

int lombo_ion_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	struct ion_platform_heap heaps_desc;
	struct device_node *heap_node = NULL;

	ion_device = ion_device_create(lombo_ion_ioctl);
	if (IS_ERR_OR_NULL(ion_device))
		return PTR_ERR(ion_device);

	do {
		u32 type = -1;
		struct ion_heap *pheap = NULL;

		/*loop all the child node */
		heap_node = of_get_next_child(np, heap_node);
		if (!heap_node)
			break;
		memset(&heaps_desc, 0, sizeof(heaps_desc));

		/* get the properties "name","type" for common ion heap	*/
		if (of_property_read_u32(heap_node, "type", &type)) {
			PRT_ERR("You need config the heap node 'type'\n");
			continue;
		}
		heaps_desc.type = type;
		heaps_desc.id = type;

		if (of_property_read_string(heap_node, "name",
			&heaps_desc.name)){
			PRT_ERR("You need config the heap node 'name'\n");
			continue;
		}

		/* for specail heaps , need extra argument to config */
		if (ION_HEAP_TYPE_CARVEOUT == heaps_desc.type) {
			u32 base = 0 , size = 0;
			if (lombo_get_ion_carvout(&base , &size)) {
				PRT_ERR("get ion carvout params error!\n");
				continue;
			}
			heaps_desc.base = base;
			heaps_desc.size = size;
		} else if (ION_HEAP_TYPE_DMA == heaps_desc.type)
			heaps_desc.priv = &(pdev->dev);

		/* now we can create a heap & add it to the ion device*/
		pheap = ion_heap_create(&heaps_desc);
		if (IS_ERR_OR_NULL(pheap)) {
			PRT_ERR("ion_heap_create '%s' failured!!\n",
				heaps_desc.name);
			continue;
		}

		ion_device_add_heap(ion_device , pheap);
	} while (1);

	return 0;
}

static const struct of_device_id lombo_ion_dt_ids[] = {
	{ .compatible = "lombo,lombo-ion" },
	{ /* end */ }
};

static struct platform_driver lombo_ion_driver = {
	.driver = {
		.name = "lombo-ion",
		.of_match_table = lombo_ion_dt_ids,
	},
	.probe = lombo_ion_probe,
};
module_platform_driver(lombo_ion_driver);
