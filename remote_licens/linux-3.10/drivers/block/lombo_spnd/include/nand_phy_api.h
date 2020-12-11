
#ifndef _NAND_PHY_API_H_
#define _NAND_PHY_API_H_

struct nandphy_hook {
	void *(*os_malloc)(unsigned long size);
	void (*os_free)(void *ptr);
	void *(*os_malloc_buf)(unsigned long size);
	void (*os_free_buf)(void *ptr, unsigned long size);
	void (*printf_debug)(const char *fmt, ...);
	void (*printf_error)(const char *fmt, ...);
	void *(*os_memset)(void *s, int c, unsigned long count);
	void *(*os_memcpy)(void *dst, void *src, unsigned long count);
	int (*strlen)(const char *);
	int (*strncmp)(const char *, const char *, int);
	unsigned long (*lock_open)(void);
	void (*lock_close)(unsigned long ld);
	void (*lock)(unsigned long ld);
	void (*unlock)(unsigned long ld);
	void (*bug)(void);
	void (*assert)(int cond);
	int (*os_vsnprintf)(char *buf, size_t size, const char *fmt,
			void *args);
	void (*os_delay)(unsigned long us);
};
#endif
