#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include"cache_sim.h"
#include <math.h>

/*initialize the cache to be direct*/
int initializeDirect(int lines, Cache test, int assoc){
	test->valid=0;
	test->miss=0;
	test->cold_misses=0;
	test->capacity_miss=0;
	test->conflict_miss=0;
	test->memory_access=0;
	test->total_misses=0;
	test->hits=0;
	int count=0;
	test->blocks=(CacheArray**)malloc(lines * sizeof(struct cache_array));
	for (int i=0;i<lines;i++){
		test->blocks[i]=(CacheArray*)malloc(assoc *sizeof(struct cache_array));
		for(int j=0;j<assoc;j++){
			test->blocks[i][j].valid=0;
			test->blocks[i][j].tag=-1;
			test->blocks[i][j].indexSet=-1;
			test->blocks[i][j].offSet=-1;
			count++;
		}
	}
	return 0;

}

int initializeSet(int lines, Cache test, int assoc, char *kind){
	test->valid=0;
	test->miss=0;
	test->cold_misses=0;
	test->capacity_miss=0;
	test->conflict_miss=0;
	test->memory_access=0;
	test->hits=0;
	test->total_misses=0;
	int count=0;
	test->blocks=(CacheArray**)malloc(lines * sizeof(struct cache_array));
	for (int i=0;i<lines;i++){
		test->blocks[i]=(CacheArray*)malloc(assoc *sizeof(struct cache_array));
		if((strlen(test->type)>=7)&&(strncmp(test->type,"assoc:", 5)==0)){
			/*create a lru policy using dll for each individual set*/
			if(strcmp(kind,"lru")==0){
				test->blocks[i]->LRU=SLCreate();
				test->blocks[i]->FIFO=NULL;
			//	printf("LRU DLL created\n");
			}
			/*create a queue for each individual set*/
			if(strcmp(kind,"fifo")==0){
				test->blocks[i]->FIFO=queueCreate(assoc);
				test->blocks[i]->LRU=NULL;
			//	printf("LRU DLL created\n");
			}
		}
		for(int j=0;j<assoc;j++){
			test->blocks[i][j].valid=0;
			test->blocks[i][j].tag=-1;
			test->blocks[i][j].indexSet=-1;
			test->blocks[i][j].offSet=-1;
			count++;
		//	printf("%d, Valid %d Tag %lld\n", j, test->blocks[i][j].valid, test->blocks[i][j].tag);
		}
		count++;
	}
	if((strcmp(test->type,"FA")==0)||((strlen(test->type)==5)&& (strcmp(test->type,"assoc")==0))){
		/*only for FA. Either lru policy or fifo*/
		if(strcmp(kind,"lru")==0){
			test->LRU=SLCreate();
			test->FIFO=NULL;
		}
		if(strcmp(kind,"fifo")==0){
			test->FIFO=queueCreate(lines);
			test->LRU=NULL;
		}
	}
	return 0;

}


/*create a respective caches: direct, Fully, n-way*/
void createDirectCache(int cacheSize, int blockSi, int assoc, Cache create){
	int six=64;
	create->numofSet=((cacheSize)/(blockSi*assoc));
	create->HMindex=log2(create->numofSet);
	create->HMblock=log2(blockSi);
	create->HMtag=six-(create->HMindex+create->HMblock);
}

void createFA(int cacheSize, int blockSi, int assoc, Cache create){
	int six=64;
	create->numofSet=((cacheSize)/(blockSi*assoc));/*how many lines*/
	create->HMblock=log2(blockSi);/*block offset*/
	create->HMtag=six-(create->HMblock); /*tag/compare bits*/
}

void createSA(int cacheSize, int blockSi, int assoc, Cache create){
	int six=64;
	/*assoc in how many columns. Combine SA and direct later*/
	create->numofSet=(cacheSize)/(blockSi*assoc); /*Essentially how many rows in array*/
	create->HMindex=log2(create->numofSet);/*how many bits for index */
	create->HMblock=log2(blockSi); /*how many bits for block offset */
	create->HMtag=six-create->HMindex-create->HMblock; /*how many bits for tag off*/

}
/*Simple function that extracts the index, tag, block offset*/
long long extract(int howMany, int start, size_t address){
    int modNum = (int) pow(2.0,(float) howMany);
    return (long long)((address>>start)% modNum);
}

