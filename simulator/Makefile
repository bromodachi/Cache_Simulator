CC = gcc
CCFLAGS = -std=c99 -Wall -pedantic -g
cache: cache_sim.c cache_sim.h main.c LRUDLL.h LRUDLL.c arrayQueue.h arrayQueue.c
	$(CC) $(CCFLAGS) main.c -o cache-sim -lm
clean:
	rm -f cache_sim *.o