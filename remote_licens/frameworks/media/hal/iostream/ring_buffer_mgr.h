#ifndef __RING_BUFFER_MGR_H__
#define __RING_BUFFER_MGR_H__

#include <stdint.h>

void *rbm_create(size_t bufsize, int align);
void rbm_destroy(void *handle);
long rbm_write(void *handle, uint8_t *data, unsigned long size);
long rbm_read(void *handle, uint8_t *data, unsigned long size);
int64_t rbm_seek(void *s, int64_t offset, int whence);
void rbm_reset(void *handle);
int64_t rbm_tell(void *s);

#endif /* __RING_BUFFER_MGR_H__ */

