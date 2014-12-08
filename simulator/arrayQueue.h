#ifndef arrayQueue_H
#define arrayQueue_H
/*
 * arrayQueue.h
 */
#include <stdlib.h>
/*a simple array queue that wraps around.
It takes the size of the assoc of the cache*/
struct arrayQueue
{
	int front;
	int rear;
	int size;
	int max;
	int *index;
};
typedef struct arrayQueue* queue;
/*Each block calls queue create to have their own queue*/
queue queueCreate(int maxItems);
/*Enqueue an item, simple enough*/
void enqueue(queue modify,int ind);
/*Dequeues the first item added*/
int dequeue(queue modify);
/*Destroys the list*/
void destroyQueue(queue destroyMe);
void displayMe(queue display);

#endif