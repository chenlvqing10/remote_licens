/*
* copyright (c) 2018 All Rights Reserved
*
* This file is part of audio.
*
* File   : decode_plat.h
* Version: V1.0
* Date   : 2018/8/21 17:16:50
* Other  : ffmpeg-3.2.7
*/
#ifndef DECODE_PLAT_H
#define DECODE_PLAT_H

#if defined WIN32
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
typedef  char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef  int int32_t;
typedef unsigned int uint32_t;
typedef  __int64 int64_t;
typedef unsigned __int64 uint64_t;
#define PRINTF printf
#define  AVIOFILE (FILE *)
#define FNULL NULL
#define FWRMODE "wb+"
#define FRMODE "rb"
#elif defined __KERNEL1__
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#ifndef NULL
#define NULL 0
#endif
#define AVIOFILE FILE *
#define FNULL NULL
#define FWRMODE "wb+"
#define FRMODE "rb"
typedef int64_t __int64;
#define PRINTF(...)
#else /* defined (OS_VENUS)__EOS__ */
#ifndef __linux__
#include <rtthread.h>
#include <dfs_posix.h>
#include <debug.h>

#define malloc(count) oscl_zalloc(count)
#define free(p) oscl_free(p)
#endif
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <oscl.h>

#ifndef NULL
#define NULL 0
#endif
typedef int64_t __int64;
#define PRINTF printf
#define AVIOFILE long
#define FNULL -1
#define FILE int
#define FWRMODE (O_RDWR | O_CREAT)
#define FRMODE O_RDONLY

extern int oscl_cfile_open(const char *file, int flags);
extern int oscl_cfile_read(int fd, void *buf, size_t len);
extern int oscl_cfile_write(int fd, const void *buf, size_t len);
extern off_t oscl_cfile_lseek(int fd, off_t offset, int whence);
extern int oscl_cfile_close(int fd);

#define fopen(x, y) oscl_cfile_open(x, y)
#define fread(a, b, c, d)  (oscl_cfile_read(d, a, (b) * (c))/b)
#define fwrite(a, b, c, d)  (oscl_cfile_write(d, a, (b) * (c))/b)
#define fseek(x, y, z) oscl_cfile_lseek((AVIOFILE)x, y, z)
#define ftell(x) tell((AVIOFILE)x)
#define tell(fd) oscl_cfile_lseek(fd, 0, SEEK_CUR)
#define fclose(x) oscl_cfile_close((AVIOFILE)x)

#endif

#endif /* DECODE_PLAT_H */

