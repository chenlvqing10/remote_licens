#include <linux/slab.h>
#include <linux/io.h>

int gax_power_on_flg = 0;
int gax_prcm_clk_en = 0;

EXPORT_SYMBOL_GPL(gax_power_on_flg);
EXPORT_SYMBOL_GPL(gax_prcm_clk_en);

void *ax_malloc(int size)
{
	return kzalloc(size, GFP_KERNEL | GFP_DMA);
}

void ax_free(void *ptr)
{
	kfree(ptr);
	ptr = NULL;
}

unsigned int Re_Reg(unsigned int reg)
{
	unsigned int value = readl((void *)reg);
	return value;
}

void Wr_Reg(unsigned int reg, unsigned int value)
{
	writel(value, (void *)reg);
}

unsigned int Re_Reg_RAW(unsigned int reg)
{
	unsigned int value = readl((void *)reg);
	return value;
}

void Wr_Reg_RAW(unsigned int value, unsigned int reg)
{
	writel(value, (void *)reg);
}
