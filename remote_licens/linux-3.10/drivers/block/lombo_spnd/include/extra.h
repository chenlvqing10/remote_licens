#ifndef ___EXTRA__H__
#define ___EXTRA__H__

#if (defined SPND_HAS_BURN_LIB) && (defined SPND_HAS_EXTRA_CODE)

#define SPND_TEST_WITH_BURN_LIB

#ifdef SPND_TEST_WITH_BURN_LIB
#define SPND_CHIP_TEST
#endif

#include "../extra/include/nand_phy_burn.h"

#ifdef SPND_TEST_WITH_BURN_LIB
struct lombo_spnd;
int spnd_init_with_lib(struct lombo_spnd *spnd);
int spnd_format(int need_alloc);
#endif

#ifdef SPND_CHIP_TEST
int spnd_chip_test(void);
int nand_chip_erase(void);
#endif

#endif

void *get_nandphy(void);
void *_get_nftl(void);

#ifdef SPND_HAS_DUMP_LIB
/* #define NANDPHY_DEBUG */
void phy_block_dump_mapping(void);
u32 phy_block_get_v2p_map(u32 vblock);
void phy_dump_phyparam(void);
void phy_dump_nandparam(void);
void phy_dump_snc_param(void);
void phy_show_block_stats(u32 pblock);
void phy_show_all_block_stats(void);
void phy_block_show_rpl_stats(void);
void phy_print_last_info(void);
#endif

#endif /* ___EXTRA__H__ */
