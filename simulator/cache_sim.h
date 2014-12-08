#ifndef CACHE_SIM_H
#define CACHE_SIM_H

#include <stdlib.h>
#include <stdio.h>
#include "LRUDLL.h"
#include "LRUDLL.c"
#include "arrayQueue.h"
#include "arrayQueue.c"

struct cache_array
{
    int valid;
	long long tag;
	long long indexSet;
	long long offSet;
	/*DLL for LRU ONLY FOR SA*/
	SortedListPtr LRU;
	/*An array based FIFO only for SA*/
	queue FIFO;
//	CacheArray* set[];
};
typedef struct cache_array CacheArray;
struct the_cache
{
    int valid;
	int miss;
	int cold_misses;
	int capacity_miss;
	int conflict_miss;
	int total_misses;
	int memory_access;
	int hits;
	/*HM stands for how many
	**How many index, block, tag*/
	int HMblock;
	int HMindex;
	int HMtag;
	int numofSet;
	/*For the nextLevel cache only*/
	int assoc;
	char *type;
	/*Data structures for FA*/
	/*End of nextLevel cache only*/
	SortedListPtr LRU;
	/*An array based FIFO only for FA*/
	queue FIFO;
	/*Dynamically allocated 2D array*/
	CacheArray **blocks;
	struct the_cache *nextLevel; /*L2 or L3. L2 will be attached to L1, etc*/
};
typedef struct the_cache* Cache;

int initializeDirect(int lines, Cache test, int assoc);
int initializeSet(int lines, Cache test, int assoc, char *kind);
void createDirectCache(int cacheSize, int blockSi, int assoc, Cache create);
void createFA(int cacheSize, int blockSi, int assoc, Cache create);
void createSA(int cacheSize, int blockSi, int assoc, Cache create);
long long extract(int howMany, int start, size_t address);
void direct(Cache modify, size_t addMe);
void SA(Cache modify, size_t addMe, int assoc);
void FA(Cache modify, size_t addMe);
void destroySet(int lines, Cache destroyMe, int assoc);
void destroyDirect(int lines, Cache destroyMe, int assoc);

#endif 