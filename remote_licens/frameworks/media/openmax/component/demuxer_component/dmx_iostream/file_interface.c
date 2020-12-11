#define DBG_LEVEL         DBG_INFO
#define LOG_TAG           "file_interface"

#define _FILE_OFFSET_BITS 64

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stddef.h>


int dmx_file_open(const char *path, int flag)
{
	return open(path, flag);
}

long dmx_file_read(int fd, void *buf, unsigned long size)
{
	return read(fd, buf, size);
}

int64_t dmx_file_seek(int fd, int64_t offset, int whence)
{
	return lseek(fd, offset, whence);
}

int dmx_file_close(int fd)
{
	return close(fd);
}

