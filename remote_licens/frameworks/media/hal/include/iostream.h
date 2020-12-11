#ifndef __IOSTREAM_H__
#define __IOSTREAM_H__

#include <stdlib.h>
#include <stdint.h>

typedef enum iostream_cmd {
	IOSTREAM_CMD_UNKNOWN,
	IOSTREAM_CMD_FILL_DATA,
	IOSTREAM_CMD_BLOCKING,
} iostream_cmd_e;

typedef struct AVIOContext {
	ssize_t (*read)(void *s, uint8_t *buf, size_t size);
	ssize_t (*write)(void *s, uint8_t *buf, size_t size);
	int64_t (*seek)(void *s, int64_t offset, int whence);

	/* get the offset of the current io stream */
	int64_t (*tell)(void *s);

	/* Get the filesize */
	int64_t (*fsize)(void *s);
	long (*ctrl)(void *s, int cmd, void *arg);
	void *handle;
	void *priv;
} AVIOContext;

#endif /* __IOSTREAM_H__ */
