#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "queue.h"

int en_queue(head_t *head, void *buf)
{
	if (!head)
		return -1;

	node_t *e = malloc(sizeof(node_t));
	if (!e) {
		printf("malloc failed, errno: %d (%s)", errno, strerror(errno));
		return errno;
	}
	e->buf = buf;
	TAILQ_INSERT_TAIL(head, e, nodes);

	return 0;
}

void *de_queue(head_t *head)
{
	void *retval = NULL;

	if (!head)
		return NULL;

	if (TAILQ_EMPTY(head))
		return NULL;

	node_t *e = TAILQ_FIRST(head);
	TAILQ_REMOVE(head, e, nodes);
	retval = e->buf;
	free(e);
	e = NULL;

	return retval;
}

void free_queue(head_t *head)
{
	node_t *e = NULL;
	while (!TAILQ_EMPTY(head))
	{
		e = TAILQ_FIRST(head);
		TAILQ_REMOVE(head, e, nodes);
		free(e);
		e = NULL;
	}
}

int empty_queue(head_t *head)
{
	return TAILQ_EMPTY(head);
}

int queue_size(head_t *head)
{
	node_t *e = NULL;
	int size = 0;
	TAILQ_FOREACH(e, head, nodes) {
		size++;
	}
	return size;
}

void queue_remove(head_t *head, node_t *e)
{
	TAILQ_REMOVE(head, e, nodes);
	free(e);
	e = NULL;
}
