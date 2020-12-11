#ifndef _NFTL_H_
#define _NFTL_H_


int _register_nftl(void);
void _deregister_nftl(void);

int _nftl_get_sector_size(void);
int _nftl_get_sector_count(void);

int _nftl_read_sector(int start_sector, int sector_cnt, void *buf);
int _nftl_write_sector(int start_sector, int sector_cnt, void *buf);
int _nftl_trim(int start_sector, int sector_cnt);
int _nftl_flush(void);

void _nftl_bg(int timer_cnt);

void _nftl_set_key(char *buf, int count);
int _change_compressor(const char *name);
void _show_gc_info(void);
void _show_block_info(void);
void _show_nftl_info(void);

void _show_map(int addr);
void _show_page(int page);
void _show_block_oob(int block);
void _show_nftl_version(void);



#endif