/*Following the functions simulates the caches*/
void direct(Cache modify, size_t addMe){
	modify->memory_access++;
	/*get the necessary bits for each tag, index, block*/
	long long boff=extract(modify->HMblock,0, addMe);
	long long index1=extract(modify->HMindex,modify->HMblock, addMe);
	long long tag1=extract(modify->HMtag,modify->HMindex, addMe);
	long long index=index1 % modify->numofSet;
	if(modify->blocks[index][0].valid==1 && modify->blocks[index][0].tag==tag1){
			/*is it the same tag? Hit, update LRU, return*/
			modify->hits++;
			return;
		}
	if(modify->blocks[index][0].valid==0){
		modify->cold_misses++;
		modify->total_misses++;
		/*call next level and see if they have it*/
		if(modify->nextLevel!=NULL){
			if(strcmp(modify->nextLevel->type, "direct")==0){
				direct(modify->nextLevel, addMe);
			}	
			if((strlen(modify->nextLevel->type)>=7)&&(strncmp(modify->nextLevel->type,"assoc:", 5)==0)){
				//	printf("I came here");
					SA(modify->nextLevel, addMe, modify->nextLevel->assoc);
				}
			if((strlen(modify->nextLevel->type)==5)&& (strcmp(modify->nextLevel->type,"assoc")==0)){
					FA(modify->nextLevel, addMe);
			}
		}
		/*once we're done, add the address' tag, index, off set to the index*/
		modify->blocks[index][0].tag=tag1;
		modify->blocks[index][0].indexSet=index;
		modify->blocks[index][0].offSet=boff;
		modify->blocks[index][0].valid=1;
	}
	/*again, not equal? call the next cache*/
	if(modify->blocks[index][0].valid==1 && modify->blocks[index][0].tag!=tag1){
		if(modify->nextLevel!=NULL){
			if(strcmp(modify->nextLevel->type, "direct")==0){
				direct(modify->nextLevel, addMe);
			}	
			if((strlen(modify->nextLevel->type)>=7)&&(strncmp(modify->nextLevel->type,"assoc:", 5)==0)){
				//	printf("I came here");
					SA(modify->nextLevel, addMe, modify->nextLevel->assoc);
				}
			if((strlen(modify->nextLevel->type)==5)&& (strcmp(modify->nextLevel->type,"assoc")==0)){
					FA(modify->nextLevel, addMe);
				}
			}
		/*once we're done, add the address' tag, index, off set to the index*/
		modify->conflict_miss++;
		modify->total_misses++;
		modify->blocks[index][0].tag=tag1;
		modify->blocks[index][0].indexSet=index;
		modify->blocks[index][0].offSet=boff;
	}
}

