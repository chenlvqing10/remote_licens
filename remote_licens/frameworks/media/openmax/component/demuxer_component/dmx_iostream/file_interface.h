#ifndef __FILE_INTERFACE_H__
#define __FILE_INTERFACE_H__

int dmx_file_open(const char *path, int flag);
long dmx_file_read(int fd, void *buf, unsigned long size);
int64_t dmx_file_seek(int fd, int64_t offset, int whence);
int dmx_file_close(int fd);

#endif /* __FILE_INTERFACE_H__ */

