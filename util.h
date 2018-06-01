#pragma once
#include <stdbool.h>
#include <stdlib.h>

/* s is source string,, times specifies how many such consecutive buffers are available of size 1024. d is its readable hash. */
void readable_hash(char *s, int slen, int times, char *d, int dlen);

/* s is source string, d is its readable hash, returns bool if d and hash(s) matches. */
bool does_string_intact(char *s, int slen, int num, char *d);

char *TimeStamp(void);
#define BIG_ENOUGH 1024
#define START_MARKER "<CEEEFACE>"
#define END_MARKER "<DEADBEEF>"
#define START_WITH_ACK_MARKER "<CEEEFACE><ACK>"

void lock(volatile int *exclusion);

void unlock(volatile int *exclusion);

//#define DEBUG_ALLOC
#ifdef DEBUG_ALLOC
#define Calloc(NTIMES, NSIZE)    \
        ({void * ppp = calloc(NTIMES, NSIZE); printf("asgard calloc: <%s,  %d> %p\n", __FUNCTION__, __LINE__, ppp ); ppp;})

#define Free(x) printf("asgard free: <%s, %d> %p\n", __FUNCTION__, __LINE__, (x)); free((x));
#else
#define Calloc(NTIMES, NSIZE) calloc(NTIMES, NSIZE);
#define Free(x) free((x));
#endif

