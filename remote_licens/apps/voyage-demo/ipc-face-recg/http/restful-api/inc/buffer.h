#ifndef BUFFER_H
#define BUFFER_H

#include <stdlib.h>
#include <strings.h>
#include <stdarg.h>

typedef struct _buffer {
	char *contents;
	int bytes_used;
	int total_size;
} buffer_t;

buffer_t *buffer_alloc(int initial_size);
int buffer_strlen(buffer_t *buf);
void buffer_free(buffer_t *buf);
int buffer_append(buffer_t *buf, char *append, int length);
int buffer_appendf(buffer_t *buf, const char *format, ...);
int buffer_nappendf(buffer_t *buf, size_t length, const char *format, ...);
char *buffer_to_s(buffer_t *buf);

#endif
