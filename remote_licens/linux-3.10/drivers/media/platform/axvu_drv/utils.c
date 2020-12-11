#include <linux/slab.h>
#include <linux/io.h>

void *vu_malloc(int size)
{
	return kzalloc(size, GFP_KERNEL | GFP_DMA);
}

void vu_free(void *ptr)
{
	kfree(ptr);
	ptr = NULL;
}
