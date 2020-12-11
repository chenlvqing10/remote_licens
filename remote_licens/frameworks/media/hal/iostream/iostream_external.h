#ifndef __IOSTREAM_EXTERAL_H__
#define __IOSTREAM_EXTERAL_H__

#include "iostream_plugin.h"

void *iostream_ext_create(void *param);
void iostream_ext_destroy(void *s);
ssize_t iostream_ext_write(void *s, uint8_t *buf, size_t size);
ssize_t iostream_ext_read(void *s, uint8_t *buf, size_t size);
int64_t iostream_ext_seek(void *s, int64_t offset, int whence);
int64_t iostream_ext_tell(void *s);
int64_t iostream_ext_fsize(void *s);
long iostream_ext_ctrl(void *s, int cmd, void *arg);

#endif /* __IOSTREAM_EXTERAL_H__ */