void SA(Cache modify, size_t addMe, int assoc){
	modify->memory_access++;
	long long boff=extract(modify->HMblock,0, addMe);
	long long index1=extract(modify->HMindex,modify->HMblock, addMe);
	long long tag1=extract(modify->HMtag,modify->HMindex, addMe);
	long long index=index1 % modify->numofSet;
	int index2;
	for (int j=0;j<assoc; j++){
		if(modify->blocks[index][j].valid==1 && modify->blocks[index][j].tag==tag1){
			/*is it the same tag? Hit, update LRU, return*/
			if(modify->blocks[index]->LRU!=NULL){
			SLInsert(modify->blocks[index]->LRU, j);
			}
			else{
				/*do nothing to fifo*/
			//	enqueue(modify->blocks[index]->FIFO,1);
			}
			modify->hits++;
			return;
		}
		/*cold miss?Look in the other caches as well*/
		if(modify->blocks[index][j].valid==0){
			if(modify->nextLevel!=NULL){
				if(strcmp(modify->nextLevel->type, "direct")==0){
					direct(modify->nextLevel, addMe);
				}	
				if((strlen(modify->nextLevel->type)>=7)&&(strncmp(modify->nextLevel->type,"assoc:", 5)==0)){
					//	printf("I came here");
						SA(modify->nextLevel, addMe, modify->nextLevel->assoc);
					}
				if((strlen(modify->nextLevel->type)==5)&& (strcmp(modify->nextLevel->type,"assoc")==0)){
						FA(modify->nextLevel, addMe);
					}
			}
			modify->cold_misses++;
			modify->total_misses++;
			/*updates the queue or DLL*/
			if(modify->blocks[index]->LRU!=NULL){
				SLInsert(modify->blocks[index]->LRU, j);
			}
			else{
				enqueue(modify->blocks[index]->FIFO,j);
			}
			//printf("%d", j);
			modify->blocks[index][j].tag=tag1;
			modify->blocks[index][j].indexSet=index;
			modify->blocks[index][j].offSet=boff;
			modify->blocks[index][j].valid=1;
		//	printf("\nj is at %d\n", j);
			return;
		}

		if(modify->blocks[index][j].valid==1 &&modify->blocks[index][j].tag!=tag1
			&& j==assoc-1){
				if(modify->nextLevel!=NULL){
				if(strcmp(modify->nextLevel->type, "direct")==0){
					direct(modify->nextLevel, addMe);
				}	
				if((strlen(modify->nextLevel->type)>=7)&&(strncmp(modify->nextLevel->type,"assoc:", 5)==0)){
					//	printf("I came here");
						SA(modify->nextLevel, addMe, modify->nextLevel->assoc);
					}
				if((strlen(modify->nextLevel->type)==5)&& (strcmp(modify->nextLevel->type,"assoc")==0)){
						FA(modify->nextLevel, addMe);
					}
			}
				//printf("I came here\n");
				/*it's full. Call LRU or FIFO*/
				modify->total_misses++;
				if(modify->blocks[index]->LRU!=NULL){
					index2=getLRU(modify->blocks[index]->LRU);
				}
				else{
					index2=dequeue(modify->blocks[index]->FIFO);
				}
			//	printf("\n num: %d\n", index2);
				modify->blocks[index][index2].tag=tag1;
				modify->blocks[index][index2].indexSet=index;
				modify->blocks[index][index2].offSet=boff;
				if(modify->blocks[index]->LRU!=NULL){
					SLInsert(modify->blocks[index]->LRU, index2);
					return;
				}
				else{
					enqueue(modify->blocks[index]->FIFO, index2);
					return;
				}
		}
	}
}

void FA(Cache modify, size_t addMe){
	int index2;
	modify->memory_access++;
	long long boff=extract(modify->HMblock,0, addMe);
	long long tag1=extract(modify->HMtag,modify->HMblock, addMe);
	for (int j=0;j<modify->numofSet; j++){
		if(modify->blocks[j][0].valid==1 && modify->blocks[j][0].tag==tag1){
			/*is it the same tag? Hit, update LRU, return*/
			if(modify->LRU!=NULL){
				SLInsert(modify->LRU, j);
				modify->hits++;
				return;
			}
			else{
				/*do nothing to fifo*/
				modify->hits++;
				return;
			}
		}
		if(modify->blocks[j][0].valid==0){
			if(modify->nextLevel!=NULL){
				if(strcmp(modify->nextLevel->type, "direct")==0){
					direct(modify->nextLevel, addMe);
				}	
					if((strlen(modify->nextLevel->type)>=7)&&(strncmp(modify->nextLevel->type,"assoc:", 5)==0)){
					//	printf("I came here");
						SA(modify->nextLevel, addMe, modify->nextLevel->assoc);
					}
					if((strlen(modify->nextLevel->type)==5)&& (strcmp(modify->nextLevel->type,"assoc")==0)){
						FA(modify->nextLevel, addMe);
					}
			}
		
			modify->cold_misses++;
			modify->total_misses++;
			/*updates the queue or DLL*/
			if(modify->LRU!=NULL){
				SLInsert(modify->LRU, j);
			}
			else{
				enqueue(modify->FIFO,j);
			}
			//printf("%d", j);
			modify->blocks[j][0].tag=tag1;
			modify->blocks[j][0].offSet=boff;
			modify->blocks[j][0].valid=1;
		//	printf("\nj is at %d\n", j);
			return;
		}

		if(modify->blocks[j][0].valid==1 &&modify->blocks[j][0].tag!=tag1
			&& j==(modify->numofSet-1)){
				if(modify->nextLevel!=NULL){
				if(strcmp(modify->nextLevel->type, "direct")==0){
					direct(modify->nextLevel, addMe);
				}	
				if((strlen(modify->nextLevel->type)>=7)&&(strncmp(modify->nextLevel->type,"assoc:", 5)==0)){
					//	printf("I came here");
						SA(modify->nextLevel, addMe, modify->nextLevel->assoc);
					}
				if((strlen(modify->nextLevel->type)==5)&& (strcmp(modify->nextLevel->type,"assoc")==0)){
						FA(modify->nextLevel, addMe);
					}
			}
		//		printf("sadI came here %d\n", j);
				/*it's full. Call LRU*/
				modify->total_misses++;
				if(modify->LRU!=NULL){
					index2=getLRU(modify->LRU);}
				else{
					index2=dequeue(modify->FIFO);
				}
			//	printf("\n num: %d\n", index2);
				modify->blocks[index2][0].tag=tag1;
				modify->blocks[index2][0].offSet=boff;
				if(modify->LRU!=NULL){
					SLInsert(modify->LRU, index2);
				}
				else{
					enqueue(modify->FIFO,index2);
				}
				return;
		}
	}
}


