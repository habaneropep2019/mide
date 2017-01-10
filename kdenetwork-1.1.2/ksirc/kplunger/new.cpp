#ifdef MEM_DEBUG

/*
 * Environment Variables control how kplunger logs info
 * KPLUNGER_LOG = 1 creates a massive log file logging each new/delete
 * define KPLUNGER_STATS to turn off stats generation
 */


#include <stddef.h>
#include <stdlib.h>
#include <iostream.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>

#include <hash_map>

#include "kplunger.h"

typedef struct Smem_info {
  char *id;
  size_t size;
  time_t when;
} mem_info;

void stats(int);

typedef hash_map<unsigned int, const mem_info*, hash<unsigned int> > aMap;
static aMap mapAllocs;
static bool delay_stats = false;
static FILE *MEMlogfile = 0x0;

void* operator new(size_t sizeofX){
    return operator new(sizeofX, "Unkown");
}

void* operator new(size_t sizeofX, char *id){
    mem_info *mi = 0x0;
    void *p = 0x0;

    delay_stats = true;
    
    static int set = 0;
    
    if(set == 0){
        if(getenv("KPLUNGER_LOG"))
            MEMlogfile = fopen("kplunger.log", "w");
        if(getenv("KPLUNGER_STATS") == 0x0){
            signal(SIGALRM, stats);
            alarm(10);
        }
        set = 1;
    }
//    printf("Did Malloc() ID: %s Bytes: %d\n", id, sizeofX);

    p = malloc(sizeofX);
    if(p == 0x0){
        fprintf(stderr, "new call failed for size: %d for id: %s\n", sizeofX, id);
    }
    mi = (mem_info *) malloc(sizeof(mem_info));
    if(mi == 0x0){
        fprintf(stderr, "new call failed for mem_info: %d for id: %s\n", sizeof(mem_info), id);
    }
    mi->id = id;
    mi->size = sizeofX;
    mi->when = time(NULL);

    if(MEMlogfile != 0)
        fprintf(MEMlogfile, "%d\t%d\t%x\t%s\tNew\n", mi->when, mi->size, p, mi->id);
    
    mapAllocs[(unsigned int) p] = mi;

    delay_stats = false;
    
    return p;
}

void* operator new[](size_t sizeofX){
    void *p = operator new(sizeofX, "Unkown[]");
//    printf("Did new[] ID: Unkown[] Bytes: %d P: %x\n", sizeofX, p);
    return p;
}

void* operator new[](size_t sizeofX, char *id){
    void *p = operator new(sizeofX, id);
//    printf("Did Malloc() ID: %s Bytes: %d P: %x\n", id, sizeofX, p);
    return p;
}

void operator delete(void *p){
    //    cerr << "Did free() ID: " << mapAllocs[(unsigned int) p] << "\n";
    /*
     * mem info lso freed since it's part of p
     */
    const mem_info *mi = mapAllocs[(unsigned int) p];
    if(MEMlogfile != 0){
        if(mi != 0x0) // In case mmem_info is invalid
            fprintf(MEMlogfile, "%d\t%d\t%x\t%s\tDelete\n", time(NULL), mi->size, p, mi->id);
        else
            fprintf(MEMlogfile, "%d\t%d\t%x\t%s\tDelete\n", time(NULL), -1, p, "UnRegistered");
    }
    mapAllocs.erase((unsigned int) p);
    free(p);
    free((void *)mi);
}

void operator delete[](void *p){
//  printf("Did delete[] ID: %s P: %x\n", mapAllocs[(unsigned int) p]->id, p);
    operator delete(p);
}


void stats(int){
    if(delay_stats == true){
        signal(SIGALRM, stats);
        alarm(1);
        return;
    }
    int bytes_allocated = 0;
    const mem_info *mi = 0x0;
    hash_map<const char*, int, hash<const char*> > numAllocs;
    aMap::const_iterator it = mapAllocs.begin();
    for(; it != mapAllocs.end(); ++it){
        mi = (*it).second;
        bytes_allocated += mi->size;
        numAllocs[mi->id] = numAllocs[mi->id]+1;
    }
    hash_map<const char *, int, hash<const char *> >::const_iterator it2 = numAllocs.begin();
    for(; it2 != numAllocs.end(); ++it2){
        fprintf(stderr, "%d %s: %d\n", time(NULL), (*it2).first, (*it2).second);
    }
    fprintf(stderr, "Size: %d\n", mapAllocs.size());
    fprintf(stderr, "Total Memory Used: %d\n",  bytes_allocated);
    signal(SIGALRM, stats);
    alarm(10);
}

#endif
