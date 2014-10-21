//Queue.h; // interface to Queue ADT
// Written by John Shepherd, March 2013

#ifndef QUEUE_H
#define QUEUE_H

#include "Item.h"

typedef struct QueueRep *Queue;

Queue newQueue(); // create new empty queue
void dropQueue(Queue q); // free memory used by queue
void showQueue(Queue q); // display as 3 > 5 > 4 > ...
void QueueJoin(Queue q, Item i); // add item on queue
Item QueueLeave(Queue q); // remove item from queue
int QueueIsEmpty(Queue q); // check for no items

#endif