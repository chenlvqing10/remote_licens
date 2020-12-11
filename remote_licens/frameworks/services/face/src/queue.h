#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <sys/queue.h>

typedef struct node
{
	void *buf;
	TAILQ_ENTRY(node) nodes;
} node_t;

typedef TAILQ_HEAD(head_s, node) head_t;

typedef struct list_head list_head_t;

int en_queue(head_t *head, void *buf);
void *de_queue(head_t *head);
void free_queue(head_t *head);
int empty_queue(head_t *head);
int queue_size(head_t *head);
void queue_remove(head_t *head, node_t *e);

#endif /* _QUEUE_H_ */
