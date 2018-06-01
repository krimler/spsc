#pragma once
#include <stdbool.h>
#include <memory.h>
typedef struct Buffer {
    char *array;
    bool busy;
} Buffer;

typedef struct Asgard {
    char *data_ptr;
    Ms ms;
    Buffer *buffers;
} Asgard;

/* returns 'num' number of allocated buffer of size 1024 at once.
 * They are guaranteed to be sequential.
 * Each buffer is actuall of size 1023. The last 1024th byte is used as delimiter for safety.
 */
char  *shm_alloc(int num);

void shm_dealloc(int num, void *mem); /* frees up the entnire buffer. */

int asgard_init(MRole role);

void init_memory(int buffers);

int get_offset(char *mem);
char *get_buffer(int offset);

#define DONE_TRANS "+DONE+"
