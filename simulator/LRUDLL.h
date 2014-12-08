#ifndef LRUDLL_H
#define LRUDLL_H
/*
 * LRUDLL.h
 */

#include <stdlib.h>
/*A double linked list for the LRU policy*/

/*Was a sorted linked-list, thus the name, that I created
 but too lazy to change the names.
Now it's a double linked list with LRU properties*/
struct node_list{
	int index; //the index.
	struct node_list *next;
	struct node_list *prev;
};
typedef struct node_list* node;



struct SortedList
{
	node head;
	node tail;
};
typedef struct SortedList* SortedListPtr;

SortedListPtr SLCreate();
/*updates/inserts the list. If the item was already in the list,
we detach the current node and update its neighbors, call insertHead to update list*/
int insertHead(SortedListPtr list, node add, int index);
/*Same as above basically*/
int SLInsert(SortedListPtr list, int index);
/*gives the tail of a list*/
int getLRU(SortedListPtr list);
/*for debugging purposes below*/
void display(SortedListPtr start);
/*free purposes*/
void SLDestroy(SortedListPtr list);
#endif