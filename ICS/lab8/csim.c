#include "cachelab.h"
#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

// name: Zhu Han
// ID: 522031910213
typedef struct
{
    int valid; // valid bit
    int tag;   // tag
    int time;  // time stamp
} Line;        // cache line,since we only need to simulate the hits,misses and evictions,we don't need to store the data in the cache line

typedef struct
{
    int S;        // number of sets
    int E;        // number of lines per set
    int B;        // number of block bits
    Line **lines; // cache lines
} Cache;          // cache

Cache *cache;                            // cache
int hits = 0, misses = 0, evictions = 0; // statistics
int verbose = 0;                         // verbose mode
char *traceFile;                         // trace file name

void initCache(int s, int E, int b)
{
    int S = 1 << s, B = 1 << b;
    int i, j;
    cache = (Cache *)malloc(sizeof(Cache)); // allocate memory for cache
    cache->S = S;
    cache->E = E;
    cache->B = B;
    cache->lines = (Line **)malloc(S * sizeof(Line *)); // allocate memory for lines
    for (i = 0; i < S; i++)
    {
        cache->lines[i] = (Line *)malloc(E * sizeof(Line)); // allocate memory for each line
        for (j = 0; j < E; j++)
        {
            cache->lines[i][j].valid = 0;
            cache->lines[i][j].tag = -1;
            cache->lines[i][j].time = 0;
        }
    }
}

void freeCache()
{
    int S = cache->S;
    for (int i = 0; i < S; i++)
        free(cache->lines[i]);
    free(cache->lines);
    free(cache);
}

void update(int e, int s, int tag)// update the block in e'th line of s'th set with tag
{
    cache->lines[s][e].tag = tag;
    cache->lines[s][e].valid = 1;
    for (int i = 0; i < cache->E; i++)
        if (cache->lines[s][i].valid == 1)
            cache->lines[s][i].time++; // update time stamp
    cache->lines[s][e].time = 0;
}

int evictLineIndex(int s) // find the line index to evict in s'th set
{
    int maxTime = 0;
    int maxIndex = 0;
    for (int i = 0; i < cache->E; i++)
    {
        if (cache->lines[s][i].valid == 0) // find an empty line
            return i;
        if (cache->lines[s][i].time > maxTime) // find the line with the max time stamp,e.g. the LRU line
        {
            maxTime = cache->lines[s][i].time;
            maxIndex = i;
        }
    }
    evictions++;// eviction
    if (verbose)
        printf(" eviction");
    return maxIndex;
}

int findLineIndex(int s, int tag) // find the line index of tag in s'th set
{
    for (int i = 0; i < cache->E; i++)
        if (cache->lines[s][i].valid == 1 && cache->lines[s][i].tag == tag)
            return i;
    return -1; // miss
}

void updateCache(int s, int tag)
{
    int idx = findLineIndex(s, tag); // find the line index of tag in s'th set
    if (idx != -1)                   // hit
    {
        hits++;
        if (verbose)
            printf(" hit");
        update(idx, s, tag);
        return;
    }
    misses++;
    if (verbose)
        printf(" miss");
    int evictIndex = evictLineIndex(s); // miss, evict a line
    update(evictIndex, s, tag);         // update the cache
}

void getTrace(int s, int E, int b)
{
    FILE *file = fopen(traceFile, "r");
    if (!file)
        exit(-1);
    char operation; // operation type 'L' or 'S' or 'M'
    unsigned address;
    int size;
    while (fscanf(file, " %c %x,%d", &operation, &address, &size) > 0)
    {
        int tag = address >> (s + b);
        int sIndex = (address >> b) & ((1 << s) - 1);
        switch (operation)
        {
        case 'L': // load
            if (verbose)
                printf("L %x,%d", address, size);
            updateCache(sIndex, tag);
            break;
        case 'S': // store
            if (verbose)
                printf("S %x,%d", address, size);
            updateCache(sIndex, tag);
            break;
        case 'M': // modify = load + store
            if (verbose)
                printf("M %x,%d", address, size);
            updateCache(sIndex, tag);
            updateCache(sIndex, tag);
            break;
        default:
            break;
        }
        if (verbose)
        {
            printf("\n");
        }
    }
    fclose(file);
}

void printHelp()
{
    printf("Usage: ./csim [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n");
    printf("Options:\n");
    printf("  -h         Print this help message.\n");
    printf("  -v         Optional verbose flag.\n");
    printf("  -s <s>     Number of set index bits.\n");
    printf("  -E <E>     Associativity.\n");
    printf("  -b <b>     Number of block bits.\n");
    printf("  -t <tracefile>   Name of the valgrind trace to replay.\n");
    printf("\n");
    printf("Examples:\n");
    printf("  linux>  ./csim -s 4 -E 1 -b 4 -t traces/yi.trace\n");
    printf("  linux>  ./csim -v -s 8 -E 2 -b 4 -t traces/yi.trace\n");
}

int main(int argc, char **argv)
{
    char opt;
    int s = 0, E = 0, b = 0;
    while ((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1)
    {
        switch (opt)
        {
        case 'h':
            printHelp();
            break;
        case 'v':
            verbose = 1;
            break;
        case 's':
            s = atoi(optarg);
            break;
        case 'E':
            E = atoi(optarg);
            break;
        case 'b':
            b = atoi(optarg);
            break;
        case 't':
            traceFile = optarg;
            break;
        default:
            printHelp();
            exit(-1); // exit with error
        }
    }
    initCache(s, E, b);
    getTrace(s, E, b);
    freeCache();
    printSummary(hits, misses, evictions);
    return 0;
}