/*Free functions*/
void destroySet(int lines, Cache destroyMe, int assoc){
	if(destroyMe->nextLevel!=NULL){
		/*need to determine kind*/
				if(strcmp(destroyMe->nextLevel->type, "direct")==0){
					destroyDirect(destroyMe->nextLevel->numofSet, destroyMe->nextLevel, destroyMe->nextLevel->assoc);
				}	
				if((strlen(destroyMe->nextLevel->type)>=7)&&(strncmp(destroyMe->nextLevel->type,"assoc:", 5)==0)){
				//	printf("I came here");
					destroySet(destroyMe->nextLevel->numofSet, destroyMe->nextLevel, destroyMe->nextLevel->assoc);
				}
				if((strlen(destroyMe->nextLevel->type)==5)&& (strcmp(destroyMe->nextLevel->type,"assoc")==0)){
					destroySet(destroyMe->nextLevel->numofSet, destroyMe->nextLevel, destroyMe->nextLevel->assoc);
				}
			}

	for (int i=0;i<lines;i++){
		if((strlen(destroyMe->type)>=7)&&(strncmp(destroyMe->type,"assoc:", 5)==0)){
			/*destroy dll for each individual set*/
				if(destroyMe->blocks[i]->LRU!=NULL){
					SLDestroy(destroyMe->blocks[i]->LRU);
					destroyMe->blocks[i]->LRU=NULL;
				}
				else{
					destroyQueue(destroyMe->blocks[i]->FIFO);
					destroyMe->blocks[i]->FIFO=NULL;
				}
			/*destroy queue for each individual set*/
		}
		free(destroyMe->blocks[i]);
	}
	/*destroy FA's fifo or lru*/
	if((strcmp(destroyMe->type,"FA")==0)||((strlen(destroyMe->type)==5)&& (strcmp(destroyMe->type,"assoc")==0))){
		/*only for FA*/
		if(destroyMe->LRU!=NULL){
					SLDestroy(destroyMe->LRU);
					destroyMe->LRU=NULL;
				}
		else{
				destroyQueue(destroyMe->FIFO);
				destroyMe->FIFO=NULL;
			}
	}
	free(destroyMe->blocks);
	free(destroyMe);
	destroyMe=NULL;
	return;

}
void destroyDirect(int lines, Cache destroyMe, int assoc){
	if(destroyMe->nextLevel!=NULL){
		/*need to determine kind*/
				if(strcmp(destroyMe->nextLevel->type, "direct")==0){
					destroyDirect(destroyMe->nextLevel->numofSet, destroyMe->nextLevel, destroyMe->nextLevel->assoc);
				}	
				if((strlen(destroyMe->nextLevel->type)>=7)&&(strncmp(destroyMe->nextLevel->type,"assoc:", 5)==0)){
				//	printf("I came here");
					destroySet(destroyMe->nextLevel->numofSet, destroyMe->nextLevel, destroyMe->nextLevel->assoc);
				}
				if((strlen(destroyMe->nextLevel->type)==5)&& (strcmp(destroyMe->nextLevel->type,"assoc")==0)){
					destroySet(destroyMe->nextLevel->numofSet, destroyMe->nextLevel, destroyMe->nextLevel->assoc);
				}
			}
	for (int i=0;i<lines;i++){
		free(destroyMe->blocks[i]);
	}
	free(destroyMe->blocks);
	free(destroyMe);
	destroyMe=NULL;
	return;

}