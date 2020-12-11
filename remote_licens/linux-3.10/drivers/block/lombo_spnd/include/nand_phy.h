
#ifndef __NAND__PHY___H__
#define __NAND__PHY___H__

void nandphy_preinit(const struct nandphy_hook *hook,
			const struct spi_bus_ops *spi_ops);

u32 nandphy_get_param_len(void);

int nandphy_init(const struct nandphy_hook *hook,
			const struct spi_bus_ops *spi_ops,
			void *param0);

int nandphy_reinit(void);

u32 nandphy_get_freq(void);

u32 nandphy_get_param1_len(void);

int nandphy_init1(void *param1);

int nandphy_init2(void *param1);

int nandphy_store_all(void);

void nandphy_exit(void);

unsigned int nandphy_get_capacity(void);

unsigned int nandphy_get_page_size(void);

#endif /* __NAND__PHY___H__ */
