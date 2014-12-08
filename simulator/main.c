#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h> /*for tolower function*/
#include "cache_sim.c"

/*Fully working cache simulator for 211
Simulates a cache read for L1, L2, and/or L3
Conado Uraga*/

/*converts the replace alg to lower...idk if it's supposed to be upper or lower
So change the fifo and lifo specification to lower since I use lower cases
in cache_sim.c*/
char convert(char *string){
	/*simple function to convert a char in a string to lower using pointer aritmetic*/
	while(*string!='\0'){
		*string=tolower(*string);
		string++;
	}
	return *string;
}
int powerOfTwo(int check){
	/*uses binary subtraction to find if it's a power of two or not
	return 1 if it is, 0 if not.
	0 is not a power of two. Ignore it, thus the not for first part
	Example. Say check is 4:
	it's represented by: 0100. check-1 is then:
	0011. Do an and operation, and it should be all zeroes
	if it's not all zeroes, it's not a power of two*/
	return ((check != 0) && !(check & (check - 1)));
}


int main(int argc, char *argv[]){
	/*let's assume they input everything correctly first*/
	size_t string;
	int checkPower;
	int L1size=0;
	int L2size=0;
	int L3size=0;
	int blockSize=0;
	int assoc=0;
	static int L2assocN=0;
	static int L3assocN=0;
	int checkerC=1;
	Cache L1=NULL;
	Cache L2=NULL;
	Cache L3=NULL;
	Cache ParallelL1=NULL;
	char n[1];
	char L2n[1];
	char L3n[1];
	char *replacAlg;
	//printf("%s",replacAlg);
	char *L1assoc=NULL;
	char *L2assoc=NULL;
	char *L3assoc=NULL;
	char passoc[]="FA";
	char *Parallelassoc=passoc;
	char helper[]="-h";
	FILE *pathFile=NULL; /*trace file holder*/
	/*cache-sim [-h] -l1size <L1 size> -l1assoc <L1 assoc> -l2size <L2 size>
	-l2assoc <L2 assoc> -l3size <L3 size> -l3assoc <L3 assoc> <block size> <replace alg>
	<trace file>
	*/
	if(strcmp(argv[1], helper)==0|| (argc!=8 && argc!=12 && argc!=16)){
		fprintf(stderr,"In order to run this program\n" 
			"you need to input the correct/following args together:\n"
			"Note: You don't need all three caches.\n"
			"Program will run with only L1 or L1 and L2 only\n"
			"-l1size <L1 size> -l1assoc <L1 assoc>\n"
			"-l2size <L2 size> -l2assoc <L2 assoc>\n"
			"-l3size <L3 size> -l3assoc <L3 assoc> <block size> <replace alg> <trace file>\n");
		return 0;
	}
	/*check for format error while converting all inputs to
	its respected format*/
	if(argc==8){
		if(sscanf(argv[5], "%i", &blockSize)!=1){
				fprintf(stderr,"ERROR: <Needs an integer for blockSize>\n");
				return -1;
		}
		blockSize=atoi(argv[5]);
		replacAlg=argv[6];
		convert(replacAlg);
		if((strcmp(replacAlg, "fifo")!=0)&&(strcmp(replacAlg, "lru")!=0)){
			fprintf(stderr,"ERROR: <Must enter fifo or lru>\n");
			return -1;
		}
		if((checkPower=powerOfTwo(blockSize))==0){
			fprintf(stderr,"ERROR: <Please enter a power of two for BlockSize>\n");
			return -1;
		}
		if(!(pathFile=fopen(argv[7], "r"))){ 
			fprintf(stderr,"ERROR: <file doesn't exist>\n");
			return -1;
		}
		if(strcmp(argv[1], "-l1size")!=0){
			fprintf(stderr,"ERROR: <Please enter the correct argument>\n");
			return -1;
		}
		if(strcmp(argv[1], "-l1size")==0){
			if(sscanf(argv[2], "%i", &L1size)!=1){
				fprintf(stderr,"ERROR: <After the size, it must be an integer>\n");
				return -1;
			}
			L1size=atoi(argv[2]);
			if((checkPower=powerOfTwo(L1size))==0){
				fprintf(stderr,"ERROR: <Please enter a power of two for L1>\n");
				return -1;
			}
		}
		if(strcmp(argv[3], "-l1assoc")!=0){
			fprintf(stderr,"ERROR: <Please enter the correct argument>\n");
			return -1;
		}
		if(strcmp(argv[3], "-l1assoc")==0){
			L1assoc=argv[4];
		}

	}
	/*for all two caches. It takes the block size and replaceAlg*/
	if(argc==12){
		if(sscanf(argv[9], "%i", &blockSize)!=1){
			fprintf(stderr,"ERROR: <Needs an integer for blockSize>\n");
			return -1;
		}

		blockSize=atoi(argv[9]);
		replacAlg=argv[10];
		convert(replacAlg);
		if((strcmp(replacAlg, "fifo")!=0)&&(strcmp(replacAlg, "lru")!=0)){
			fprintf(stderr,"ERROR: <Must enter fifo or lru>\n");
			return -1;
		}
		if((checkPower=powerOfTwo(blockSize))==0){
			fprintf(stderr,"ERROR: <Please enter a power of two for BlockSize>\n");
			return -1;
		}
		if(!(pathFile=fopen(argv[11], "r"))){ 
			fprintf(stderr,"ERROR: <file doesn't exist>\n");
			return -1;
		}
	}
	/*for all three caches. It takes the block size and replaceAlg*/
	if(argc==16){
		if(sscanf(argv[13], "%i", &blockSize)!=1){
				fprintf(stderr,"ERROR: <Needs an integer for blockSize>\n");
				return -1;
			}
		blockSize=atoi(argv[13]);
		replacAlg=argv[14];
		convert(replacAlg);
		if((strcmp(replacAlg, "fifo")!=0)&&(strcmp(replacAlg, "lru")!=0)){
			fprintf(stderr,"ERROR: <Must enter fifo or lru>\n");
			return -1;
		}
		if((checkPower=powerOfTwo(blockSize))==0){
			fprintf(stderr,"ERROR: <Please enter a power of two for BlockSize>\n");
			return -1;
		}
		if(!(pathFile=fopen(argv[15], "r"))){ 
			fprintf(stderr,"ERROR: <file doesn't exist>\n");
			return -1;
		}
	}
	/*Only two caches? Create L1 and L2. Don't create L3*/
	if(argc==12){
		if(strcmp(argv[1], "-l1size")!=0){
			fprintf(stderr,"ERROR: <Please enter the correct argument>\n");
			return -1;
		}
		if(strcmp(argv[1], "-l1size")==0){
			if(sscanf(argv[2], "%i", &L1size)!=1){
				fprintf(stderr,"ERROR: <After the size, it must be an integer>\n");
				return -1;
			}
			L1size=atoi(argv[2]);
			if((checkPower=powerOfTwo(L1size))==0){
				fprintf(stderr,"ERROR: <Please enter a power of two for L1>\n");
				return -1;
			}
		}
		if(strcmp(argv[3], "-l1assoc")!=0){
			fprintf(stderr,"ERROR: <Please enter the correct argument>\n");
			return -1;
		}
		if(strcmp(argv[3], "-l1assoc")==0){
			L1assoc=argv[4];
		}
		if(strcmp(argv[5], "-l2size")!=0){
			fprintf(stderr,"ERROR: <Please enter the correct argument>\n");
			return -1;
		}
		if(strcmp(argv[5], "-l2size")==0){
			if(sscanf(argv[6], "%i", &L2size)!=1){
				fprintf(stderr,"ERROR: <Needs an integer for size of L2>\n");
				return -1;
			}
			L2size=atoi(argv[6]);
			if((checkPower=powerOfTwo(L2size))==0){
				fprintf(stderr,"ERROR: <Please enter a power of two for L2>\n");
				return -1;
			}
		}
		if(strcmp(argv[7], "-l2assoc")!=0){
			fprintf(stderr,"ERROR: <Please enter the correct argument>\n");
			return -1;
		}
		if(strcmp(argv[7], "-l2assoc")==0){
			L2assoc=argv[8];
			if(strcmp(L2assoc,"direct")==0){
				L2assocN=1;
				L2=malloc(sizeof(struct the_cache));
				L2->type=L2assoc;
				L2->assoc=L2assocN;
				createDirectCache(L2size, blockSize, L2assocN, L2);
				initializeDirect(L2->numofSet, L2, 1);
			//createDirectCache(L1size, blockSize, assoc, &L1numOfSet, &L1BlockOffSet, &L1Tag, &L1setIndex);
			}
			if((strlen(L2assoc)>=7)&&(strncmp(L2assoc,"assoc:", 5)==0)){
				strcpy(L2n,&L2assoc[6]);
				if(sscanf(L2n, "%i", &L2assocN)!=1){
					fprintf(stderr,"ERROR: <After the assoc:,please input a number>\n");
					return -1;
				}
				L2assocN=atoi(L2n);
				//strncpy(L2->type, L2assoc, 5);
				//printf("\nI'm not supposed to come here L2 assoc is: %d\n",L2assocN);
				L2=malloc(sizeof(struct the_cache));
				L2->type=L2assoc;
				L2->assoc=L2assocN;
				createSA(L2size, blockSize, L2assocN, L2);
				initializeSet(L2->numofSet, L2, L2assocN, replacAlg);
			}
			if((strlen(L2assoc)==5)&& (strcmp(L2assoc,"assoc")==0)){
				//printf("I came here and only here\n");
				L2assocN=1;
				L2=malloc(sizeof(struct the_cache));
				L2->type=L2assoc;
				L2->assoc=L2assocN;
				createFA(L2size, blockSize, L2assocN, L2);
				initializeSet(L2->numofSet, L2, L2assocN, replacAlg);
			}
		}
	
	}
	/*if it's all three caches*/
	if(argc==16){
		if(strcmp(argv[1], "-l1size")!=0){
			fprintf(stderr,"ERROR: <Please enter the correct argument>\n");
			return -1;
		}
		if(strcmp(argv[1], "-l1size")==0){
			if(sscanf(argv[2], "%i", &L1size)!=1){
				fprintf(stderr,"ERROR: <After the size, it must be an integer>\n");
				return -1;
			}
			L1size=atoi(argv[2]);
			if((checkPower=powerOfTwo(L1size))==0){
				fprintf(stderr,"ERROR: <Please enter a power of two for L1>\n");
				return -1;
			}
		}
		if(strcmp(argv[3], "-l1assoc")!=0){
			fprintf(stderr,"ERROR: <Please enter the correct argument>\n");
			return -1;
		}
		if(strcmp(argv[3], "-l1assoc")==0){
			L1assoc=argv[4];
		}
		if(strcmp(argv[5], "-l2size")!=0){
			fprintf(stderr,"ERROR: <Please enter the correct argument>\n");
			return -1;
		}
		if(strcmp(argv[5], "-l2size")==0){
			if(sscanf(argv[6], "%i", &L2size)!=1){
				fprintf(stderr,"ERROR: <Needs an integer for size of L2>\n");
				return -1;
			}
			L2size=atoi(argv[6]);
			if((checkPower=powerOfTwo(L2size))==0){
				fprintf(stderr,"ERROR: <Please enter a power of two for L2>\n");
				return -1;
			}
		}
		if(strcmp(argv[7], "-l2assoc")!=0){
			fprintf(stderr,"ERROR: <Please enter the correct argument>\n");
			return -1;
		}
		if(strcmp(argv[7], "-l2assoc")==0){
			L2assoc=argv[8];
			if(strcmp(L2assoc,"direct")==0){
				L2assocN=1;
				L2=malloc(sizeof(struct the_cache));
				L2->type=L2assoc;
				L2->assoc=L2assocN;
				createDirectCache(L2size, blockSize, L2assocN, L2);
				initializeDirect(L2->numofSet, L2, 1);
			}
			if((strlen(L2assoc)>=7)&&(strncmp(L2assoc,"assoc:", 5)==0)){
				strcpy(L2n,&L2assoc[6]);
				if(sscanf(L2n, "%i", &L2assocN)!=1){
					fprintf(stderr,"ERROR: <Needs an integer after assoc: for L2>\n");
					return -1;
				}
				L2assocN=atoi(L2n);
				L2=malloc(sizeof(struct the_cache));
				L2->type=L2assoc;
				L2->assoc=L2assocN;
				createSA(L2size, blockSize, L2assocN, L2);
				initializeSet(L2->numofSet, L2, L2assocN, replacAlg);
			}
			if((strlen(L2assoc)==5)&& (strcmp(L2assoc,"assoc")==0)){
				L2assocN=1;
				L2=malloc(sizeof(struct the_cache));
				L2->type=L2assoc;
				L2->assoc=L2assocN;
				createFA(L2size, blockSize, L2assocN, L2);
				initializeSet(L2->numofSet, L2, L2assocN, replacAlg);
			}
		}
		/*convert size to integer. Should check if it's an integer*/
		if(strcmp(argv[9], "-l3size")!=0){
			fprintf(stderr,"ERROR: <Please enter the correct argument>\n");
			return -1;
		}
		if(strcmp(argv[9], "-l3size")==0){
			if(sscanf(argv[10], "%i", &L3size)!=1){
				fprintf(stderr,"ERROR: <Needs an integer for L3 size>\n");
				return -1;
			}
			L3size=atoi(argv[10]);
			if((checkPower=powerOfTwo(L3size))==0){
				fprintf(stderr,"ERROR: <Please enter a power of two for L3>\n");
				return -1;
			}
		}
		if(strcmp(argv[11], "-l3assoc")!=0){
			fprintf(stderr,"ERROR: <Please enter the correct argument>\n");
			return -1;
		}
		if(strcmp(argv[11], "-l3assoc")==0){
			L3assoc=argv[12];
			/*creating a direct cache for L3 right away*/
			if(strcmp(L3assoc,"direct")==0){
				L3assocN=1;
				L3=malloc(sizeof(struct the_cache));
				L3->assoc=L3assocN;
				L3->type=L3assoc;
				createDirectCache(L3size, blockSize, L3assocN, L3);
				initializeDirect(L3->numofSet, L3, 1);
			}
			/*creating a n-way assoc cache for L3 right away*/
			if((strlen(L3assoc)>=7)&&(strncmp(L3assoc,"assoc:", 5)==0)){
				strcpy(L3n,&L3assoc[6]);
				if(sscanf(L3n, "%i", &L3assocN)!=1){
					fprintf(stderr,"ERROR: <Needs an integer after assoc:>\n");
					return -1;
				}
				L3assocN=atoi(L3n);
				L3=malloc(sizeof(struct the_cache));
				L3->type=L3assoc;
				L3->assoc=L3assocN;
				createSA(L3size, blockSize, L3assocN, L3);
				initializeSet(L3->numofSet, L3, L3assocN, replacAlg);
			}
			/*creating a fully assoc cache for L3 right away*/
			if((strlen(L3assoc)==5)&& (strcmp(L3assoc,"assoc")==0)){
				L3assocN=1;
				L3=malloc(sizeof(struct the_cache));
				L3->type=L3assoc;
				L3->assoc=L3assocN;
				createFA(L3size, blockSize, L3assocN, L3);
				initializeSet(L3->numofSet, L3, L3assocN, replacAlg);
			}
		}
	}
	/*If L1 is a direct cache, initialize, create, do the same for parallel*/
	if(strcmp(L1assoc,"direct")==0){
		assoc=1;
		L1=malloc(sizeof(struct the_cache));
		ParallelL1=malloc(sizeof(struct the_cache));
		createDirectCache(L1size, blockSize, assoc, L1);
		createFA(L1size, blockSize, 1, ParallelL1);
		initializeDirect(L1->numofSet, L1, 1);
		ParallelL1->type=Parallelassoc;
		initializeSet(ParallelL1->numofSet, ParallelL1, 1, "lru");
	}
	/*If L1 is a n-way assoc cache, initialize, create, do the same for parallel*/
	if((strlen(L1assoc)>=7)&&(strncmp(L1assoc,"assoc:", 5)==0)){
		strcpy(n,&L1assoc[6]);
		if(sscanf(n, "%i", &assoc)!=1){
			fprintf(stderr,"ERROR: <Needs an integer after assoc>\n");
			return -1;
		}
		assoc=atoi(n);
		L1=malloc(sizeof(struct the_cache));
		L1->type=L1assoc;
		L1->assoc=assoc;
		ParallelL1=malloc(sizeof(struct the_cache));
		createSA(L1size, blockSize, assoc, L1);
		createFA(L1size, blockSize, 1, ParallelL1);
		ParallelL1->type=Parallelassoc;
		initializeSet(L1->numofSet, L1, assoc, replacAlg);
		initializeSet(ParallelL1->numofSet, ParallelL1, 1, "lru");
	}
	/*If L1 is a fully assoc cache, initialize, create, do the same for parallel*/
	if((strlen(L1assoc)==5)&& (strcmp(L1assoc,"assoc")==0)){
			//printf("L3 I came here and only here\n");
			assoc=1;
			L1=malloc(sizeof(struct the_cache));
			L1->type=L1assoc;
			createFA(L1size, blockSize, 1, L1);
			initializeSet(L1->numofSet, L1, 1, replacAlg);
			ParallelL1=malloc(sizeof(struct the_cache));
			createFA(L1size, blockSize, 1, ParallelL1);
			ParallelL1->type=Parallelassoc;
			initializeSet(ParallelL1->numofSet, ParallelL1, 1, "lru");
		}
	/*Set each cache Level link to the next one. Parrallel L1 gets nothing*/

	ParallelL1->nextLevel=NULL;
	if(L2!=NULL){
		L1->nextLevel=L2;
		if(L3!=NULL){
			L2->nextLevel=L3;
			L3->nextLevel=NULL;
		}
	}

	/*L1 is direct? Call direct function*/
	if(strcmp(L1assoc,"direct")==0){
	do{
			checkerC=fscanf(pathFile,"%zx[^\n]\n", &string);
			/*if there's an invalid format, we stop right away*/
			if(checkerC==0){
				break;
			}
			/*reached the end of the file? We have to break*/
			if(checkerC==EOF){
				break;
			}
			direct(L1, string);
			FA(ParallelL1,string);
		}
		while(checkerC==1);

	}
	/*if L1 is n-way assoc, call n-way assoc function*/
	if((strlen(L1assoc)>=7)&&(strncmp(L1assoc,"assoc:", 5)==0)){
	do{
			checkerC=fscanf(pathFile,"%zx[^\n]\n", &string);
			/*if there's an invalid format, we stop right away*/
			if(checkerC==0){
				break;
			}
			/*reached the end of the file? We have to break*/
			if(checkerC==EOF){
				break;
			}
			SA(L1, string, assoc);
			FA(ParallelL1,string);
		}
		while(checkerC==1);

	}
	/*if L1 is assoc, fill it in*/
	if((strlen(L1assoc)==5)&& (strcmp(L1assoc,"assoc")==0)){
	do{
			checkerC=fscanf(pathFile,"%zx[^\n]\n", &string);
			/*if there's an invalid format, we stop right away*/
			if(checkerC==0){
				break;
			}
			/*reached the end of the file? We have to break*/
			if(checkerC==EOF){
				break;
			}
			FA(L1, string);
			FA(ParallelL1,string);
		}
		while(checkerC==1);

	}
	/*my purposes only:*/
	/*printf("Memory accesses: %i\nL1 Cache hits: %i\nTotal misses direct: %d\nTotal misses: %i\nL1 Cache conflict misses: %i\n L1 Cache cold Misses: %i\n L1 Cache capacity Misses: %i\n"
		,L1->memory_access, L1->hits,L1->total_misses, L1->conflict_miss+L1->cold_misses,L1->conflict_miss,L1->cold_misses,L1->capacity_miss);
	printf("\nL2 Memory accesses: %i\nL2 Cache hits: %i\nTotal misses direct: %d\n Total misses: %i\nL2 Cache conflict misses: %i\n L2 Cache cold Misses: %i\n L2 Cache capacity Misses: %i\n"
		, L2->memory_access,L2->hits,L2->total_misses,L2->conflict_miss+L2->cold_misses+L2->capacity_miss,L2->conflict_miss,L2->cold_misses,L2->capacity_miss);
	printf("\nL3 Memory accesses: %i\nL3 Cache hits: %i\nTotal misses direct: %d\n Total misses: %i\nL3 Cache conflict misses: %i\n L3 Cache cold Misses: %i\n L3 Cache capacity Misses: %i\n"
		, L3->memory_access,L3->hits,L3->total_misses,L3->conflict_miss+L3->cold_misses+L3->capacity_miss,L3->conflict_miss,L3->cold_misses,L3->capacity_miss);
	printf("Parallel L1\nL1 Memory accesses: %i\nL1 Cache hits: %i\nTotal misses direct: %d\n Total misses computed: %i\nL1 Cache conflict misses: %i\n L1 Cache cold Misses: %i\n L1 Cache capacity Misses: %i\n"
		, ParallelL1->memory_access,ParallelL1->hits,ParallelL1->total_misses,ParallelL1->conflict_miss+ParallelL1->cold_misses+ParallelL1->capacity_miss,ParallelL1->conflict_miss,ParallelL1->cold_misses,ParallelL1->capacity_miss);*/

	/*Compute the remainder of misses for L1*/
	L1->capacity_miss=ParallelL1->total_misses-L1->cold_misses;
	L1->conflict_miss=L1->total_misses-L1->capacity_miss-L1->cold_misses;
	/*format for the autograder:*/
	if(argc==16){
		printf("Memory accesses: %i\n", L1->memory_access);
		printf("L1 Cache hits: %i\n", L1->hits);
		printf("L1 Cache miss: %i\n", L1->total_misses);
		printf("L2 Cache hits: %i\n", L2->hits);
		printf("L2 Cache miss: %i\n", L2->total_misses);
		printf("L3 Cache hits: %i\n", L3->hits);
		printf("L3 Cache miss: %i\n", L3->total_misses);
		printf("L1 Cold misses: %i\n", L1->cold_misses);
		printf("L2 Cold misses: %i\n", L2->cold_misses);
		printf("L3 Cold misses: %i\n", L3->cold_misses);
		printf("L1 Conflict misses: %i\n", L1->conflict_miss);
		printf("L1 Capacity misses: %i\n", L1->capacity_miss);
	}
	if(argc==12){
		printf("Memory accesses: %i\n", L1->memory_access);
		printf("L1 Cache hits: %i\n", L1->hits);
		printf("L1 Cache miss: %i\n", L1->total_misses);
		printf("L2 Cache hits: %i\n", L2->hits);
		printf("L2 Cache miss: %i\n", L2->total_misses);
		printf("L1 Cold misses: %i\n", L1->cold_misses);
		printf("L2 Cold misses: %i\n", L2->cold_misses);
		printf("L1 Conflict misses: %i\n", L1->conflict_miss);
		printf("L1 Capacity misses: %i\n", L1->capacity_miss);
	}
	if(argc==8){
		printf("Memory accesses: %i\n", L1->memory_access);
		printf("L1 Cache hits: %i\n", L1->hits);
		printf("L1 Cache miss: %i\n", L1->total_misses);
		printf("L1 Cold misses: %i\n", L1->cold_misses);
		printf("L1 Conflict misses: %i\n", L1->conflict_miss);
		printf("L1 Capacity misses: %i\n", L1->capacity_miss);
	}
	/*just for analysis purposes:*/
	/*float hitRate= (float)(L1->hits)/(float)(L1->memory_access);
	float missRate=(float)( L1->total_misses)/(float)(L1->memory_access);
	float hitRateL2= (float)(L2->hits)/(float)(L2->memory_access);
	printf("L1 hitRate %f\nL1 Miss rate %f\n L2' hit: %f\n",hitRate,missRate, hitRateL2);*/
	/**/

	/*finally, free everything else*/
	if(strcmp(L1assoc,"direct")==0){
		destroyDirect(L1->numofSet, L1, 1);
		destroySet(ParallelL1->numofSet, ParallelL1, 1);
	}
	if((strlen(L1assoc)>=7)&&(strncmp(L1assoc,"assoc:", 5)==0)){
		destroySet(L1->numofSet, L1, assoc);
		destroySet(ParallelL1->numofSet, ParallelL1, 1);
	}
	if((strlen(L1assoc)==5)&& (strcmp(L1assoc,"assoc")==0)){
			destroySet(L1->numofSet, L1, assoc);
			destroySet(ParallelL1->numofSet, ParallelL1, 1);
		}
	fclose(pathFile);
	return 0;
}