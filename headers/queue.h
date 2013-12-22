#ifndef QUEUE_H
#define QUEUE_H

#include <stdint.h>

typedef struct node {
	struct node *prevnode;
	uint8_t value;
	struct node *nextnode; 
} node;

typedef struct queue {
	node *first;
	node *last;
} queue;

// Put node to the end of the queue
void put_node(queue *q, node *newnode);
// Return head node and remove it from the queue
node *take_node(queue *q);
// Init node pool queue
void nodepool_init(queue *poolptr, node poolmas[], uint32_t pool_size);

#endif //QUEUE_H
