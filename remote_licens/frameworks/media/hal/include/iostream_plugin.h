#ifndef __IOSTREAM_PLUGIN_H__
#define __IOSTREAM_PLUGIN_H__

typedef enum iostream_type {
	STREAM_TYPE_FILE,
	STREAM_TYPE_EXTERNAL,
	STREAM_TYPE_MAX
} iostream_type_e;

typedef struct io_interface {
	int (*open)(const char *path, int flag);
	long (*read)(int fd, void *buf, unsigned long size);
	long (*write)(int fd, void *buf, unsigned long size);
	int64_t (*lseek)(int fd, int64_t offset, int whence);
	int (*close)(int fd);
} io_interface_t;

typedef struct file_stream_para {
	io_interface_t io_intf;
	char *filename;
} file_stream_para_t;

typedef struct external_stream_para {
	int size;
	int align;
} external_stream_para_t;

typedef struct iostream_plugin {
	void *(*create)(void *param, iostream_type_e type);
	void (*destroy)(void *s);
	long (*filldata)(void *s, void *data, size_t size);
	long (*ctrl)(void *s, int cmd, void *arg);
} iostream_plugin_t;

#endif /* __IOSTREAM_PLUGIN_H__ */

