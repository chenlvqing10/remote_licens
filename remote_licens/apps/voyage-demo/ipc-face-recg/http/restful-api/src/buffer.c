#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>

#include "buffer.h"
#include "dbg.h"

/**
 * buffer_alloc: create buffer by initial_size length
 * @initial_size: buffer length
 *
 * Returns NULL means failure, other means pointer of buffer.
 */
buffer_t *buffer_alloc(int initial_size)
{
	buffer_t *buf = malloc(sizeof(buffer_t));
	char *tmp = calloc(1, initial_size * sizeof(char));

	jump_to_error_if(buf == NULL || tmp == NULL);

	buf->contents = tmp;
	buf->bytes_used = 0;
	buf->total_size = initial_size;

	return buf;
error:
	if (buf)
		buffer_free(buf);
	if (tmp)
		free(tmp);

	return NULL;
}

/**
 * buffer_strlen: length of used buffer
 * @buf: pointer of buffer
 *
 * Returns length of having been used buffer.
 */
int buffer_strlen(buffer_t *buf)
{
	return buf->bytes_used;
}

/**
 * buffer_free: free buffer
 * @buf: pointer of buffer
 *
 * Returns None.
 */
void buffer_free(buffer_t *buf)
{
	free(buf->contents);
	free(buf);
}

/**
 * buffer_has_space: get length of buffer remained
 * @buf: pointer of buffer
 * desired_length: desired length
 *
 * Returns 0: remain value less desired value,
 *		remain value more than desired value.
 */
int buffer_has_space(buffer_t *buf, int desired_length)
{
	int bytes_remaining = buf->total_size - buf->bytes_used;
	/*
	printf("Requesting %d bytes, %d available\n",
			desired_length, bytes_remaining);
	*/
	return desired_length <= bytes_remaining;
}

/**
 * buffer_grow: remalloc buffer
 * @buf: pointer of buffer
 * minimum_size: minimum size
 *
 * Returns 0 means success, other means failure.
 */
int buffer_grow(buffer_t *buf, int minimum_size)
{
	int factor = buf->total_size;

	if (factor < minimum_size)
		factor = minimum_size;

	int new_size = factor * 2;
	/*
	printf("Growing buffer from %d to %d bytes\n",
			buf->total_size, new_size);
	*/
	char *tmp = realloc(buf->contents, new_size * sizeof(char));
	jump_to_error_if(tmp == NULL);

	buf->contents = tmp;
	buf->total_size = new_size;

	return 0;
error:
	return -1;
}

/**
 * buffer_cat: data has been appended to buffer
 * @buf: pointer of buffer
 * append: data string
 * length: data length
 *
 * Returns None.
 */
void buffer_cat(buffer_t *buf, char *append, int length)
{
	int i = 0;
	int bytes_copied = 0;
	int buffer_position = 0;

	for (i = 0; i < length; i++) {
		if (append[i] == '\0')
			break;

		buffer_position = buf->bytes_used + i;
		*(buf->contents + buffer_position) = append[i];

		bytes_copied++;
	}

	buf->bytes_used += bytes_copied;
	*(buf->contents + buf->bytes_used) = '\0';
}

/**
 * buffer_append: data has been appended to buffer
 * @buf: pointer of buffer
 * append: data string
 * length: data length
 *
 * Returns 0 means success, other means failure.
 */
int buffer_append(buffer_t *buf, char *append, int length)
{
	int status = 0;
	int desired_length = length + 1;

	if (!buffer_has_space(buf, desired_length)) {
		status = buffer_grow(buf, desired_length);
		jump_to_error_unless(status == 0);
	}

	buffer_cat(buf, append, length);

	return 0;

error:
	return -1;
}

/**
 * buffer_appendf: data has been appended to buffer by format
 * @buf: pointer of buffer
 * format: format string
 *
 * Returns 0 means success, other means failure.
 */
int buffer_appendf(buffer_t *buf, const char *format, ...)
{
	char *tmp = NULL;
	int bytes_written, status;

	va_list argp;
	va_start(argp, format);

	bytes_written = vasprintf(&tmp, format, argp);
	jump_to_error_if(bytes_written < 0);

	va_end(argp);

	status = buffer_append(buf, tmp, bytes_written);
	jump_to_error_unless(status == 0);

	free(tmp);

	return 0;

error:
	if (tmp != NULL)
		free(tmp);
	return -1;
}

/**
 * buffer_appendf: data has been appended to buffer by format
 * @buf: pointer of buffer
 * format: format string
 *
 * Returns 0 means success, other means failure.
 */
int buffer_nappendf(buffer_t *buf, size_t length, const char *format, ...)
{
	int status = 0, printf_length = length + 1;

	char *tmp = calloc(1, printf_length * sizeof(char));

	jump_to_error_if(tmp == NULL);

	va_list argp;
	va_start(argp, format);

	status = vsnprintf(tmp, printf_length, format, argp);
	jump_to_error_if(status < 0);

	va_end(argp);
	status = buffer_append(buf, tmp, length);
	jump_to_error_unless(status == 0);

	free(tmp);

	return 0;
error:
	if (tmp != NULL)
		free(tmp);
	return -1;

}

/**
 * buffer_to_s: copying data from buf
 * @buf: pointer of buffer
 *
 * Returns data string of having been copyed.
 */
char *buffer_to_s(buffer_t *buf)
{
	char *result = calloc(1, buf->bytes_used + 1);
	strncpy(result, buf->contents, buffer_strlen(buf));

	return result;
}
