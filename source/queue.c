#include <stdint.h>
#include "queue.h"
#include <stdlib.h>

void put_node(queue *q, node *newnode)
{
	newnode->prevnode = q->last;
	newnode->nextnode = NULL;

	if (q->first == NULL)
		q->first = newnode;
	
	(q->last)->nextnode = newnode;
	q->last = newnode;
}

node *take_node(queue *q)
{	
	node *temp;

	if (q->first == NULL)
		return NULL;
	
	temp = q->first;
    if ((q->first)->nextnode != NULL) {
        q->first = (q->first)->nextnode;
        (q->first)->prevnode = NULL;
    }
    else {
        q->first = NULL;
        q->last = NULL;
    }
	
	return temp;
}

void nodepool_init(queue *poolptr, node poolmas[], uint32_t pool_size)
{
	int i;
	for (i = 0; i < pool_size; i++)
		put_node(poolptr, &(poolmas[i]));
}
