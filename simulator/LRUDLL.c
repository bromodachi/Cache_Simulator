#include	"LRUDLL.h"
#include	<unistd.h>
#include	<stdio.h>
#include	<string.h>
#include	<stdlib.h>


SortedListPtr SLCreate(){
	/*initialize create function*/
	SortedListPtr start=(SortedListPtr)malloc(sizeof(struct SortedList));
	/*if malloc somehow fails, we return NULL*/
	if(start==NULL){
		return NULL;

	}
	start->head=NULL;
	start->tail=NULL;
	return start;
}

int insertHead(SortedListPtr list, node add, int index){
	/*was it used again? move it to the front*/
	/*all inserted items gets inserted in the front*/
	node temp=add;
	temp->prev=NULL;
	list->head->prev=temp;
	temp->next=list->head;
	list->head=temp;
	return 1;

}
int SLInsert(SortedListPtr list, int index){
	if(list==NULL){
		return 0;

	}
	node temp=(node)malloc(sizeof(struct node_list));
	/*if malloc somehow fails, we return 0*/
	if(temp==NULL){
		return 0;
	}
	node prev;
	node curr;
	temp->next=NULL;
	temp->prev=NULL;
	temp->index=index;
	curr=list->head;
	if((list->head==NULL)){
		list->head=temp;
		list->tail=list->head;
		return 1;
	}

	/*already in the list, no need to call insert head, free it*/
	if(curr->index==temp->index){
		free(temp);
		return 1;
	}
	/*used this for only one node in list and it's not the head. We simply
	*make the newly added node the new head and the curr the tail
	*for traversing the list easier in while loop*/
	if(curr->next==NULL &&curr->index!=temp->index){
		/*printf("\ndid I come here and break");*/
		curr->prev=temp;
		temp->next=curr;
		temp->prev=NULL;
		list->tail=curr;
		list->head=temp;
		return 1;
	}
	prev=curr;
	curr=curr->next;
	while(curr!=NULL){
		if(curr->index==temp->index){ /*if it equals curr, and curr next is null, delink everything, insert*/
			if(curr->next==NULL){
				free(temp);
				prev->next=NULL;
				curr->next=NULL;
				curr->prev=NULL;
				list->tail=prev;
				insertHead(list, curr, index);
				return 1;
			
			}
			/*found our target, make it delink with everything, insert at head*/
			free(temp);
			prev->next=curr->next;
			curr->next->prev=prev;
			curr->next=NULL;
			curr->prev=NULL;
			insertHead(list, curr, index);
			return 1;
		}
		prev=curr;							/*else we simply move along the list*/
		curr=curr->next;
	}
	/*I can assume it will be inserted in the front if curr is null*/
	list->tail=prev;
	insertHead(list, temp, index);
	return 1;
}
int getLRU(SortedListPtr list){
	/*simply returns the last item in the list*/
	int index=list->tail->index;
	if(list->tail==list->head){
		node temp=list->tail;
		list->tail=list->head;
		list->head=NULL;
		free(temp);
		return index;
	}
	node temp=list->tail;
	node prev=temp->prev;
	list->tail=prev;
	prev->next=NULL;
	free(temp);
	return index;
}
void display(SortedListPtr start){
	/*my purposes only*/
	node i=start->head;
	int y=0;
	for(;i!=NULL; i=i->next){
		int value=i->index;
		printf("Place %d digit: %d\n", y, value);
		if(i->prev!=NULL){
			printf("Previous value  for %d is at digit: %d\n",value,i->prev->index);
		}
		y++;
	}

}

void SLDestroy(SortedListPtr list){
	/*free everything that was allocated*/
    if(list != NULL)
    {
		if(list->head==NULL){
			return;
		}
        node start=list->head;
        node next=NULL;
        for(;start!=NULL;start=next){
            next=start->next;
            free(start);
        }
        free(list);
        list->head=NULL;
    }
}