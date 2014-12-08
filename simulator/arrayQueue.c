#include	"arrayQueue.h"
#include	<unistd.h>
#include	<stdio.h>
#include	<string.h>
#include	<stdlib.h>

queue queueCreate(int maxItems){
	/*allocates a dynamic array. Create queue as well, set everything to zero
	returns the queue*/
	queue start=(queue)malloc(sizeof(struct arrayQueue));
	start->front=0;
	start->rear=0;
	start->max=maxItems;
	start->index=(int *)malloc(sizeof(int)*maxItems);
	return start;
}
void enqueue(queue modify, int ind){
	/*simple enqeueu functions that moves the index rear and front respectively*/
	if(modify->rear==0){
		modify->index[modify->rear]=ind;
		modify->rear++;
		modify->size++;
		return;
	}
	if(modify->size==modify->max){
		printf("full\n");
		return;
	}
	/*if we rear equal the max, set it back to the front, insert, update rear*/
	if(modify->rear==modify->max){
		modify->rear=0;
		modify->index[modify->rear]=ind;
		modify->rear++;
		modify->size++;
		return;
	}
	/*if rear is the maxed and is greater that front and max, we have to set it back to zero*/
	if(modify->rear==modify->max && modify->rear-modify->front>0){
		modify->rear=0;
		modify->index[modify->rear]=ind;
		modify->rear++;
		modify->size++;
		return;
	}
	else{
		/*else we just add, and increment normally*/
		modify->index[modify->rear]=ind;
		modify->rear++;
		modify->size++;

	}

}

int dequeue(queue modify){
	/*returns the first item added. Updates the parameters of the queue*/
	if(modify->size==0){
		return -1;
	}
	else{
		if(modify->front>=modify->max){
			modify->front=0;
		}
		int giveMe=modify->index[modify->front];
		modify->index[modify->front]=-1;
		modify->front++;
		modify->size--;
		return giveMe;
	}
}

void destroyQueue(queue destroyMe){
	/*free everythhing that was allocated*/
	free(destroyMe->index);
	free(destroyMe);

}
/*displayMe is for debugging purposes only. Was rather useless:*/
void displayMe(queue display){
	for (int i=0; i<display->max; i++){
		printf("%i",display->index[i]);
	}
}