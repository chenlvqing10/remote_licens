#ifndef __IOSTREAM_FILE_H__
#define __IOSTREAM_FILE_H__

#include "iostream_plugin.h"

ssize_t iostream_file_write(void *s, uint8_t *buf, size_t size);
ssize_t iostream_file_read(void *s, uint8_t *buf, size_t size);
int64_t iostream_file_seek(void *s, int64_t offset, int whence);
int64_t iostream_file_tell(void *s);
int64_t iostream_file_fsize(void *s);
long iostream_file_ctrl(void *s, int cmd, void *arg);
void *iostream_file_create(void *param);
void iostream_file_destroy(void *handle);

#endif /* __IOSTREAM_FILE_H__ */
